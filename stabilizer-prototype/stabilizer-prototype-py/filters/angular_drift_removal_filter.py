from filters import AxisAccessor, GyroDataFilterBase
from gyro_data import GyroData

import numpy as np

import matplotlib.pyplot as plt

# not online


class AngularDriftRemovalFilter(GyroDataFilterBase):
    def apply_filter(self, gyro_data: GyroData) -> GyroData:
        arr = gyro_data.to_numpy_array()
        orig = arr.copy()
        mean = np.mean(arr[:, GyroData.slice_angular], axis=0)
        print(f'{arr.shape=}, {mean.shape=}, {mean=}')
        arr[:, GyroData.slice_angular] -= mean
        for x in range(len(arr)):
            print(f'{orig[x, GyroData.slice_angular]} --> {arr[x, GyroData.slice_angular]}')
        plt.plot(gyro_data.timestamps(), arr[:, GyroData.slice_angular])
        plt.plot(gyro_data.timestamps(), orig[:, GyroData.slice_angular], '.')
        plt.show()
        print(arr)
        # return gyro_data
        return GyroData.from_ndarray(gyro_data.timestamps(), arr)
