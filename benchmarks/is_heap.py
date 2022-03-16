import pyhpx
import timeit

def is_heap(A):
    return all(A[i] >= A[(i - 1) // 2] for i in range(1, len(A)))

A = list(range(10000000))

python_is_heap = timeit.timeit(lambda: is_heap(A), number=1)
pyhpx_is_heap = timeit.timeit(lambda: pyhpx.is_heap(A), number=1)

print(f"Python is_heap: {python_is_heap}s")
print(f"PyHPX is_heap: {pyhpx_is_heap}s")
