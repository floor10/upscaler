FROM ubuntu:18.04

RUN apt update \
    && apt install -y --no-install-recommends \
        build-essential \
        cmake \
        pkg-config \
        libglib2.0-dev \
        gobject-introspection \
        libgirepository1.0-dev \
        libgstreamer1.0-0-dbg \
        gstreamer1.0-plugins-base-dbg \
        gstreamer1.0-plugins-good-dbg \
        gstreamer1.0-plugins-bad-dbg \
        gstreamer1.0-plugins-ugly-dbg \
        gstreamer1.0-libav-dbg \
        python3-gst-1.0-dbg
        # gstreamer1.0-doc \
        # gstreamer1.0-tools \
        # gstreamer1.0-x \
        # gstreamer1.0-alsa \
        # gstreamer1.0-gl \
        # gstreamer1.0-gtk3 \
        # gstreamer1.0-qt5 \
        # gstreamer1.0-pulseaudio

COPY . /usr/share/sources/upscaler

WORKDIR /usr/share/sources/upscaler

RUN mkdir build && cd build && cmake .. && make -j $(nproc)