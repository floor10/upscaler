/*******************************************************************************
 * Copyright (C) 2019 floor10
 *
 * SPDX-License-Identifier: MIT
 ******************************************************************************/

#ifndef __GST_UPSCALER_H__
#define __GST_UPSCALER_H__

#include <gst/gst.h>

G_BEGIN_DECLS

/* #defines don't like whitespacey bits */
#define GST_TYPE_UPSCALER (gst_up_scaler_get_type())
#define GST_UPSCALER(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), GST_TYPE_UPSCALER, GstUpScaler))
#define GST_UPSCALER_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), GST_TYPE_UPSCALER, GstUpScalerClass))
#define GST_IS_UPSCALER(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), GST_TYPE_UPSCALER))
#define GST_IS_UPSCALER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), GST_TYPE_UPSCALER))

typedef struct _GstUpScaler GstUpScaler;
typedef struct _GstUpScalerClass GstUpScalerClass;

struct _GstUpScaler {
    GstElement element;

    GstPad *sinkpad, *srcpad;

    gboolean silent;
};

struct _GstUpScalerClass {
    GstElementClass parent_class;
};

GType gst_up_scaler_get_type(void);

G_END_DECLS

#endif /* __GST_UPSCALER_H__ */
