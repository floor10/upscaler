#!/bin/bash

gst-launch-1.0 filesrc location=/home/test/repositories/video-analytics/video-examples/Pexels_Videos_4786_960x540.mp4 ! decodebin ! video/x-raw ! videoconvert ! videoscale ! video/x-raw,width=1920,height=1080 ! videoconvert ! fpsdisplaysink sync=false