/*******************************************************************************
 * Copyright (C) 2019 floor10
 *
 * SPDX-License-Identifier: MIT
 ******************************************************************************/

#include "gstupscaler.h"
#include "openvino_inference.h"
#include <opencv2/opencv.hpp>
#include <fstream>
#include <Magick++.h>

void write_buffer_to_file(unsigned char* blob, size_t h, size_t w, std::string filename)
{
   // Initialise ImageMagick library
   Magick::InitializeMagick("oo");

   // Create Image object and read in from pixel data above
   Magick::Image image;
   image.read(w, h,"RGB", Magick::StorageType::CharPixel, blob);

   // Write the image to a file - change extension if you want a GIF or JPEG
   image.write(filename);
    // fstream ostr(filename, ios_base::out |  ios_base::binary);
    // ostr.write(reinterpret_cast<char*>(buffer), size);
    // ostr.close();
}

using namespace std;
using namespace InferenceEngine;

namespace {


CNNNetwork create_network(std::string path_to_model_xml) {
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
    DataPtr output_info = outputs_info.begin()->second;
    string output_name = outputs_info.begin()->first;
    output_info->setPrecision(Precision::FP32);
    return outputs_info;
}

void copy_image_into_blob(GstMemory *memory, Blob::Ptr &blob) {
    GstMapInfo info = {};
    gst_memory_map(memory, &info, GST_MAP_READ);

    auto blob_data = blob->buffer().as<PrecisionTrait<Precision::FP32>::value_type *>();

    const auto &dims = blob->getTensorDesc().getDims();
    size_t channels_number = dims[1];
    size_t image_size = dims[3] * dims[2];
    size_t height = dims[2];
    size_t width = dims[3];

    cv::Mat in_frame_mat(height, width, CV_8UC3, info.data);
    // write_buffer_to_file(info.data, dims[2], dims[3], "image.png");
    for (size_t c = 0; c < channels_number; c++) {
        for (size_t  h = 0; h < height; h++) {
            for (size_t w = 0; w < width; w++) {
                size_t index = c * width * height + h * width + w;
                // blob_data[index] = info.data[index];
                blob_data[index] = in_frame_mat.at<cv::Vec3b>(h, w)[c];
            }
        }
    }
    // for (size_t pid = 0; pid < image_size; pid++) {
    //     for (size_t ch = 0; ch < channels_number; ch++) {
    //         size_t index = ch * image_size + pid;
    //         blob_data[index] = info.data[index];
    //     }
    // }
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
    size_t height = dims[2];
    size_t width = dims[3];
    // write_buffer_to_file(info.data, dims[2], dims[3], "image.png");
    std::vector<cv::Mat> imgPlanes = {
        cv::Mat(height, width, CV_32FC1, blob_data),
        cv::Mat(height, width, CV_32FC1, &(blob_data[image_size])),
        cv::Mat(height, width, CV_32FC1, &(blob_data[image_size * 2]))
    };
    for (auto & img : imgPlanes)
        img.convertTo(img, CV_8UC1, 255);
    cv::Mat resultImg;
    cv::merge(imgPlanes, resultImg);
    size_t buffer_size = resultImg.total() * resultImg.elemSize();
    memcpy(static_cast<void*>(info.data), static_cast<void*>(resultImg.data), buffer_size);

    // for (size_t c = 0; c < channels_number; c++) {
    //     for (size_t  h = 0; h < height; h++) {
    //         for (size_t w = 0; w < width; w++) {
    //             size_t index = c * width * height + h * width + w;
    //             info.data[index] = resultImg.data[index];
    //         }
    //     }
    // }
    // for (size_t pid = 0; pid < image_size; pid++) {
    //     for (size_t ch = 0; ch < channels_number; ch++) {
    //         size_t index = ch * image_size + pid;
    //         info.data[index] = blob_data[index];
    //     }
    // }
    // write_buffer_to_file(resultImg.data, dims[2], dims[3], "opencv-output.png");
    // write_buffer_to_file(info.data, dims[2], dims[3], "output.png");
    gst_memory_unmap(memory, &info);
}

} // namespace

OpenVinoInference::OpenVinoInference(std::string path_to_model_xml) {
    this->network = create_network(path_to_model_xml);
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

        auto it = memories.find(get_blob_size(input_blob));
        if (it != memories.end()) {
            GstMemory *memory = it->second;
            copy_image_into_blob(memory, input_blob);
        } else {
            throw logic_error("network input and input image sizes are different");
        }
    }
}

void OpenVinoInference::run(GstMemory *original_image, GstMemory *resized_image, GstMemory *result_image) {
    copy_images_into_blobs(resized_image, original_image);

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

void run_inference(GstUpScaler *upscaler, GstMemory *original_image, GstMemory *resized_image,
                   GstMemory *result_image) {
    if (!upscaler || !upscaler->inference || !upscaler->inference->openvino_inference) {
        // TODO:
        return;
    }
    upscaler->inference->openvino_inference->run(original_image, resized_image, result_image);
}
