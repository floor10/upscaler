/*******************************************************************************
 * Copyright (C) 2019 floor10
 *
 * SPDX-License-Identifier: MIT
 ******************************************************************************/

#ifndef __OPENVINO_INFERENCE__
#define __OPENVINO_INFERENCE__

#include <gst/gst.h>

typedef struct _GstUpScaler GstUpScaler;

#ifdef __cplusplus

#include <inference_engine.hpp>
#include <opencv2/opencv.hpp>

const int BATCH_SIZE = 1;

class OpenVinoInference {
  private:
    size_t _input_frame_width;
    size_t _input_frame_height;

    InferenceEngine::InferRequest _infer_request;
    InferenceEngine::InputsDataMap _inputs_info;
    InferenceEngine::OutputsDataMap _outputs_info;

    InferenceEngine::CNNNetwork _network;
    InferenceEngine::InferencePlugin _plugin;
    InferenceEngine::ExecutableNetwork _executable_network;

    cv::Mat resize_by_opencv(const GstMapInfo &image, size_t width, size_t height);

  public:
    OpenVinoInference(std::string path_to_model_xml);
    ~OpenVinoInference() = default;

    void set_input_frame_size(size_t width, size_t height);
    void run(GstMemory *original_image, GstMemory *result_image);
};

#else /* __cplusplus */

typedef struct OpenVinoInference OpenVinoInference;

#endif /* __cplusplus */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct _InferenceFactory {
    OpenVinoInference *openvino_inference;
} InferenceFactory;

InferenceFactory *create_openvino_inference(gchar *path_to_model_xml, GError **error);
void set_input_video_size(GstUpScaler *upscaler, GstVideoInfo *video_info);
void run_inference(GstUpScaler *upscaler, GstMemory *original_image, GstMemory *result_image);

#ifdef __cplusplus
} /* extern C */
#endif /* __cplusplus */

#endif /* __OPENVINO_INFERENCE__ */
