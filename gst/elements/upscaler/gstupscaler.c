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
#define ELEMENT_BRIEF_DESCRIPTION \
    "The same is truth about its description... Who the hell thought that it is good idea?"

GST_DEBUG_CATEGORY_STATIC(gst_up_scaler_debug);
#define GST_CAT_DEFAULT gst_up_scaler_debug

/* Filter signals and args */
enum {
    /* FILL ME */
    LAST_SIGNAL
};

enum { PROP_0, PROP_SILENT };

/* the capabilities of the inputs and outputs.
 *
 * describe the real formats here.
 */
static GstStaticPadTemplate sink_factory =
    GST_STATIC_PAD_TEMPLATE("sink", GST_PAD_SINK, GST_PAD_ALWAYS, GST_STATIC_CAPS("ANY"));

static GstStaticPadTemplate src_factory =
    GST_STATIC_PAD_TEMPLATE("src", GST_PAD_SRC, GST_PAD_ALWAYS, GST_STATIC_CAPS("ANY"));

#define gst_up_scaler_parent_class parent_class
G_DEFINE_TYPE(GstUpScaler, gst_up_scaler, GST_TYPE_ELEMENT);

static void gst_up_scaler_set_property(GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec);
static void gst_up_scaler_get_property(GObject *object, guint prop_id, GValue *value, GParamSpec *pspec);

static gboolean gst_up_scaler_sink_event(GstPad *pad, GstObject *parent, GstEvent *event);
static GstFlowReturn gst_up_scaler_chain(GstPad *pad, GstObject *parent, GstBuffer *buf);

/* GObject vmethod implementations */

/* initialize the upscaler's class */
static void gst_up_scaler_class_init(GstUpScalerClass *klass) {
    GObjectClass *gobject_class;
    GstElementClass *gstelement_class;

    gobject_class = (GObjectClass *)klass;
    gstelement_class = (GstElementClass *)klass;

    gobject_class->set_property = gst_up_scaler_set_property;
    gobject_class->get_property = gst_up_scaler_get_property;

    g_object_class_install_property(
        gobject_class, PROP_SILENT,
        g_param_spec_boolean("silent", "Silent", "Produce verbose output ?", FALSE, G_PARAM_READWRITE));

    // FIXME: update plugin definition
    // clang-format off
    gst_element_class_set_details_simple(gstelement_class,
        ELEMENT_LONG_NAME,
        "Video",
        ELEMENT_BRIEF_DESCRIPTION,
        "https://github.com/floor10");
    // clang-format on

    gst_element_class_add_pad_template(gstelement_class, gst_static_pad_template_get(&src_factory));
    gst_element_class_add_pad_template(gstelement_class, gst_static_pad_template_get(&sink_factory));
}

/* initialize the new element
 * instantiate pads and add them to element
 * set pad calback functions
 * initialize instance structure
 */
static void gst_up_scaler_init(GstUpScaler *filter) {
    filter->sinkpad = gst_pad_new_from_static_template(&sink_factory, "sink");
    gst_pad_set_event_function(filter->sinkpad, GST_DEBUG_FUNCPTR(gst_up_scaler_sink_event));
    gst_pad_set_chain_function(filter->sinkpad, GST_DEBUG_FUNCPTR(gst_up_scaler_chain));
    GST_PAD_SET_PROXY_CAPS(filter->sinkpad);
    gst_element_add_pad(GST_ELEMENT(filter), filter->sinkpad);

    filter->srcpad = gst_pad_new_from_static_template(&src_factory, "src");
    GST_PAD_SET_PROXY_CAPS(filter->srcpad);
    gst_element_add_pad(GST_ELEMENT(filter), filter->srcpad);

    filter->silent = FALSE;
}

static void gst_up_scaler_set_property(GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec) {
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

static void gst_up_scaler_get_property(GObject *object, guint prop_id, GValue *value, GParamSpec *pspec) {
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

/* GstElement vmethod implementations */

/* this function handles sink events */
static gboolean gst_up_scaler_sink_event(GstPad *pad, GstObject *parent, GstEvent *event) {
    GstUpScaler *filter;
    gboolean ret;

    filter = GST_UPSCALER(parent);

    GST_LOG_OBJECT(filter, "Received %s event: %" GST_PTR_FORMAT, GST_EVENT_TYPE_NAME(event), event);

    switch (GST_EVENT_TYPE(event)) {
    case GST_EVENT_CAPS: {
        GstCaps *caps;

        gst_event_parse_caps(event, &caps);
        /* do something with the caps */

        /* and forward */
        ret = gst_pad_event_default(pad, parent, event);
        break;
    }
    default:
        ret = gst_pad_event_default(pad, parent, event);
        break;
    }
    return ret;
}

/* chain function
 * this function does the actual processing
 */
static GstFlowReturn gst_up_scaler_chain(GstPad *pad, GstObject *parent, GstBuffer *buf) {
    GstUpScaler *filter;

    filter = GST_UPSCALER(parent);

    if (filter->silent == FALSE)
        g_print("I'm plugged, therefore I'm in.\n");

    /* just push out the incoming buffer without touching it */
    return gst_pad_push(filter->srcpad, buf);
}
