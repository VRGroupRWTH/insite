FROM nestsim/nest:3.0

RUN apt-get update && apt-get install -y \
    automake \
    autotools-dev \
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
    libltdl-dev \
    libreadline-dev \
    libreadline8 \
    libssl-dev \
    libtool \
    libwebsocketpp-dev \
    make \
    netcat \
    ninja-build \
    openssl

RUN git clone --single-branch --branch v2.10.16 --recurse-submodules https://github.com/microsoft/cpprestsdk.git /cpprestsdk
WORKDIR /cpprestsdk-build
RUN cmake \
    -G Ninja \
    -DCMAKE_BUILD_TYPE=Release \
    -DBUILD_TESTS=OFF \
    /cpprestsdk
RUN ninja && ninja install

COPY src /insite-module
WORKDIR /insite-module-build
RUN cmake \
    -G Ninja \
    -Dwith-nest=/opt/nest/bin/nest-config \
    -DCMAKE_BUILD_TYPE=Release \
    /insite-module
RUN ninja && ninja install
ENV PGPASSWORD=postgres

ENTRYPOINT ["/insite-module-build/run_nest_server.sh"]
