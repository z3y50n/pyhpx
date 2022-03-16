cmake_minimum_required(VERSION 3.18)
project(pyhpx CXX)

add_subdirectory(pybind11)
pybind11_add_module(pyhpx src/is_heap.cpp)

find_package(HPX)

IF(HPX_FOUND)
    message("Found HPX")
    target_link_libraries(pyhpx PUBLIC HPX::hpx)
ELSE()
    message("Could not found HPX")
    message("Proceed to build it")
    add_subdirectory(hpx)
    target_link_libraries(pyhpx PUBLIC hpx)
ENDIF()
