from gyro_data import GyroData
from filters import GyroDataFilterBase

class NullFilter(GyroDataFilterBase):
    def apply_filter(self, gyro_data: GyroData) -> GyroData:
        return gyro_data