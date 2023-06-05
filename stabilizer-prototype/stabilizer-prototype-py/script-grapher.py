import pandas as pd
import numpy as np

import matplotlib.pyplot as plt
import matplotlib.font_manager as font_manager

import pathlib


font = {'fontname':'Noto Serif SC'}
fontprop = font_manager.FontProperties(family='Noto Serif SC', size=14)

plt.rcParams["font.family"] = font['fontname']

labels = ['roi_x roi_y'.split(), 'gx gy gz'.split()]

def remap(arr, minv=0, maxv=1):
    am = np.max(arr)
    an = np.min(arr)
    return (arr - an) / (am - an) * (maxv - minv) + minv

path = pathlib.Path(
    r'D:\Projects\ML\imu-stabilization\stabilizer-prototype\output\sample 012')

roi = pd.read_csv(path / 'roi.csv', header=None).to_numpy()
gyro = pd.read_csv(path / 'gyro.txt', header=None).to_numpy()
t_roi = pd.read_csv(path / 'framestamps.txt', header=None).to_numpy()

roi_x, roi_y = [roi[:, a] + roi[:, a + 2] / 2 for a in range(2)]

roi_time_begin = t_roi[0]

gt = gyro[:, 0]
gtime_slice = gt > roi_time_begin
gt = gt[gtime_slice]
gx, gy, gz = gyro[gtime_slice][:, 7:10].T

for i, y in enumerate([remap(roi_x), remap(roi_y)]):
    if True or i in [0]:
        plt.plot(t_roi, y, label=labels[0][i])
for i, y in enumerate([gx, gy, gz]):
    if True or i in [2]:
        plt.plot(gt, remap(y), '--', label=labels[1][i])
# plt.legend(prop=fontprop)
plt.show()