# PyHPX

This project is an initial attempt to port [HPX](https://github.com/STEllAR-GROUP/hpx) in Python using [Pybind11](https://github.com/pybind/pybind11)

Currently it builds locally by running the following commands:

`python3 setup.py build -j <num_of_jobs>`

`pip3 install dist/pyhpx-0.0.1-cp38-cp38-<platform>.whl`

### Examples

```python
>>> import pyhpx
>>> pyhpx.is_heap([1,2,3])
False
>>> pyhpx.is_heap((3,2,1))
True
```
