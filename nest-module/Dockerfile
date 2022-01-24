FROM ubuntu:20.04
ARG DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    cmake \
    cython3 \
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
    python3-pip \
    python3-venv \
    uwsgi \
    wget

RUN apt-get autoremove -y && \
    rm -rf /var/lib/apt/lists/* && \
    update-alternatives --install /usr/bin/python python /usr/bin/python3 10


# Install Python package for NEST Server
RUN python3 -m pip install \
    flask \
    flask-cors \
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

#
# Install NEST Simulator
ARG NEST_VERSION=3.2
ARG NEST_SRC_PATH=/tmp

# Get released files.
RUN wget https://github.com/nest/nest-simulator/archive/refs/tags/v${NEST_VERSION}.tar.gz -P ${NEST_SRC_PATH} && \
    cd ${NEST_SRC_PATH} && tar -zxf v${NEST_VERSION}.tar.gz

# Patch version number in NEST
RUN wget https://github.com/nest/nest-simulator/releases/download/v3.2/nest-simulator-3.2-p1-VersionNumber.patch -P ${NEST_SRC_PATH} && \
    cd ${NEST_SRC_PATH}/nest-simulator-${NEST_VERSION} && \
    patch -p1 < ${NEST_SRC_PATH}/nest-simulator-3.2-p1-VersionNumber.patch

# Install requirements for Sphinx documentation of NEST
RUN python3 -m pip install -r ${NEST_SRC_PATH}/nest-simulator-${NEST_VERSION}/doc/requirements.txt

# Compile and install NEST Simulator
WORKDIR /nest-build
RUN cmake \
    -G Ninja \
    -Dwith-mpi=ON \
    -Dwith-python=ON\
    -DCMAKE_INSTALL_PREFIX=/nest-install \
    -DCMAKE_BUILD_TYPE=Release \
    ${NEST_SRC_PATH}/nest-simulator-${NEST_VERSION}
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
