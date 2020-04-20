FROM ubuntu:latest
ARG DEBIAN_FRONTEND=noninteractive
RUN apt-get update && apt-get install -y \
    cmake g++ make ninja-build python3 python3-dev python3-pip python3-numpy python3-scipy python3-matplotlib \
    git gsl-bin libgsl0-dev libltdl-dev libtool netcat \
    libboost-atomic-dev libboost-thread-dev libboost-system-dev libboost-date-time-dev libboost-regex-dev \
    libboost-filesystem-dev libboost-random-dev libboost-chrono-dev libboost-serialization-dev \
    libwebsocketpp-dev openssl libssl-dev ninja-build \
    openmpi-bin libopenmpi-dev libpq-dev postgresql-server-dev-all
RUN pip3 install Cython

RUN git clone --single-branch --branch master https://github.com/nest/nest-simulator.git nest && \
    cd nest && \
    git checkout 5c0f41230dda9e4b99b8df89729ea43b340246ad && \
    cd /
WORKDIR /nest-build
RUN cmake \
    -G Ninja \
    -Dwith-mpi=ON \
    -DCMAKE_INSTALL_PREFIX=/nest-install \
    -DCMAKE_BUILD_TYPE=Release \
    /nest
RUN ninja && ninja install

RUN git clone --single-branch --branch v2.10.14 --recurse-submodules https://github.com/microsoft/cpprestsdk.git /cpprestsdk
WORKDIR /cpprestsdk-build
RUN cmake \
    -G Ninja \
    -DCMAKE_BUILD_TYPE=Release \
    -DBUILD_TESTS=OFF \
    /cpprestsdk
RUN ninja && ninja install

RUN git clone --single-branch --branch 6.4.6 https://github.com/jtv/libpqxx.git /libpqxx
WORKDIR /libpqxx-build
RUN cmake \
    -G Ninja \
    -DCMAKE_BUILD_TYPE=Release \
    -DBUILD_TESTS=OFF \
    /libpqxx
RUN ninja && ninja install

COPY . /insite
WORKDIR /insite-build
RUN cmake \
    -G Ninja \
    -Dwith-nest=/nest-install/bin/nest-config \
    -DCMAKE_BUILD_TYPE=Release \
    /insite
RUN ninja && ninja install
ENV PGPASSWORD=postgres

EXPOSE 8000
ENTRYPOINT "/insite-build/run_brunel_simulation.sh"
CMD 1000 2500 2