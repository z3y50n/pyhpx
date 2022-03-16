# PyHPX

This project is an initial attempt to port [HPX](https://github.com/STEllAR-GROUP/hpx) in Python using [Pybind11](https://github.com/pybind/pybind11)

### Install

To build local `.so` file:

`python3 setup.py build -j <num_of_jobs>`

To build wheel and install:

`CMAKE_BUILD_PARALLEL_LEVEL=<num_of_jobs> python3 setup.py bdist_wheel`

`pip3 install dist/pyhpx-0.0.1-cp38-cp38-<platform>.whl`

### Examples

```python
>>> import pyhpx
>>> pyhpx.is_heap([1,2,3])
False
>>> pyhpx.is_heap((3,2,1))
True
```

### Benchmarks

Running `python3 benchmarks/is_heap.py`:

```
Python is_heap: 0.879698056000052s
PyHPX is_heap: 1.1873999028466642e-05s
```
