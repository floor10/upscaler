FROM ubuntu:18.04

RUN apt-get update \
    && DEBIAN_FRONTEND=noninteractive \
        apt-get install -y --no-install-recommends \
            build-essential \
            cmake \
            pkg-config \
            libglib2.0-dev \
            gobject-introspection \
            libgirepository1.0-dev \
            libgstreamer1.0-dev \
            libgstreamer-plugins-base1.0-dev \
            libgstreamer-plugins-good1.0-dev \
            libgstreamer-plugins-bad1.0-dev \
            gstreamer1.0-plugins-ugly-dbg \
            gstreamer1.0-libav-dbg \
            python3-gst-1.0-dbg \
    && rm -rf /var/lib/apt/lists/*

RUN apt-get update \
    && DEBIAN_FRONTEND=noninteractive \
        apt-get install -y --no-install-recommends \
            gstreamer1.0-tools \
            libopencv-dev \
    && rm -rf /var/lib/apt/lists/*

COPY . /usr/share/sources/upscaler

WORKDIR /usr/share/sources/upscaler

RUN mkdir build && cd build && cmake .. && make -j $(nproc)

ENV GST_PLUGIN_PATH=/usr/share/sources/upscaler/build/gst/elements/