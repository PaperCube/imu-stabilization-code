from gyro_data import GyroData
from filters import GyroDataFilterBase

import scipy as sp
from scipy import signal

# from scipy import signal


class ButterworthFilter(GyroDataFilterBase):
    def __init__(self, n, wn, btype='low', analog=False, output='ba', fs=None):
        self._ff = signal.butter(n, wn, btype, analog, output, fs)

    def apply_filter(self, gyro_data: GyroData) -> GyroData:
        arr = signal.filtfilt(*self._ff, gyro_data.to_numpy_array(), axis=0)
        return GyroData.from_ndarray(gyro_data.timestamps(), arr)
