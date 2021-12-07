FROM ubuntu:20.04
ARG DEBIAN_FRONTEND=noninteractive
RUN apt-get update && apt-get install -y \
    cmake g++ make ninja-build python3 python3-dev python3-pip python3-numpy python3-scipy python3-matplotlib \
    git gsl-bin libgsl0-dev libltdl-dev libtool netcat \
    libboost-atomic-dev libboost-thread-dev libboost-system-dev libboost-date-time-dev libboost-regex-dev \
    libboost-filesystem-dev libboost-random-dev libboost-chrono-dev libboost-serialization-dev \
    libwebsocketpp-dev openssl libssl-dev ninja-build \
    openmpi-bin libopenmpi-dev python3-venv pandoc
RUN pip3 install Cython

RUN git clone --single-branch --branch v2.10.16 --recurse-submodules https://github.com/microsoft/cpprestsdk.git /cpprestsdk
WORKDIR /cpprestsdk-build
RUN cmake \
    -G Ninja \
    -DCMAKE_BUILD_TYPE=Release \
    -DBUILD_TESTS=OFF \
    /cpprestsdk
RUN ninja && ninja install

RUN git clone --depth 1 --branch v3.1 https://github.com/nest/nest-simulator /nest
WORKDIR /nest-build
RUN cmake \
    -G Ninja \
    -Dwith-mpi=ON \
    -Dwith-python=ON\
    -DCMAKE_INSTALL_PREFIX=/nest-install \
    -DCMAKE_BUILD_TYPE=Release \
    /nest


RUN python3 -m pip install --upgrade pip setuptools \
    && pip3 install wheel \
                    pandoc\
                    numpy\
                    RestrictedPython\
                    flask\
                    flask-cors\
                    uwsgi
RUN pip3 install -r /nest/extras/nest-simulator-doc-requirements.txt


RUN ninja \
    && ninja html \
    && ninja install


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

ENTRYPOINT ["/insite-module-build/run_simulation.sh"]
CMD ["/example/brunel_simulation.py"]
