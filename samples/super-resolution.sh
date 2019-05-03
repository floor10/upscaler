#!/bin/bash

VIDEO_FILE=${1:-/root/video-examples/Pexels_Videos_4786_960x540.mp4}

gdb --args \
gst-launch-1.0 filesrc location=${VIDEO_FILE} ! decodebin ! video/x-raw ! videoconvert ! interpolator ! upscaler ! videoconvert ! fpsdisplaysink sync=false
