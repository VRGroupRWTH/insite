cmake_minimum_required(VERSION 3.15)
project(insite-arbor-ring-example)

find_package(insite-arbor REQUIRED)
add_subdirectory(sup)

add_executable(ring ring.cpp)
target_link_libraries(ring PUBLIC arbor-sup insite-arbor)
