FROM ubuntu:20.04
ARG DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    cmake \
    g++ \
    git \
    gsl-bin \
    libboost-atomic-dev \
    libboost-chrono-dev \
    libboost-date-time-dev \
    libboost-filesystem-dev \
    libboost-random-dev \
    libboost-regex-dev \
    libboost-serialization-dev \
    libboost-system-dev \
    libboost-thread-dev \
    libgsl0-dev \
    libltdl-dev \
    libopenmpi-dev \
    libssl-dev \
    libtool \
    libwebsocketpp-dev  \
    make \
    netcat \
    ninja-build \
    openmpi-bin \
    openssl \
    pandoc \
    python3 \
    python3-dev \
    python3-matplotlib \
    python3-numpy \
    python3-pip \
    python3-scipy \
    python3-venv \
    uwsgi


# Install Python package to build NEST Simulator
RUN python3 -m pip install --upgrade \
    Cython \
    pip
    # setuptools
    # wheel

# Install Python package for NEST Server
RUN python3 -m pip install --upgrade \
    flask-cors \
    flask \
    pandoc \
    RestrictedPython \
    uwsgi


# Install C++ REST SDK
RUN git clone \
    --branch v2.10.16 \
    --recurse-submodules \
    --single-branch \
    https://github.com/microsoft/cpprestsdk.git /cpprestsdk
WORKDIR /cpprestsdk-build
RUN cmake \
    -G Ninja \
    -DCMAKE_BUILD_TYPE=Release \
    -DBUILD_TESTS=OFF \
    /cpprestsdk
RUN ninja && ninja install


# Install NEST Simulator
RUN git clone \
    --branch nest-server-command \
    --depth 1 \
    https://github.com/babsey/nest-simulator /nest
RUN python3 -m pip install -r /nest/doc/requirements.txt
WORKDIR /nest-build
RUN cmake \
    -G Ninja \
    -Dwith-mpi=ON \
    -Dwith-python=ON\
    -DCMAKE_INSTALL_PREFIX=/nest-install \
    -DCMAKE_BUILD_TYPE=Release \
    /nest
RUN ninja \
    && ninja html \
    && ninja install


# Install Insite Module
COPY src /insite-module
WORKDIR /insite-module-build
RUN cmake \
    -G Ninja \
    -Dwith-nest=/nest-install/bin/nest-config \
    -DCMAKE_BUILD_TYPE=Release \
    /insite-module
RUN ninja && ninja install
ENV PGPASSWORD=postgres

COPY example /example

ENTRYPOINT ["/insite-module-build/start_script.sh"]
