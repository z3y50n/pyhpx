#include <hpx/algorithm.hpp>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>


template <typename T>
bool my_is_heap(T t) {
    bool ih = hpx::is_heap(t.begin(), t.end());
    return ih;
}

int add(int a, int b) {
    return a + b;
}

namespace py = pybind11;

PYBIND11_MODULE(pyhpx, m) {
    m.doc() = "pybind11 is_heap plugin";
    m.def("is_heap", &my_is_heap<py::list>, "A function that checks if list is a heap");
    m.def("is_heap", &my_is_heap<py::tuple>, "A function that checks if tuple is a heap");
    m.def("add", &add, "A function that adds two integers");
}
