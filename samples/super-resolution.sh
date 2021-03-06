#!/bin/bash

VIDEO_FILE=${2:-/root/video-examples/Pexels_Videos_4786_960x540.mp4}

export LC_NUMERIC="C"
echo C locale is set

gst-launch-1.0 filesrc location=${VIDEO_FILE} ! decodebin ! video/x-raw ! videoconvert ! upscaler model=${1} ! video/x-raw,width=1920,height=1080 ! videoconvert ! fpsdisplaysink sync=false
