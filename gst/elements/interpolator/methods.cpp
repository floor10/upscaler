/*******************************************************************************
 * Copyright (C) 2019 floor10
 *
 * SPDX-License-Identifier: MIT
 ******************************************************************************/

#include "methods.h"

#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>

const int ORIGINAL_IMAGE_MODEL_INPUT_WIDTH = 640;
const int ORIGINAL_IMAGE_MODEL_INPUT_HEIGHT = 360;

GstMemory *resize(const GstMapInfo &map_info, GstVideoInfo *video_info, int width, int height) {
    cv::Mat in_frame_mat(GST_VIDEO_INFO_HEIGHT(video_info), GST_VIDEO_INFO_WIDTH(video_info), CV_8UC3, map_info.data);

    cv::Mat resized;
    cv::resize(in_frame_mat, resized, cv::Size(width, height), 0, 0, cv::INTER_CUBIC);

    // cv::Mat color_converted_to_rgb;
    // cv::cvtColor(resized, color_converted_to_rgb, cv::ColorConversionCodes::COLOR_BGRA2BGR);

    size_t buffer_size = resized.total() * resized.elemSize();
    unsigned char *data = new unsigned char[buffer_size];
    memcpy(static_cast<void *>(data), static_cast<void *>(resized.data), buffer_size);

    return gst_memory_new_wrapped(GST_MEMORY_FLAG_ZERO_PREFIXED, (gpointer)data, buffer_size, 0, buffer_size, NULL,
                                  NULL);
}

GstFlowReturn opencv_resize(GstInterpolator *interpolator, GstBuffer *in_buffer, GstBuffer *out_buffer) {
    GstMapInfo in_buffer_map_info;
    GstMemory *out_frame_mem;

    if (!gst_buffer_map(in_buffer, &in_buffer_map_info, GST_MAP_READ))
        GST_ERROR("Buffer map failed");


    GstMemory *resized_to_original_image_model_input_size =
        resize(in_buffer_map_info, interpolator->input_video_info, ORIGINAL_IMAGE_MODEL_INPUT_WIDTH,
               ORIGINAL_IMAGE_MODEL_INPUT_HEIGHT);

    gst_buffer_replace_memory(out_buffer, 0, resized_to_original_image_model_input_size);


    GstMemory *resized_to_destination_size =
        resize(in_buffer_map_info, interpolator->input_video_info, interpolator->width, interpolator->height);
    gst_buffer_append_memory(out_buffer, resized_to_destination_size);

    gst_buffer_unmap(in_buffer, &in_buffer_map_info);
    // TODO: set metadata to buffer using GstVideoMeta

    return GST_FLOW_OK;
}
