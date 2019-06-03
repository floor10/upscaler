
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
            gstreamer1.0-vaapi \
            gstreamer1.0-tools \
            python3-gst-1.0-dbg \
            curl \
            wget \
            libssl-dev \
            ca-certificates \
            git \
            libboost-regex-dev \
            gcc-multilib \
            g++-multilib \
            libgtk2.0-dev \
            unzip \
            automake \
            libtool \
            autoconf \
            libcairo2-dev \
            libpango1.0-dev \
            libglib2.0-dev \
            libgtk2.0-dev \
            libswscale-dev \
            libavcodec-dev \
            libavformat-dev \
            libusb-1.0-0-dev \
            libopenblas-dev \
    && rm -rf /var/lib/apt/lists/*

RUN apt-get update \
    && DEBIAN_FRONTEND=noninteractive \
        apt-get install -y --no-install-recommends \
    && rm -rf /var/lib/apt/lists/*

RUN git clone https://github.com/opencv/dldt &&\
    cd dldt && \
    git checkout 2019_R1.1 && \
    git submodule update --init --recursive && \
    mkdir inference-engine/build && cd inference-engine/build && \
    cmake .. && make -j $(nproc)


COPY . /usr/share/sources/upscaler

WORKDIR /usr/share/sources/upscaler

RUN mkdir build && cd build && cmake .. && make -j $(nproc)

ENV GST_PLUGIN_PATH=/usr/share/sources/upscaler/build/gst/elements/
