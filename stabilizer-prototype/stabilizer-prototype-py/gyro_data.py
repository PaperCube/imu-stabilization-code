import bisect

import numpy as np

from collections import namedtuple
from typing import *


class GyroData:
    class GyroEntry(namedtuple('GyroEntry', ['t', 'x', 'y', 'z'])):
        __slots__ = ()

        @property
        def pos(self) -> np.ndarray:
            return np.array([self.x, self.y, self.z])

    data: list[GyroEntry]

    @staticmethod
    def load_from_file(file_path: str, drift: Iterable[float] = [0, 0, 0]):
        dx, dy, dz = drift[:3]
        with open(file_path) as f:
            lines = f.readlines()
        ret = GyroData()
        for ln in lines:
            if not ln.strip():
                continue
            time, x, y, z = map(float, ln.split(',')[:4])
            ret.data.append(
                GyroData.GyroEntry._make((time, x - dx, y - dy, z - dz))
            )
        return ret

    def __init__(self):
        self.data = []
        pass

    def __len__(self):
        return len(self.data)

    def __getitem__(self, key) -> 'GyroData.GyroEntry':
        return self.data[key]

    def lowerbound(self, value, key: Callable[[GyroEntry], Any] = lambda x: x.t) -> int:
        l = list(map(key, self.data))
        idx = bisect.bisect_left(l, value)
        return idx

    def get_nearest_idx(self, value, key: Callable[[GyroEntry], Any] = lambda x: x.t) -> int:
        idx = self.lowerbound(value, key)
        if idx == 0:
            return idx
        if idx == len(self):
            return idx - 1
        if abs(key(self.data[idx]) - value) < abs(key(self.data[idx - 1]) - value):
            return idx
        else:
            return idx - 1

    def __iter__(self):
        return iter(self.data)
