FROM alpine

RUN apk update && apk add git ninja openssl-dev cmake build-base linux-headers
COPY ./src ./access_node/src
COPY ./include ./access_node/include
COPY ./CMakeLists.txt ./access_node/CMakeLists.txt
COPY ./CPM.cmake ./access_node/CPM.cmake

WORKDIR access_node_build

RUN cmake -G Ninja ../access_node -DBUILD_SHARED_LIBS=OFF
RUN ninja insite-access-node

# FROM ubuntu:jammy
FROM alpine
RUN apk update && apk add openssl-dev
COPY --from=0 /access_node_build/insite-access-node /insite-access-node

ENTRYPOINT ["./insite-access-node"]
