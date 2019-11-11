FROM ubuntu:latest
ARG DEBIAN_FRONTEND=noninteractive
RUN apt-get update && apt-get install -y \
    cmake g++ make ninja-build python3 python3-dev python3-pip python3-numpy python3-scipy python3-matplotlib \
    git gsl-bin libgsl0-dev libltdl-dev libtool \
    libboost-atomic-dev libboost-thread-dev libboost-system-dev libboost-date-time-dev libboost-regex-dev \
    libboost-filesystem-dev libboost-random-dev libboost-chrono-dev libboost-serialization-dev \
    libwebsocketpp-dev openssl libssl-dev ninja-build
RUN pip3 install Cython
RUN git clone --single-branch --branch 098ee030f1027be8eb05ee20bc794ea7863c2ef4 https://github.com/jougs/nest-simulator.git nest
WORKDIR /nest-build
RUN cmake \
    -G Ninja \
    -DCMAKE_INSTALL_PREFIX=/nest-install \
    -DCMAKE_BUILD_TYPE=Release \
    /nest
RUN ninja && ninja install
RUN git clone --single-branch --branch v2.10.14 --recurse-submodules https://github.com/microsoft/cpprestsdk.git /cpprestsdk
WORKDIR /cpprestsdk-build
RUN cmake \
    -G Ninja \
    -DCMAKE_BUILD_TYPE=Release \
    /cpprestsdk
RUN ninja && ninja install
COPY . /insite
WORKDIR /insite-build
RUN cmake \
    -G Ninja \
    -Dwith-nest=/nest-install/bin/nest-config \
    -DCMAKE_BUILD_TYPE=Release \
    /insite
RUN ninja && ninja install
ENTRYPOINT [ "/insite-build/run_brunel_simulation.sh" ]