/*******************************************************************************
 * Copyright (C) 2019 floor10
 *
 * SPDX-License-Identifier: MIT
 ******************************************************************************/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gst/gst.h>

#include "gstupscaler.h"

// FIXME: think about description
#define ELEMENT_LONG_NAME "Upscaler long name without it plugin won't load"
#define ELEMENT_BRIEF_DESCRIPTION                                                                                      \
    "The same is truth about its description... Who the hell thought that it is good idea?"

#define GST_VIDEO_SRC_CAPS GST_VIDEO_CAPS_MAKE("{ BGR, BGRx, BGRA }")
#define GST_VIDEO_SINK_CAPS GST_VIDEO_CAPS_MAKE("{ BGR, BGRx, BGRA }")

GST_DEBUG_CATEGORY_STATIC(gst_upscaler_debug);
#define GST_CAT_DEFAULT gst_upscaler_debug

/* Filter signals and args */
enum {
    /* FILL ME */
    LAST_SIGNAL
};

enum { PROP_0, PROP_SILENT };

G_DEFINE_TYPE_WITH_CODE(GstUpScaler, gst_upscaler, GST_TYPE_BASE_TRANSFORM,
                        GST_DEBUG_CATEGORY_INIT(gst_upscaler_debug, "upscaler", 0,
                                                "Debug category of upscaler element"));

static void gst_upscaler_set_property(GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec);
static void gst_upscaler_get_property(GObject *object, guint prop_id, GValue *value, GParamSpec *pspec);

/* caps handling */
static gboolean gst_upscaler_set_caps(GstBaseTransform *transform, GstCaps *in_caps, GstCaps *out_caps);

/* frame output */
static GstFlowReturn gst_upscaler_transform(GstBaseTransform *transform, GstBuffer *in_buffer, GstBuffer *out_buffer);

/* initialize the upscaler's class */
static void gst_upscaler_class_init(GstUpScalerClass *klass) {
    GObjectClass *gobject_class = (GObjectClass *)klass;
    GstElementClass *gstelement_class = (GstElementClass *)klass;
    GstBaseTransformClass *base_transform_class = (GstBaseTransformClass *)klass;

    gobject_class->set_property = gst_upscaler_set_property;
    gobject_class->get_property = gst_upscaler_get_property;

    g_object_class_install_property(
        gobject_class, PROP_SILENT,
        g_param_spec_boolean("silent", "Silent", "Produce verbose output ?", FALSE, G_PARAM_READWRITE));

    gst_element_class_set_static_metadata(gstelement_class, ELEMENT_LONG_NAME, "Video", ELEMENT_BRIEF_DESCRIPTION,
                                          "https://github.com/floor10");

    gst_element_class_add_pad_template(
        gstelement_class,
        gst_pad_template_new("src", GST_PAD_SRC, GST_PAD_ALWAYS, gst_caps_from_string(GST_VIDEO_SRC_CAPS)));
    gst_element_class_add_pad_template(
        gstelement_class,
        gst_pad_template_new("sink", GST_PAD_SINK, GST_PAD_ALWAYS, gst_caps_from_string(GST_VIDEO_SINK_CAPS)));

    base_transform_class->set_caps = GST_DEBUG_FUNCPTR(gst_upscaler_set_caps);
    base_transform_class->transform = GST_DEBUG_FUNCPTR(gst_upscaler_transform);
}

/* initialize the new element
 * initialize instance structure
 */
static void gst_upscaler_init(GstUpScaler *upscaler) { upscaler->silent = FALSE; }

static void gst_upscaler_set_property(GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec) {
    GstUpScaler *filter = GST_UPSCALER(object);

    switch (prop_id) {
    case PROP_SILENT:
        filter->silent = g_value_get_boolean(value);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

static void gst_upscaler_get_property(GObject *object, guint prop_id, GValue *value, GParamSpec *pspec) {
    GstUpScaler *filter = GST_UPSCALER(object);

    switch (prop_id) {
    case PROP_SILENT:
        g_value_set_boolean(value, filter->silent);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

/* GstUpscaler vmethod implementations */

static gboolean gst_upscaler_set_caps(GstBaseTransform *transform, GstCaps *in_caps, GstCaps *out_caps) {
    GstUpScaler *upscaler = GST_UPSCALER(transform);
    GST_DEBUG_OBJECT(upscaler, "Caps setting in the process");

    // TODO: implement method

    return TRUE;
}

static GstFlowReturn gst_upscaler_transform(GstBaseTransform *transform, GstBuffer *in_buffer, GstBuffer *out_buffer) {
    GstUpScaler *upscaler = GST_UPSCALER(transform);
    GST_DEBUG_OBJECT(upscaler, "Buffers transforming in the process");

    // TODO: implement method

    return GST_FLOW_OK;
}
