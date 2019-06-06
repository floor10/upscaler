FROM floor10/dldt:latest

RUN apt-get update \
    && DEBIAN_FRONTEND=noninteractive \
        apt-get install -y --no-install-recommends \
            pkg-config \
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
    && rm -rf /var/lib/apt/lists/*

WORKDIR /root/upscaler

COPY ./ ./

RUN mkdir build && cd build && cmake .. && make -j $(nproc)

ENV GST_PLUGIN_PATH=/root/upscaler/build/gst/elements
