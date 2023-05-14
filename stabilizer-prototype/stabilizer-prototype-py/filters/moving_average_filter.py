from filters import AxisAccessor, GyroDataFilterBase

import numpy as np

from gyro_data import GyroData

class MovingAverageFilter(GyroDataFilterBase):
    def __init__(self, half_window):
        self._half_window = half_window

    def apply_filter(self, gdata: GyroData) -> GyroData:
        half_window = self._half_window

        data = gdata.to_numpy_array()
        orig_data = data.copy()
        for i in range(len(data)):
            sliced = orig_data[max(0, i - half_window):i + half_window + 1]
            # print(f'{data=}\n{sliced=}')
            data[i, :] = np.mean(sliced, axis=0)
        return GyroData.from_ndarray(gdata.timestamps(), data)