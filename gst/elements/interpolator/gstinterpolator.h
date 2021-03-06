/*******************************************************************************
 * Copyright (C) 2019 floor10
 *
 * SPDX-License-Identifier: MIT
 ******************************************************************************/

#ifndef __GST_INTERPOLATOR_H__
#define __GST_INTERPOLATOR_H__

#include <gst/base/gstbasetransform.h>
#include <gst/gst.h>
#include <gst/video/video.h>

G_BEGIN_DECLS

/* #defines don't like whitespacey bits */
#define GST_TYPE_INTERPOLATOR (gst_interpolator_get_type())
#define GST_INTERPOLATOR(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), GST_TYPE_INTERPOLATOR, GstInterpolator))
#define GST_INTERPOLATOR_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), GST_TYPE_INTERPOLATOR, GstInterpolatorClass))
#define GST_IS_INTERPOLATOR(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), GST_TYPE_INTERPOLATOR))
#define GST_IS_INTERPOLATOR_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), GST_TYPE_INTERPOLATOR))

typedef struct _GstInterpolator GstInterpolator;
typedef struct _GstInterpolatorClass GstInterpolatorClass;

struct _GstInterpolator {
    GstBaseTransform base_interpolator;

    GstPad *sinkpad, *srcpad;
    GstVideoInfo *input_video_info;
    guint width;
    guint height;

    gboolean silent;
};

struct _GstInterpolatorClass {
    GstBaseTransformClass base_interpolator_class;
};

/* Standard function returning type information. */
GType gst_interpolator_get_type(void);

G_END_DECLS

#endif /* __GST_INTERPOLATOR_H__ */
