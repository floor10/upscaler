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

const std::string path_to_model_with_name =
    "/home/uabdumum/umed/single-image-super-resolution-1032/FP32/single-image-super-resolution-1032";
const int BATCH_SIZE = 1;

class OpenVinoInference {
  private:
    InferenceEngine::InferRequest _infer_request;
    InferenceEngine::InputsDataMap _inputs_info;
    InferenceEngine::OutputsDataMap _outputs_info;

    InferenceEngine::CNNNetwork network;
    InferenceEngine::InferencePlugin plugin;
    InferenceEngine::ExecutableNetwork executable_network;

    void copy_images_into_blobs(GstMemory *resized_image, GstMemory *original_image);

  public:
    OpenVinoInference();
    ~OpenVinoInference() = default;

    void run(GstMemory *original_image, GstMemory *resized_image, GstMemory *result_image);
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

InferenceFactory *create_openvino_inference();
void run_inference(GstUpScaler *upscaler, GstMemory *original_image, GstMemory *resized_image, GstMemory *result_image);

#ifdef __cplusplus
} /* extern C */
#endif /* __cplusplus */

#endif /* __OPENVINO_INFERENCE__ */
