import bisect

import numpy as np

from collections import namedtuple
from typing import *


class GyroData:
    slice_linacc = slice(0, 3)
    slice_angular = slice(3, 6)
    slice_quaternion = slice(6, 10)

    class GyroEntry:
        def __init__(self, t, values):
            self.t = float(t)
            assert len(values) == 10
            self.values = np.array(values)
            # if len(self.values) > 10:
            #     raise ValueError(f"values too long")
            # if len(self.values) < 10:
            #     self.values.extend([0] * (10 - len(self.values)))

        @staticmethod
        def compose(t, linacc, angular, quaternion):
            assert (len(linacc) == 3 and len(angular)
                    == 3 and len(quaternion) == 4)
            return GyroData.GyroEntry(t, [*linacc, *angular, *quaternion])

        def __getitem__(self, idx):
            return self.values[idx]

        @property
        def linacc(self) -> np.ndarray:
            return np.array(self[GyroData.slice_linacc])

        @property
        def angular(self) -> np.ndarray:
            return np.array(self[GyroData.slice_angular])

        @property
        def quaternion(self) -> np.ndarray:
            return np.array(self[GyroData.slice_quaternion])

        def __repr__(self) -> str:
            lx, ly, lz = self.linacc
            ax, ay, az = self.angular
            qw, qx, qy, qz = self.quaternion
            return f'@{self.t:.4f}: linacc({lx:.3f}, {ly:.3f}, {lz:.3f}), angular({ax:.3f}, {ay:.3f}, {az:.3f}), qtn({qw:.3f}, {qx:.3f}, {qy:.3f}, {qz:.3f}))'

    data: list[GyroEntry]

    @staticmethod
    def __auto_extract_values(val: Iterable[float]) -> Iterable[float]:
        # return value is angular velocity? confirm first.
        val = list(val)
        if len(val) == 4:
            return val[0], val[1:4], [0] * 3, [0] * 4
        return val[0], val[1:4], val[4:7], val[13:17]

    @staticmethod
    def load_from_file(file_path: str, drift: Iterable[float] = [0, 0, 0]):
        # dx, dy, dz = drift[:3]
        with open(file_path) as f:
            lines = f.readlines()
        ret = GyroData()
        for ln in lines:
            if not ln.strip():
                continue
            time, linacc, angular, quaternion = GyroData.__auto_extract_values(
                map(float, ln.strip().rstrip(",").split(',')))
            ret.data.append(
                GyroData.GyroEntry.compose(time, linacc, angular, quaternion)
            )
        return ret

    @staticmethod
    def from_ndarray(timestamps: Iterable[float], values: np.ndarray) -> 'GyroData':
        assert len(timestamps) == len(values)
        ret = GyroData()
        for t, v in zip(timestamps, values):
            ret.data.append(GyroData.GyroEntry(t, v))
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
        return np.array([x.values for x in self.data])

    def timestamps(self) -> np.ndarray:
        return np.array([x.t for x in self.data])

    def discard_before(self, first):
        idx = self.lowerbound(first)
        self.data = self.data[idx:]
