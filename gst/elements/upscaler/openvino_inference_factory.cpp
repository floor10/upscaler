#include "openvino_inference.h"

OpenVinoInference *create_openvino_inference(GstUpScaler *upscaler) {
    return new OpenVinoInference(upscaler);
}
