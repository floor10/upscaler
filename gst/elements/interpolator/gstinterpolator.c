/*******************************************************************************
 * Copyright (C) 2019 floor10
 *
 * SPDX-License-Identifier: MIT
 ******************************************************************************/

#include "gstinterpolator.h"

#define ELEMENT_NAME "interpolator"
// FIXME: think about description
#define ELEMENT_LONG_NAME "Interpolator long name without it plugin won't load"
#define ELEMENT_BRIEF_DESCRIPTION \
    "The same is truth about its description... Who the hell thought that it is good idea?"

#define GST_VIDEO_SRC_CAPS GST_VIDEO_CAPS_MAKE("{ BGR, BGRx, BGRA }")
#define GST_VIDEO_SINK_CAPS GST_VIDEO_CAPS_MAKE("{ BGR, BGRx, BGRA }")

GST_DEBUG_CATEGORY_STATIC (gst_interpolator_debug);
#define GST_CAT_DEFAULT gst_interpolator_debug

/* Filter signals and args */
enum
{
  /* FILL ME */
  LAST_SIGNAL
};

enum
{
  PROP_0,
  PROP_SILENT
};

#define gst_interpolator_parent_class parent_class
G_DEFINE_TYPE (GstInterpolator, gst_interpolator, GST_TYPE_ELEMENT);

static void gst_interpolator_set_property (GObject * object, guint prop_id,
    const GValue * value, GParamSpec * pspec);
static void gst_interpolator_get_property (GObject * object, guint prop_id,
    GValue * value, GParamSpec * pspec);

static GstFlowReturn gst_interpolator_transform_frame(GstVideoFilter *filter,
                                                     GstVideoFrame *in,
                                                     GstVideoFrame *out);

static void gst_interpolator_class_init (GstInterpolatorClass *klass)
{
  GObjectClass *gobject_class = (GObjectClass *) klass;;
  GstElementClass *gstelement_class = (GstElementClass *) klass;
  GstVideoFilterClass *filter_class = (GstVideoFilterClass *) klass;

  gobject_class->set_property = gst_interpolator_set_property;
  gobject_class->get_property = gst_interpolator_get_property;

  g_object_class_install_property (gobject_class, PROP_SILENT,
      g_param_spec_boolean ("silent", "Silent", "Produce verbose output ?",
          FALSE, G_PARAM_READWRITE));

  gst_element_class_set_static_metadata(gstelement_class,
    ELEMENT_LONG_NAME,
    "Video",
    ELEMENT_BRIEF_DESCRIPTION,
    "https://github.com/floor10");

  gst_element_class_add_pad_template(gstelement_class, gst_pad_template_new("src", GST_PAD_SRC, GST_PAD_ALWAYS,
      gst_caps_from_string(GST_VIDEO_SRC_CAPS)));
  gst_element_class_add_pad_template(gstelement_class, gst_pad_template_new("sink", GST_PAD_SINK, GST_PAD_ALWAYS,
      gst_caps_from_string(GST_VIDEO_SINK_CAPS)));

  filter_class->transform_frame = GST_DEBUG_FUNCPTR(gst_interpolator_transform_frame);
}

static void gst_interpolator_init(GstInterpolator * interpolator)
{
  interpolator->silent = FALSE;
}

static void
gst_interpolator_set_property (GObject * object, guint prop_id,
    const GValue * value, GParamSpec * pspec)
{
  GstInterpolator *filter = GST_INTERPOLATOR (object);

  switch (prop_id) {
    case PROP_SILENT:
      filter->silent = g_value_get_boolean (value);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static void
gst_interpolator_get_property (GObject * object, guint prop_id,
    GValue * value, GParamSpec * pspec)
{
  GstInterpolator *filter = GST_INTERPOLATOR (object);

  switch (prop_id) {
    case PROP_SILENT:
      g_value_set_boolean (value, filter->silent);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static GstFlowReturn gst_interpolator_transform_frame (GstVideoFilter * filter,
    GstVideoFrame * in_frame, GstVideoFrame * out_frame)
{
  GstFlowReturn ret = GST_FLOW_OK;

  return ret;
}
