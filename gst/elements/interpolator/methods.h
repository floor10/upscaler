/*******************************************************************************
 * Copyright (C) 2019 floor10
 *
 * SPDX-License-Identifier: MIT
 ******************************************************************************/

#ifndef __METHODS_H__
#define __METHODS_H__

#include <gst/gst.h>
#include <gst/video/video.h>

#include "gstinterpolator.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

GstFlowReturn opencv_resize(GstInterpolator *interpolator, GstBuffer *in_buffer, GstBuffer *out_buffer);

#ifdef __cplusplus
} /* extern C */
#endif /* __cplusplus */

#endif /* __METHODS_H__ */
