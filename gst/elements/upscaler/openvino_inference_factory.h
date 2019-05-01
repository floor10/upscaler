#ifndef __OPENVINO_INFERENCE_FACTORY__
#define __OPENVINO_INFERENCE_FACTORY__

#include "gstupscaler.h"

#ifdef  __cplusplus
class OpenVinoInference;
extern "C" {
#else
typedef struct OpenVinoInference;
#endif

OpenVinoInference* create_openvino_inference(GstUpScaler *upscaler);

#ifdef  __cplusplus
}
#endif

#endif // __OPENVINO_INFERENCE_FACTORY__
