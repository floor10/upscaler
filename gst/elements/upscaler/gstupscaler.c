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

#define DEFAULT_DEVICE GST_UPSCALER_CPU

#define GST_VIDEO_SRC_CAPS GST_VIDEO_CAPS_MAKE("{ BGR, BGRx, BGRA }")
#define GST_VIDEO_SINK_CAPS GST_VIDEO_CAPS_MAKE("{ BGR, BGRx, BGRA }")

GST_DEBUG_CATEGORY_STATIC(gst_upscaler_debug);
#define GST_CAT_DEFAULT gst_upscaler_debug

/* Filter signals and args */
enum {
    /* FILL ME */
    LAST_SIGNAL
};

enum { PROP_0, PROP_MODEL, PROP_DEVICE, PROP_SILENT };

G_DEFINE_TYPE_WITH_CODE(GstUpScaler, gst_upscaler, GST_TYPE_BASE_TRANSFORM,
                        GST_DEBUG_CATEGORY_INIT(gst_upscaler_debug, "upscaler", 0,
                                                "Debug category of upscaler element"));

static void gst_upscaler_set_property(GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec);
static void gst_upscaler_get_property(GObject *object, guint prop_id, GValue *value, GParamSpec *pspec);

#define GST_TYPE_UPSCALER_DEVICE (gst_upscaler_device_get_type())
GType gst_upscaler_device_get_type(void);

/* states */
static gboolean gst_upscaler_start(GstBaseTransform *transform);

/* property initialization */
static void gst_upscaler_set_device(GstUpScaler *upscaler, GstUpscalerDevice device_type);

/* caps handling */
static gboolean gst_upscaler_set_caps(GstBaseTransform *transform, GstCaps *in_caps, GstCaps *out_caps);

/* frame output */
static GstFlowReturn gst_upscaler_transform(GstBaseTransform *transform, GstBuffer *input_buffer,
                                            GstBuffer *output_buffer);

/* initialize the upscaler's class */
static void gst_upscaler_class_init(GstUpScalerClass *klass) {
    GObjectClass *gobject_class = (GObjectClass *)klass;
    GstElementClass *gstelement_class = (GstElementClass *)klass;
    GstBaseTransformClass *base_transform_class = (GstBaseTransformClass *)klass;

    gobject_class->set_property = gst_upscaler_set_property;
    gobject_class->get_property = gst_upscaler_get_property;

    g_object_class_install_property(gobject_class, PROP_MODEL,
                                    g_param_spec_string("model", "Model", "Required. Inference model file path", NULL,
                                                        G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

    g_object_class_install_property(gobject_class, PROP_DEVICE,
                                    g_param_spec_enum("device", "Device", "Device for inference",
                                                      GST_TYPE_UPSCALER_DEVICE, DEFAULT_DEVICE,
                                                      G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

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
    base_transform_class->start = GST_DEBUG_FUNCPTR(gst_upscaler_start);
}

/* initialize the new element
 * initialize instance structure
 */
static void gst_upscaler_init(GstUpScaler *upscaler) {
    GST_DEBUG_OBJECT(upscaler, "Upscaler element initialization");

    upscaler->model = NULL;
    gst_upscaler_set_device(upscaler, DEFAULT_DEVICE);
    upscaler->silent = FALSE;
    upscaler->inference = NULL;
}

static void gst_upscaler_set_property(GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec) {
    GstUpScaler *upscaler = GST_UPSCALER(object);

    GST_DEBUG_OBJECT(upscaler, "Setting upscaler element properties");

    switch (prop_id) {
    case PROP_MODEL:
        // FIXME: add protection in case of dynamic model change
        upscaler->model = g_value_dup_string(value);
        break;
    case PROP_DEVICE:
        gst_upscaler_set_device(upscaler, g_value_get_enum(value));
        break;
    case PROP_SILENT:
        upscaler->silent = g_value_get_boolean(value);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

static void gst_upscaler_get_property(GObject *object, guint prop_id, GValue *value, GParamSpec *pspec) {
    GstUpScaler *upscaler = GST_UPSCALER(object);

    switch (prop_id) {
    case PROP_MODEL:
        g_value_set_string(value, upscaler->model);
        break;
    case PROP_DEVICE:
        g_value_set_enum(value, upscaler->device_type);
        break;
    case PROP_SILENT:
        g_value_set_boolean(value, upscaler->silent);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

/* GstUpscaler vmethod implementations */

static gboolean gst_upscaler_start(GstBaseTransform *transform) {
    GstUpScaler *upscaler = GST_UPSCALER(transform);
    GST_DEBUG_OBJECT(upscaler, "UpScaler start");
    GError *error = NULL;
    upscaler->inference = create_openvino_inference(upscaler->model, &error);

    if (error)
    {
        GST_ELEMENT_ERROR(upscaler, RESOURCE, TOO_LAZY, ("Upscaler plugin intitialization failed"),
                          ("%s", error->message));
        return FALSE;
    }

    return TRUE;
}

static gboolean gst_upscaler_set_caps(GstBaseTransform *transform, GstCaps *in_caps, GstCaps *out_caps) {
    GstUpScaler *upscaler = GST_UPSCALER(transform);
    GST_DEBUG_OBJECT(upscaler, "Caps setting in the process");

    // TODO: implement method

    return TRUE;
}

static GstFlowReturn gst_upscaler_transform(GstBaseTransform *transform, GstBuffer *input_buffer,
                                            GstBuffer *output_buffer) {
    GstUpScaler *upscaler = GST_UPSCALER(transform);
    GST_DEBUG_OBJECT(upscaler, "Buffers transforming in the process");

    GstMemory *original_image = gst_buffer_peek_memory(input_buffer, 0);
    if (original_image == NULL) {
        GST_ERROR_OBJECT(upscaler, "Can not get the original image from the buffer");
        return GST_BASE_TRANSFORM_FLOW_DROPPED;
    }
    GstMemory *resized_image = gst_buffer_peek_memory(input_buffer, 1);
    if (resized_image == NULL) {
        GST_ERROR_OBJECT(upscaler, "Can not get the resized image from the buffer");
        return GST_BASE_TRANSFORM_FLOW_DROPPED;
    }
    gsize resized_image_size = gst_memory_get_sizes(resized_image, NULL, NULL);
    GstMemory *result_image = gst_allocator_alloc(NULL, resized_image_size, NULL);
    run_inference(upscaler, original_image, resized_image, result_image);
    gst_buffer_replace_memory(output_buffer, 0, result_image);

    return GST_FLOW_OK;
}

/* GstUpscaler method implementations */

GType gst_upscaler_device_get_type(void) {
    static GType upscaler_device_type = 0;
    static const GEnumValue device_types[] = {{GST_UPSCALER_CPU, "Inference device is CPU", "CPU"},
                                              {GST_UPSCALER_GPU, "Inference device is GPU", "GPU"},
                                              {0, NULL, NULL}};

    if (!upscaler_device_type) {
        upscaler_device_type = g_enum_register_static("GstUpscalerDevice", device_types);
    }
    return upscaler_device_type;
}

static void gst_upscaler_set_device(GstUpScaler *upscaler, GstUpscalerDevice device_type) {
    GST_DEBUG_OBJECT(upscaler, "Setting device to %d", device_type);

    GEnumValue *enum_value;
    GEnumClass *enum_class;

    upscaler->device_type = device_type;

    enum_class = g_type_class_ref(GST_TYPE_UPSCALER_DEVICE);
    enum_value = g_enum_get_value(enum_class, device_type);
    if (enum_value) {
        g_free(upscaler->device);
        upscaler->device = g_strdup(enum_value->value_nick);
    } else
        g_assert_not_reached();

    g_type_class_unref(enum_class);
}
