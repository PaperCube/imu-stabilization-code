import itertools

from typing import *
from collections.abc import *

__sign = {
    '+': 1,
    '-': -1,
    'z': 0
}


def remap_axis(direction: str, source: list[Any], dest=None):
    assert len(direction) % 2 == 0
    result_size = 0
    for i in range(0, len(direction), 2):
        result_size = max(int(direction[i]) + 1, result_size)
    if dest is None:
        dest = [None] * result_size
    assert len(dest) >= result_size
    for i in range(3):
        mi, d = direction[i * 2], direction[i * 2 + 1]
        assert mi in '0123456789' and d in __sign.keys()
        dest[i] = source[int(mi)] * __sign[d]
    return dest


def permutations(size: int, direction_domain='+-'):
    for indexes in itertools.permutations(list(range(size))):
        for direction in itertools.product(direction_domain, repeat=size):
            yield ''.join(map(lambda x: x[0] + x[1], zip(indexes, direction)))
