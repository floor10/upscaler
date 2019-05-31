/*******************************************************************************
 * Copyright (C) 2019 floor10
 *
 * SPDX-License-Identifier: MIT
 ******************************************************************************/

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include "gstupscaler.h"
#include "openvino_inference.h"

using namespace std;
using namespace InferenceEngine;

namespace {


CNNNetwork create_network(string path_to_model_xml) {
    CNNNetReader network_reader;
    network_reader.ReadNetwork(path_to_model_xml);
    network_reader.ReadWeights(path_to_model_xml.substr(0, path_to_model_xml.rfind('.')) + ".bin");
    network_reader.getNetwork().setBatchSize(BATCH_SIZE);
    return network_reader.getNetwork();
}

InputsDataMap get_configured_inputs(CNNNetwork &network) {
    InputsDataMap inputs_info(network.getInputsInfo());
    for (auto &input_info : inputs_info) {
        // input_info.second->setLayout(Layout::NCHW);
        // input_info.second->setPrecision(Precision::U8);
    }
    return inputs_info;
}

OutputsDataMap get_configured_outputs(CNNNetwork &network) {
    OutputsDataMap outputs_info = network.getOutputsInfo();
    for (auto &item : outputs_info) {
        item.second->setPrecision(Precision::FP32);
    }

    return outputs_info;
}

template <typename T>
void copy_image_into_blob(cv::Mat image, Blob::Ptr &blob) {
    T* blob_data = blob->buffer().as<T*>();

    const auto &dims = blob->getTensorDesc().getDims();
    size_t channels_number = dims[1];
    size_t height = dims[2];
    size_t width = dims[3];
    size_t image_size = width * height;

    for (size_t c = 0; c < channels_number; c++) {
        for (size_t  h = 0; h < height; h++) {
            for (size_t w = 0; w < width; w++) {
                blob_data[c * width * height + h * width + w] = image.at<cv::Vec3b>(h, w)[c];
            }
        }
    }
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
    size_t height = dims[2];
    size_t width = dims[3];
    size_t image_size = width * height;

    std::vector<cv::Mat> imgPlanes = {
        cv::Mat(height, width, CV_32FC1, blob_data),
        cv::Mat(height, width, CV_32FC1, &(blob_data[image_size])),
        cv::Mat(height, width, CV_32FC1, &(blob_data[image_size * 2]))
    };

    cv::Mat resultImg;
    cv::merge(imgPlanes, resultImg);
    resultImg.convertTo(resultImg, CV_8UC3, 255);

    size_t buffer_size = resultImg.total() * resultImg.elemSize();
    memcpy(static_cast<void*>(info.data), static_cast<void*>(resultImg.data), buffer_size);
    gst_memory_unmap(memory, &info);
}

} // namespace

OpenVinoInference::OpenVinoInference(string path_to_model_xml) {
    this->_plugin = InferencePlugin(PluginDispatcher().getSuitablePlugin(TargetDevice::eCPU));

    this->_network = create_network(path_to_model_xml);
    this->_inputs_info = get_configured_inputs(_network);
    this->_outputs_info = get_configured_outputs(_network);

    this->_executable_network = this->_plugin.LoadNetwork(_network, {});
    this->_infer_request = this->_executable_network.CreateInferRequest();
}

cv::Mat OpenVinoInference::resize_by_opencv(const GstMapInfo &image, size_t width, size_t height) {
    cv::Mat resized_image;
    cv::Mat in_frame_mat_rgb(540, 960, CV_8UC3, image.data);
    cv::resize(in_frame_mat_rgb, resized_image, cv::Size(width, height), 0, 0, cv::INTER_CUBIC);
    return resized_image;
}

void OpenVinoInference::run(GstMemory *original_image, GstMemory *result_image) {
    GstMapInfo image_map_info;
    cv::Mat input_image_mat;

    if (!gst_memory_map(original_image, &image_map_info, GST_MAP_READ))
        GST_ERROR("Memory map failed");

    for (auto &input_info : this->_inputs_info) {
        string input_name = input_info.first;
        Blob::Ptr input_blob = this->_infer_request.GetBlob(input_name);
        SizeVector blob_size = input_blob->getTensorDesc().getDims();
        size_t height = blob_size[2];
        size_t width = blob_size[3];
        input_image_mat = resize_by_opencv(image_map_info, width, height);
        copy_image_into_blob<float_t>(input_image_mat, input_blob);
    }

    gst_memory_unmap(original_image, &image_map_info);

    this->_infer_request.Infer();

    string output_layer_name = this->_outputs_info.begin()->first;
    Blob::Ptr output_blob = this->_infer_request.GetBlob(output_layer_name);
    copy_blob_into_image(output_blob, result_image);
}

InferenceFactory *create_openvino_inference(gchar *path_to_model_xml, GError **error) {
    InferenceFactory *inference_factory = nullptr;
    try {
        // FIXME: invalid error message in case of empty path_to_model_xml
        inference_factory = new InferenceFactory{new OpenVinoInference(path_to_model_xml)};
    } catch (const std::exception &exc) {
        g_set_error(error, 1, 1, "%s", exc.what());
    }
    return inference_factory;
}

void run_inference(GstUpScaler *upscaler, GstMemory *original_image, GstMemory *result_image) {
    if (!upscaler || !upscaler->inference || !upscaler->inference->openvino_inference) {
        // TODO:
        return;
    }
    upscaler->inference->openvino_inference->run(original_image, result_image);
}
