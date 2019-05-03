/*******************************************************************************
 * Copyright (C) 2019 floor10
 *
 * SPDX-License-Identifier: MIT
 ******************************************************************************/

#include "gstupscaler.h"
#include "openvino_inference.h"

using namespace std;
using namespace InferenceEngine;

namespace {

CNNNetwork create_network() {
    CNNNetReader network_reader;
    network_reader.ReadNetwork(path_to_model_with_name + ".xml");
    network_reader.ReadWeights(path_to_model_with_name + ".bin");
    network_reader.getNetwork().setBatchSize(BATCH_SIZE);
    return network_reader.getNetwork();
}

InputsDataMap get_configured_inputs(CNNNetwork &network) {
    InputsDataMap inputs_info(network.getInputsInfo());
    for (auto &input_info : inputs_info) {
        input_info.second->setLayout(Layout::NCHW);
        input_info.second->setPrecision(Precision::U8);
    }
    return inputs_info;
}

OutputsDataMap get_configured_outputs(CNNNetwork &network) {
    OutputsDataMap outputs_info = network.getOutputsInfo();
    DataPtr output_info = outputs_info.begin()->second;
    string output_name = outputs_info.begin()->first;
    output_info->setPrecision(Precision::FP32);
    return outputs_info;
}

void copy_image_into_blob(GstMemory *memory, Blob::Ptr &blob) {
    GstMapInfo info = {};
    gst_memory_map(memory, &info, GST_MAP_READ);

    auto blob_data = blob->buffer().as<PrecisionTrait<Precision::U8>::value_type *>();

    const auto &dims = blob->getTensorDesc().getDims();
    size_t channels_number = dims[1];
    size_t image_size = dims[3] * dims[2];
    for (size_t pid = 0; pid < image_size; pid++) {
        for (size_t ch = 0; ch < channels_number; ch++) {
            size_t index = ch * image_size + pid;
            blob_data[index] = info.data[index];
        }
    }
    gst_memory_unmap(memory, &info);
}

size_t get_blob_size(const Blob::Ptr &blob) {
    const auto &dims = blob->getTensorDesc().getDims();
    return dims[1] * dims[2] * dims[3];
}

void copy_blob_into_image(const Blob::Ptr &blob, GstMemory *memory) {
    GstMapInfo info = {};
    gst_memory_map(memory, &info, GST_MAP_WRITE);
    auto blob_data = blob->buffer().as<PrecisionTrait<Precision::FP32>::value_type *>();

    const auto &dims = blob->getTensorDesc().getDims();
    size_t channels_number = dims[1];
    size_t image_size = dims[3] * dims[2];
    for (size_t pid = 0; pid < image_size; pid++) {
        for (size_t ch = 0; ch < channels_number; ch++) {
            size_t index = ch * image_size + pid;
            info.data[index] = blob_data[index];
        }
    }
    gst_memory_unmap(memory, &info);
}

} // namespace

OpenVinoInference::OpenVinoInference() {
    this->network = create_network();
    this->plugin = InferencePlugin(PluginDispatcher().getSuitablePlugin(TargetDevice::eCPU));
    this->executable_network = plugin.LoadNetwork(network, {});

    this->_infer_request = executable_network.CreateInferRequest();
    this->_inputs_info = get_configured_inputs(network);
    this->_outputs_info = get_configured_outputs(network);
}

void OpenVinoInference::copy_images_into_blobs(GstMemory *resized_image, GstMemory *original_image) {
    // TODO: rewrite with more optimal code
    map<int, GstMemory *> memories = {{original_image->size, original_image}, {resized_image->size, resized_image}};
    for (auto &input_info : this->_inputs_info) {
        string input_name = input_info.first;

        Blob::Ptr input_blob = this->_infer_request.GetBlob(input_name);

        GstMemory *memory = memories[get_blob_size(input_blob)];
        copy_image_into_blob(memory, input_blob);
    }
}

void OpenVinoInference::run(GstMemory *original_image, GstMemory *resized_image, GstMemory *result_image) {
    copy_images_into_blobs(resized_image, original_image);

    this->_infer_request.Infer();

    string output_layer_name = this->_outputs_info.begin()->first;
    Blob::Ptr output_blob = this->_infer_request.GetBlob(output_layer_name);
    copy_blob_into_image(output_blob, result_image);
}

InferenceFactory *create_openvino_inference() { return new InferenceFactory{new OpenVinoInference()}; }

void run_inference(GstUpScaler *upscaler, GstMemory *original_image, GstMemory *resized_image,
                   GstMemory *result_image) {
    if (!upscaler || !upscaler->inference || !upscaler->inference->openvino_inference) {
        // TODO:
        return;
    }
    upscaler->inference->openvino_inference->run(original_image, resized_image, result_image);
}
