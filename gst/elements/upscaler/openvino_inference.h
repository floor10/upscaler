#include "upscaler.h"
#include <gst/gst.h>
#include <inference_engine.hpp>

const std::string path_to_model_with_name = "";
const int BATCH_SIZE = 1;

class OpenVinoInference {
  private:
    GstUpScaler *_upscaler;
    InferenceEngine::InferRequest _infer_request;
    InferenceEngine::InputsDataMap _inputs_info;
    InferenceEngine::OutputsDataMap _outputs_info;

    void copy_images_into_blobs(GstMemory *resized_image, GstMemory *original_image);

  public:
    OpenVinoInference(GstUpScaler *upscaler);
    ~OpenVinoInference() = default;

    void inference(GstMemory *original_image, GstMemory *resized_image, GstMemory *result_image);
};
