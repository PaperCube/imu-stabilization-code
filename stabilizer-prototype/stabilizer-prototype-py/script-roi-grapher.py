import pandas as pd
import numpy as np

import matplotlib.pyplot as plt
import matplotlib.font_manager as font_manager

import pathlib

font = {'fontname':'Times New Roman'}
# fontprop = font_manager.FontProperties(family='Noto Serif SC', size=14)

plt.rcParams["font.family"] = font['fontname']
plt.rcParams['font.size'] = 12

def load_roi_center_from_file(path, *, header=None):
    path = pathlib.Path(path)

    data = pd.read_csv(path, header=header).to_numpy()
    result = np.array([
        data[:, 0] + data[:, 2] / 2, 
        data[:, 1] + data[:, 3] / 2
    ])
    # print(data)
    # print(result)
    return result

def data_columns(data):
    ret = []
    for i in range(data.shape[0]):
        ret.append(data[i, :])
    return ret

def log_difference(data):
    result = 0
    prev = data[0]
    for i in range(1, len(data)):
        result += np.log(data[i] / prev) ** 2
        prev = data[i]
    return result / len(data)

source_path = pathlib.Path(r'D:\Documents\CUST\毕业设计\Notes of Essay - Obsidian Vault\tex-files\res\sensor-graphs\stabilize-eval\s013')

t_roi = pd.read_csv(source_path / 'framestamps.txt', header=None).to_numpy().reshape(-1)

d1 = data_columns(load_roi_center_from_file(source_path / 'corrected_frames.mp4.csv'))
for i, col in enumerate(d1):
    plt.plot(t_roi, col, color='black')

d2 = data_columns(load_roi_center_from_file(source_path / 'uncorrected.mp4.csv'))
for i, col in enumerate(d2):
    plt.plot(t_roi, col, '--', color='#AAAAAA')


condition = t_roi > 25
for data in [d1, d2]:
    for i in range(2):
        # print(data[i])
        # print(f'{i}: {np.mean(data[i][condition])} ± {np.std(data[i][condition])}')
        print(f'{i}: {log_difference(data[i][condition])}')

plt.xlabel('time (s)')
plt.ylabel('position (px)')
plt.show()