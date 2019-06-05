FROM ubuntu:18.04 as openvino

RUN apt-get update \
    && DEBIAN_FRONTEND=noninteractive \
        apt-get install -y --no-install-recommends \
            build-essential \
            cmake \
            curl \
            wget \
            libssl-dev \
            ca-certificates \
            git \
            libboost-regex-dev \
            gcc-multilib \
            g++-multilib \
            libgtk2.0-dev \
            pkg-config \
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
            libgstreamer1.0-0 \
            gstreamer1.0-plugins-base \
            libusb-1.0-0-dev \
            libopenblas-dev \
            libpng-dev \
        && rm -rf /var/lib/apt/lists/*

WORKDIR /tmp

RUN git clone https://github.com/opencv/dldt && cd dldt && \
    git checkout 2019_R1.1 && git submodule update --init --recursive && \
    mkdir inference-engine/build && cd inference-engine/build && \
    cmake \
        -DENABLE_MKL_DNN=ON \
        -DENABLE_CLDNN=ON \
        .. && \
    make -j $(nproc) && \
    cd /tmp/dldt/inference-engine && \
    mkdir -p /opt/intel/dldt/inference-engine/lib && \
    mkdir -p /opt/intel/dldt/inference-engine/thirdparty/intel_ocl_icd && \
    mkdir -p /opt/intel/dldt/inference-engine/external && \
    \
    mv ./thirdparty/clDNN/common/intel_ocl_icd/6.3/linux/Release/bin/x64 \
                                        /opt/intel/dldt/inference-engine/thirdparty/intel_ocl_icd/lib &&\
    mv ./bin/intel64/Release/lib        /opt/intel/dldt/inference-engine/lib/intel64 && \
    mv ./include                        /opt/intel/dldt/inference-engine/ && \
    mv ./src                            /opt/intel/dldt/inference-engine/ && \
    mv ./build/share                    /opt/intel/dldt/inference-engine/ && \
    mv ./temp/opencv_4.1.0_ubuntu18     /opt/intel/dldt/inference-engine/external/opencv && \
    mv ./temp/gna/linux                 /opt/intel/dldt/inference-engine/external/gna && \
    mv ./temp/tbb                       /opt/intel/dldt/inference-engine/external/ && \
    \
    cd /tmp && rm -rf dldt

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

COPY --from=openvino /opt /opt

RUN echo "\
/opt/intel/dldt/inference-engine/thirdparty/intel_ocl_icd/lib\n\
/opt/intel/dldt/inference-engine/lib/intel64\n\
/opt/intel/dldt/inference-engine/external/gna/lib\n\
/opt/intel/dldt/inference-engine/external/tbb/lib\n\
/opt/intel/dldt/inference-engine/external/opencv/lib" > /etc/ld.so.conf.d/opencv-dldt-gst.conf && ldconfig

ENV OpenCV_DIR=/opt/intel/dldt/inference-engine/external/opencv/cmake
ENV InferenceEngine_DIR=/opt/intel/dldt/inference-engine/share

RUN useradd -ms /bin/bash user
USER user

COPY --chown=user:user . /home/user/upscaler

WORKDIR /home/user/upscaler

RUN mkdir build && cd build && cmake .. && make -j $(nproc)

ENV GST_PLUGIN_PATH=/home/user/upscaler/build/gst/elements
