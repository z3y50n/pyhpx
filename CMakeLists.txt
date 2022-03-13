cmake_minimum_required(VERSION 3.18)
project(pyhpx CXX)

add_subdirectory(pybind11)
pybind11_add_module(pyhpx src/is_heap.cpp)

add_subdirectory(hpx)
target_link_libraries(pyhpx PUBLIC hpx)
