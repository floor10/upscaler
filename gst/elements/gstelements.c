#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gst/gst.h>
#include "gstinterpolator.h"
#include "gstupscaler.h"

static gboolean
plugins_init (GstPlugin *plugin)
{
    if (!gst_element_register(plugin, "interpolator", GST_RANK_NONE, GST_TYPE_INTERPOLATOR))
      return FALSE

    if (!gst_element_register(plugin, "upscaler", GST_RANK_NONE, GST_TYPE_UPSCALER))
      return FALSE

    return TRUE
}

GST_PLUGIN_DEFINE (
    GST_VERSION_MAJOR,
    GST_VERSION_MINOR,
    videoscaling,
    ELEMENT_BRIEF_DESCRIPTION,
    plugins_init,
    VERSION,
    GST_LICENSE,
    PACKAGE_NAME,
    GST_PACKAGE_ORIGIN
)