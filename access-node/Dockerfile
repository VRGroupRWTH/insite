FROM ubuntu:jammy 

RUN apt-get update && apt-get install -y git ninja-build libssl-dev cmake build-essential 
COPY ./access_node ./access_node/

WORKDIR build_access_node

RUN cmake -G Ninja ../access_node -DBUILD_SHARED_LIBS=OFF
RUN ninja insite-access-node

FROM ubuntu:jammy
RUN apt-get update && apt-get install -y libssl-dev
COPY --from=0 /build_access_node/insite-access-node /insite-access-node

ENTRYPOINT ["./insite-access-node"]
