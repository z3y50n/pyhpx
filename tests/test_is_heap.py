import pytest
import pyhpx

def test_is_heap():
    assert pyhpx.is_heap([1,2,3]) == False
    assert pyhpx.is_heap([3,2,1]) == True
    with pytest.raises(TypeError):
        pyhpx.is_heap(range(10))
