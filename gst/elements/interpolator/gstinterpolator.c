/*******************************************************************************
 * Copyright (C) 2019 floor10
 *
 * SPDX-License-Identifier: MIT
 ******************************************************************************/

#include "gstinterpolator.h"
#include "methods.h"

// FIXME: think about description
#define ELEMENT_LONG_NAME "Interpolator long name without it plugin won't load"
#define ELEMENT_BRIEF_DESCRIPTION                                                                                      \
    "The same is truth about its description... Who the hell thought that it is good idea?"

#define GST_VIDEO_SRC_CAPS GST_VIDEO_CAPS_MAKE("{ BGR, BGRx, BGRA }")
#define GST_VIDEO_SINK_CAPS GST_VIDEO_CAPS_MAKE("{ BGR, BGRx, BGRA }")

GST_DEBUG_CATEGORY_STATIC(gst_interpolator_debug);
#define GST_CAT_DEFAULT gst_interpolator_debug

/* Filter signals and args */
enum {
    /* FILL ME */
    LAST_SIGNAL
};

enum { PROP_0, PROP_SILENT };

G_DEFINE_TYPE_WITH_CODE(GstInterpolator, gst_interpolator, GST_TYPE_BASE_TRANSFORM,
                        GST_DEBUG_CATEGORY_INIT(gst_interpolator_debug, "interpolator", 0,
                                                "Debug category of interpolator element"));

static void gst_interpolator_set_property(GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec);
static void gst_interpolator_get_property(GObject *object, guint prop_id, GValue *value, GParamSpec *pspec);

/* caps handling */
static gboolean gst_interpolator_set_caps(GstBaseTransform *transform, GstCaps *in_caps, GstCaps *out_caps);

/* frame output */
static GstFlowReturn gst_interpolator_transform(GstBaseTransform *transform, GstBuffer *in_buffer,
                                                GstBuffer *out_buffer);

/* interpolator class initialization */
static void gst_interpolator_class_init(GstInterpolatorClass *klass) {
    GObjectClass *gobject_class = (GObjectClass *)klass;
    GstElementClass *gstelement_class = (GstElementClass *)klass;
    GstBaseTransformClass *base_transform_class = (GstBaseTransformClass *)klass;

    gobject_class->set_property = gst_interpolator_set_property;
    gobject_class->get_property = gst_interpolator_get_property;

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

    base_transform_class->set_caps = GST_DEBUG_FUNCPTR(gst_interpolator_set_caps);
    base_transform_class->transform = GST_DEBUG_FUNCPTR(gst_interpolator_transform);
}

static void gst_interpolator_init(GstInterpolator *interpolator) {
    interpolator->input_video_info = gst_video_info_new();
    interpolator->output_video_info = gst_video_info_new();
    interpolator->silent = FALSE;
}

static void gst_interpolator_set_property(GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec) {
    GstInterpolator *filter = GST_INTERPOLATOR(object);

    switch (prop_id) {
    case PROP_SILENT:
        filter->silent = g_value_get_boolean(value);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

static void gst_interpolator_get_property(GObject *object, guint prop_id, GValue *value, GParamSpec *pspec) {
    GstInterpolator *filter = GST_INTERPOLATOR(object);

    switch (prop_id) {
    case PROP_SILENT:
        g_value_set_boolean(value, filter->silent);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

static gboolean gst_interpolator_set_caps(GstBaseTransform *transform, GstCaps *in_caps, GstCaps *out_caps) {
    GstInterpolator *interpolator = GST_INTERPOLATOR(transform);
    GST_DEBUG_OBJECT(interpolator, "Caps setting in the process");

    if (!gst_video_info_from_caps(interpolator->input_video_info, in_caps) ||
        !gst_video_info_from_caps(interpolator->output_video_info, out_caps)) {
        GST_ERROR_OBJECT(interpolator, "Caps are invalid");
        return FALSE;
    }

    return TRUE;
}

static GstFlowReturn gst_interpolator_transform(GstBaseTransform *transform, GstBuffer *in_buffer,
                                                GstBuffer *out_buffer) {
    GstInterpolator *interpolator = GST_INTERPOLATOR(transform);
    GST_DEBUG_OBJECT(interpolator, "Buffers transforming in the process");

    return opencv_resize(interpolator, in_buffer, out_buffer);
}
