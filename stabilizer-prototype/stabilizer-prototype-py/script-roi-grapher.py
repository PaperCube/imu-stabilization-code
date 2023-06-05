import pandas as pd
import numpy as np

import matplotlib.pyplot as plt
import matplotlib.font_manager as font_manager

import pathlib

font = {'fontname':'Noto Serif SC'}
fontprop = font_manager.FontProperties(family='Noto Serif SC', size=14)

plt.rcParams["font.family"] = font['fontname']

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

source_path = pathlib.Path(r'D:\Documents\CUST\毕业设计\Notes of Essay - Obsidian Vault\tex-files\res\sensor-graphs\stabilize-eval\s013')

t_roi = pd.read_csv(source_path / 'framestamps.txt', header=None).to_numpy()

files = ['corrected_frames.mp4.csv', 'uncorrected.mp4.csv']

for file in files:
    data = load_roi_center_from_file(source_path / file)
    # print(f'{data[0]=}')
    for i in range(data.shape[0]):
        plt.plot(t_roi, data[i, :])
plt.show()