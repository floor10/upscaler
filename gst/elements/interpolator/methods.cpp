/*******************************************************************************
 * Copyright (C) 2019 floor10
 *
 * SPDX-License-Identifier: MIT
 ******************************************************************************/

#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>

#include "methods.h"

GstFlowReturn opencv_resize(GstInterpolator *interpolator, GstBuffer *in_buffer, GstBuffer *out_buffer)
{
    cv::Mat out_frame_mat; 
    GstMapInfo in_buffer_map_info;
    GstMemory *out_frame_mem;

    if (!gst_buffer_map(in_buffer, &in_buffer_map_info, GST_MAP_READ))
        GST_ERROR("Buffer map failed");
    cv::Mat in_frame_mat(GST_VIDEO_INFO_HEIGHT(interpolator->input_video_info), GST_VIDEO_INFO_WIDTH(interpolator->input_video_info), 
        CV_8UC4, in_buffer_map_info.data);
    cv::Size out_frame_size(GST_VIDEO_INFO_WIDTH(interpolator->output_video_info), GST_VIDEO_INFO_HEIGHT(interpolator->output_video_info));
    cv::resize(in_frame_mat, out_frame_mat, out_frame_size, 0, 0, cv::INTER_CUBIC);

    out_buffer = gst_buffer_new_wrapped((gpointer)in_frame_mat.data, in_frame_mat.total() * in_frame_mat.elemSize());
    out_frame_mem = gst_memory_new_wrapped(GST_MEMORY_FLAG_ZERO_PREFIXED, (gpointer)out_frame_mat.data,
        out_frame_mat.total() * out_frame_mat.elemSize(), 0, out_frame_mat.total() * out_frame_mat.elemSize(),
        NULL, NULL);
    gst_buffer_append_memory(out_buffer, out_frame_mem);
    
    // TODO: set metadata to buffer using GstVideoMeta

    gst_buffer_unmap(in_buffer, &in_buffer_map_info);

    return GST_FLOW_OK;
}
