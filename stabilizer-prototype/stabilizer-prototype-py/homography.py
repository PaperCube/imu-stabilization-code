import cv2
import threading
import time
import os
import sys
import random
import math

import numpy as np
import scipy as sp
import sympy as smp

from typing import *
from numberstore import NumberStore

shutdown_flag = False


config = NumberStore(
    data_fields=[
        NumberStore.ConfigItem('rot', 'wWsSaAdDqQeE'),
        NumberStore.ConfigItem('focal_ratio', '-_=+',
                               step=0.1, mod_scale=5, display_func=lambda x: math.exp(x)),
        NumberStore.ConfigItem('trl', 'uUjJhHkK[{]}', step=0.2, mod_scale=5),
    ]
)


homography = np.array([[1, 0, 0], [0, 1, 0], [0, 0, 1]])


def create_intrinsic_matrix(fx, fy, cx, cy):
    return np.array([[fx, 0, cx], [0, fy, cy], [0, 0, 1]])


# K: np.ndarray = create_intrinsic_matrix(960, 960, 480, 480)


def normalize_homography(hom):
    return hom / hom[2, 2]


def process_image(img_mat):
    try:
        from scipy.spatial.transform import Rotation as R

        rx, ry, rz = config['rot']
        ratio_log,  = config['focal_ratio']
        tx, ty, tz = config['trl']
        t_vec = np.array([tx, ty, tz]).reshape(3, 1)
        normal = np.array([0, 0, 1]).reshape(3, 1)

        ratio = math.exp(ratio_log)
        K = create_intrinsic_matrix(960 * ratio, 960 * ratio, 480, 480)

        rot_mat = R.from_euler('xyz', [rx, ry, rz], degrees=True).as_matrix()
        hom_upd = K @ (rot_mat - t_vec @
                       normal.T) @ homography @ np.linalg.inv(K)

        # print('processing img with', values)
        # apply homography
        return cv2.warpPerspective(img_mat, hom_upd, (img_mat.shape[1], img_mat.shape[0]))
    except Exception as e:
        print(e)
        raise e


def cv_worker():
    fps = 30
    global shutdown_flag
    while not shutdown_flag:
        mat = cv2.imread("../grid.png")
        mat = process_image(mat)
        # print(mat)
        if mat is None:
            time.sleep(1/fps)
            continue

        cv2.imshow('image', mat)

        key = cv2.waitKey(delay=0)

        key_ch = chr(key & 0xFF)
        pos = config['rot']

        if (alt_field := config.handle_key(key_ch)) is not None:
            print('rot', config['rot'], 'exp(focal_ratio) =',
                  math.exp(config['focal_ratio'][0]))
            print(alt_field)
        elif key_ch == 'z':
            config.reset()


def main_worker():
    global shutdown_flag
    while not shutdown_flag:
        try:
            ln = input()
        except KeyboardInterrupt:
            shutdown_flag = True
            break

        if ln.startswith('q'):
            shutdown_flag = True
            break
        if ln.startswith('!'):
            try:
                print(eval(ln.lstrip('!')))
            except Exception as e:
                print(e)


if __name__ == '__main__':
    thread = threading.Thread(target=cv_worker, daemon=True)
    thread.start()

    main_worker()
