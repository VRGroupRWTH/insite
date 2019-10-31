FROM ubuntu:latest
RUN apt-get update && apt-get install -y cmake g++ make python3 python3-dev python3-pip git gsl-bin libgsl0-dev libltdl-dev libtool libboost-atomic-dev libboost-thread-dev libboost-system-dev libboost-date-time-dev libboost-regex-dev libboost-filesystem-dev libboost-random-dev libboost-chrono-dev libboost-serialization-dev libwebsocketpp-dev openssl libssl-dev ninja-build
RUN pip3 install Cython
RUN git clone --single-branch --branch nestio https://github.com/jougs/nest-simulator.git nest
WORKDIR /nest-build
RUN cmake \
    -DCMAKE_INSTALL_PREFIX=/nest-install \
    -DCMAKE_BUILD_TYPE=Release \
    /nest
RUN make && make install
RUN git clone --single-branch --branch v2.10.14 --recurse-submodules https://github.com/microsoft/cpprestsdk.git /cpprestsdk
WORKDIR /cpprestsdk-build
RUN cmake \
    -DCMAKE_BUILD_TYPE=Release \
    /cpprestsdk
RUN make && make install
ARG DEBIAN_FRONTEND=noninteractive
RUN apt install -y python3-numpy python3-scipy python3-matplotlib
COPY . /insite
WORKDIR /insite-build
RUN cmake \
    -Dwith-nest=/nest-install/bin/nest-config \
    -DCMAKE_BUILD_TYPE=Release \
    /insite
RUN make -j2 && make install
CMD [ "/insite/examples/run.sh" ]
# CMD [ "python3", "-m", "http.server", "8000" ]