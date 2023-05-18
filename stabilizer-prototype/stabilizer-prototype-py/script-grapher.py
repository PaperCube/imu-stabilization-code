import pandas as pd
import numpy as np

import matplotlib.pyplot as plt

import pathlib


def counter() -> int:
    global __counter
    if not '__counter' in globals():
        __counter = 0
    __counter += 1
    return __counter

def remap(arr, minv=0, maxv=1):
    am = np.max(arr)
    an = np.min(arr)
    return (arr - an) / (am - an) * (maxv - minv) + minv

path = pathlib.Path(
    r'D:\Projects\ML\imu-stabilization\stabilizer-prototype\output\sample 012')

roi = pd.read_csv(path / 'roi.csv', header=None).to_numpy()
gyro = pd.read_csv(path / 'gyro.txt', header=None).to_numpy()
t_roi = pd.read_csv(path / 'framestamps.txt', header=None).to_numpy()

roi_x, roi_y = [roi[:, a] + roi[:, a + 1] / 2 for a in range(2)]

roi_time_begin = t_roi[0]

gt = gyro[:, 0]
gtime_slice = gt > roi_time_begin
gt = gt[gtime_slice]
gx, gy, gz = gyro[gtime_slice][:, 7:10].T

for y in [remap(roi_x), remap(roi_y)]:
    plt.plot(t_roi, y, label=str(counter()))
for y in [gx, gy, gz]:
    plt.plot(gt, remap(y), '--', label=str(counter()))
plt.legend()
plt.show()