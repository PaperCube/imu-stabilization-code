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
    
    @staticmethod 
    def from_ndarray(timestamps: Iterable[float], values: np.ndarray) -> 'GyroData':
        assert len(timestamps) == len(values)
        ret = GyroData()
        for t, v in zip(timestamps, values):
            ret.data.append(GyroData.GyroEntry(t, *v))
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

    def average_report_interval(self):
        return (self.data[-1].t - self.data[0].t) / len(self.data)

    def to_numpy_array(self) -> np.ndarray:
        """Returns values excluding timestamps
        """
        return np.array([x.pos for x in self.data])
    
    def timestamps(self) -> np.ndarray:
        return np.array([x.t for x in self.data])

    def discard_before(self, first):
        idx = self.lowerbound(first)
        self.data = self.data[idx:]
