import numpy as np
import pandas as pd

import pathlib

from tqdm import tqdm

home = pathlib.Path(
    r'D:\Projects\ML\imu-stabilization\stabilizer-prototype\output\sample 012')


def read_headless_csv(path):
    return pd.read_csv(path, header=None).to_numpy()


def read_roi_timestamps(path):
    return read_headless_csv(path)


def read_roi_center(path):
    roi = read_headless_csv(path)
    return [roi[:, a] + roi[:, a + 2] / 2 for a in range(2)]


def read_gyro_readings(path, time_begin=-float('inf')):
    gyro = read_headless_csv(path)
    gt = gyro[:, 0]
    gtime_slice = gt > time_begin
    gx, gy, gz = gyro[gtime_slice][:, 7:10].T
    return gt[gtime_slice], gx, gy, gz


roi_t = read_roi_timestamps(home / 'framestamps.txt')
gt, gx, gy, gz = read_gyro_readings(home / 'gyro.txt', roi_t[0])
roi_x, roi_y = read_roi_center(home / 'roi.csv')

# compare roi_x and gz


def compare(a, b):
    if a > b:
        return 1
    elif a < b:
        return -1
    assert a == b, f'bad comparison on type {type(a), type(b)}'
    return 0


def trend_segmentify(arr):
    result = []
    n = len(arr)
    last_direction = None
    last_update = 0
    for i in range(1, n):
        cur_direction = compare(arr[i], arr[i - 1])
        if last_direction != cur_direction:
            if last_direction is not None:
                result.append((last_update, i - 1, last_direction))
            last_direction = cur_direction
            last_update = i - 1
    result.append((last_update, n - 1, last_direction))
    return result


def interval_intersection(int1, int2):
    b1, e1 = int1
    b2, e2 = int2

    max_b = max(b1, b2)
    min_e = min(e1, e2)

    return max(0, min_e - max_b)


def trend_consistency(s1, t1, s2, t2, *, td1=0, td2=0):
    # return np.correlate(v1, v2)
    i1, i2 = 0, 0
    n1, n2 = len(s1), len(s2)
    # assert len(s1) == len(t1) and len(s2) == len(t2)

    result = 0

    while i1 < n1 and i2 < n2:
        # print(i1, n1, i2, n2)

        # sync
        begin1, end1, dir1 = s1[i1]
        begin2, end2, dir2 = s2[i2]
        tb1, tb2 = t1[begin1] + td1, t2[begin2] + td2
        te1, te2 = t1[end1] + td1, t2[end2] + td2

        _time_info = f'{s1[i1]}[{tb1}-{te1}], {s2[i2]}[{tb2}-{te2}]'

        if dir1 == dir2:
            overlap_size = interval_intersection(
                (tb1, te1), (tb2, te2))
            # print(f'{_time_info}: overlap {overlap_size}')
            result += overlap_size
        else:
            # print((f'{_time_info}: directions differ'))
            pass

        if (te1 < te2 and i1 < n1 - 1) or i2 + 1 >= n2:
            i1 += 1
        else:
            i2 += 1

    return result

print('roi_t time range', roi_t[0], roi_t[-1], f'({roi_t[-1] - roi_t[0]})')

results = []

seg1 = trend_segmentify(roi_x)
seg2 = trend_segmentify(gz)
for td1 in tqdm(np.arange(-0.1, -0.05, 0.0001)):
    res = trend_consistency(
        seg1,
        roi_t,
        seg2, 
        gt, 
        td1=td1
    )
    results.append((td1, res))

results.sort(key=lambda x: x[1], reverse=True)
print(results[:20])

'''
-1, 1, 0.005

(-0.07999999999999918, array([13.131])),
(-0.07499999999999918, array([13.033])),
(-0.08499999999999919, array([12.868])),
(-0.06999999999999917, array([12.593])),
(-0.08999999999999919, array([12.288])),
(-0.06499999999999917, array([11.822])),
(-0.0949999999999992, array([11.436])),
(-0.059999999999999165, array([10.781])),
(-0.0999999999999992, array([10.389])),
(0.1450000000000009, array([9.997])),
(0.140000000000001, array([9.948])),
(0.15000000000000102, array([9.916])),
(0.030000000000000915, array([9.799])),
(0.03500000000000103, array([9.796])),
(0.13500000000000112, array([9.771])),
(0.15500000000000114, array([9.691])),
(0.0250000000000008, array([9.656])),
(0.040000000000000924, array([9.637])),
(-0.05499999999999916, array([9.579])),
(-0.4199999999999995,  array([9.575]))
'''

'''
[(-0.0789999999999994, array([13.139])),
(-0.0790999999999994, array([13.1382])),
(-0.0788999999999994, array([13.1382])),
(-0.0787999999999994, array([13.1374])),
(-0.07919999999999941, array([13.1374])),
(-0.07929999999999941, array([13.1366])),
(-0.0786999999999994, array([13.1366])),
(-0.07859999999999939, array([13.1358])),
(-0.07939999999999942, array([13.1358])),
(-0.07949999999999942, array([13.135])),
(-0.07849999999999939, array([13.135])),
(-0.07839999999999939, array([13.1342])),
(-0.07959999999999942, array([13.1342])),
(-0.07829999999999938, array([13.1334])),
(-0.07969999999999942, array([13.1334])),
(-0.07819999999999938, array([13.1326])),
(-0.07979999999999943, array([13.1326])),
(-0.07809999999999938, array([13.1318])),
(-0.07989999999999943, array([13.1318])),
(-0.07999999999999943, array([13.131]))]
'''
