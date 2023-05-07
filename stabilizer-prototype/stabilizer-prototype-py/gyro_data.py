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
    def __auto_extract_values(val: Iterable[float]) -> Iterable[float]:
        # return value is angular velocity? confirm first.
        val = list(val)
        if len(val) == 4:
            return val
        return val[0], *val[4:7]

    @staticmethod
    def load_from_file(file_path: str, drift: Iterable[float] = [0, 0, 0]):
        dx, dy, dz = drift[:3]
        with open(file_path) as f:
            lines = f.readlines()
        ret = GyroData()
        for ln in lines:
            if not ln.strip():
                continue
            time, x, y, z = GyroData.__auto_extract_values(
                map(float, ln.strip().rstrip(",").split(',')))
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

    def simple_filter_inplace(self, half_window=10):
        orig_data = self.data[:]
        for i in range(len(self.data)):
            sliced = orig_data[max(0, i - half_window):i + half_window + 1]
            self.data[i] = GyroData.GyroEntry(
                self.data[i].t,
                np.mean([x.x for x in sliced]),
                np.mean([x.y for x in sliced]),
                np.mean([x.z for x in sliced]),
            )


    def __iter__(self):
        return iter(self.data)
