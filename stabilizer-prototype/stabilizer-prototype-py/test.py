from collections import namedtuple
from filters.filter_base import AxisAccessor

G = namedtuple('G', 'x y z')

value = [
    G(x, x + 100, x + 10000) for x in range(9)
]

AxisAccessor(value, [0])[1] = 1000
AxisAccessor(value, 'x')[1] = 1000
print(list(AxisAccessor(value, [0])))
print(list(AxisAccessor(value, 'y')))
