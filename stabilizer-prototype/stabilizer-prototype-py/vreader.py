import bisect
import time
import math
import signal
import os
import threading
from collections import namedtuple
from typing import *

import cv2
import numpy as np
import scipy as sp
import functools
import itertools

from scipy.spatial.transform import Rotation as R

import axis_remapping
from dotdict import dotdict
from numberstore import NumberStore, ConfigItem
from homography import create_intrinsic_matrix
from gyro_data import GyroData

from filters import *

base_dir = r"D:\Documents\CUST\毕业设计\Stage 02-Examples\manifold_motion_smoothing\data\\"
base_dir = r"D:\Projects\ML\imu-stabilization\stabilizer-prototype\output\sample 004\\"

filter = MovingAverageFilter(1)
# filter = ButterworthFilter(8, .01, 'lowpass')
# filter = NullFilter()

files = dotdict({
    'framestamps': base_dir + "framestamps.txt",
    'gyro': base_dir + "gyro.txt",
    'video': base_dir + "video_sample.avi"
})

vinfo = dotdict({
    'fps': 30,
    'vsize': None,
})

# hwinfo = dotdict({
#     'gyro_diff': 0.1555,
#     'gyro_drift': -np.array([-0.0082, 0.0052, 0.0155]), # note: negated compared to matlab codes
#     'f': 649.2773,
# })

hwinfo = dotdict({
    'gyro_diff': 0.0,
    # note: negated compared to matlab codes
    'gyro_drift': np.array([0, 0, 0]),
    'f': 447.4,
})


def load_framestamps(path: str) -> list[float]:
    ret = []
    with open(path) as f:
        lines = f.readlines()
    for ln in lines:
        if not ln.strip():
            continue
        ret.append(float(ln))
    return ret


gyro_data: GyroData
vcap: cv2.VideoCapture
framestamps: list[float]


def preload_data():
    global gyro_data, vcap, framestamps
    print('Loading framestamps')
    framestamps = load_framestamps(files.framestamps)

    print('Loading gyro data')
    gyro_data = GyroData.load_from_file(files.gyro, drift=hwinfo.gyro_drift)
    gyro_data.discard_before(framestamps[0])
    gyro_data = AngularDriftRemovalFilter().apply_filter(gyro_data)
    gyro_data = filter.apply_filter(gyro_data)

    vcap = cv2.VideoCapture(files.video)
    vinfo.vsize = (int(vcap.get(cv2.CAP_PROP_FRAME_WIDTH)),
                   int(vcap.get(cv2.CAP_PROP_FRAME_HEIGHT)))

    print('integrating gyro data')
    load_gyro_prefix_sum()


def synced_framestamps() -> Iterator:
    time_start = None

    for frame_id, framestamp in enumerate(framestamps):
        if time_start is None:
            time_start = time.perf_counter() - framestamp
        time_elapsed = time.perf_counter() - time_start
        time_offset = framestamp - time_elapsed
        yield frame_id, framestamp, time_offset


def load_frames() -> list[cv2.Mat]:
    ret = []
    while True:
        success, frame = vcap.read()
        if not success:
            break
        ret.append(frame)
    return ret


viewer_hud = dotdict()


def render_viewer_hud(
    frame: cv2.Mat,
    *,
    line_spacing: int = 30
) -> cv2.Mat:
    items = sorted(viewer_hud.items(), key=lambda x: x[0])
    ln_number = 0
    for k, v in items:
        cv2.putText(
            frame,
            f'{k}: {v}',
            (10, 30 + ln_number * line_spacing),
            cv2.FONT_HERSHEY_SIMPLEX,
            0.5,
            color=(100, 100, 255),
            thickness=1
        )
        ln_number += 1
    return frame


@functools.cache
def load_gyro_prefix_sum() -> np.ndarray:
    '''
    They cannot be directly added as they are space vectors
    '''

    average_report_interval = gyro_data.average_report_interval()
    print('gyro_average_report_rate:', average_report_interval)

    matrices = []
    rot_matrix_pre = np.eye(3)
    for g in gyro_data:
        x, y, z = g.angular
        rot = R.from_euler(
            'xyz',
            np.array([x, y, z]) * average_report_interval,
            degrees=True
        ).as_matrix()
        rot_integrated_matrix = rot_matrix_pre @ rot
        matrices.append(rot_integrated_matrix)
        rot_matrix_pre = rot_integrated_matrix
    int_angle = np.empty((len(matrices), 3))
    for i, m in enumerate(matrices):
        int_angle[i] = R.from_matrix(m).as_euler('xyz', degrees=True)

    int_movement = np.zeros((len(matrices), 3))
    for i, g in enumerate(gyro_data):
        movement_delta = (g.linacc - [0, 0, 9.9]) * \
            0.1 * average_report_interval
        int_movement[i] += movement_delta
        if i:
            int_movement[i] += int_movement[i - 1]
        # print(f'delta = {movement_delta} --> integrated: {int_movement[i]}')

    return int_angle, int_movement


_ratio = 1


@functools.cache
def K():
    return create_intrinsic_matrix(
        hwinfo.f,
        hwinfo.f,
        vinfo.vsize[0] / 2,
        vinfo.vsize[1] / 2
    )


def warp_image(frame: cv2.Mat,
               current_integrated_gyro_data: Tuple[np.ndarray, np.ndarray],
               direction=None  # default: 0+1+2+
               ) -> cv2.Mat:
    if direction is None:
        direction = '0+1+2+'
    angle, mvmt = current_integrated_gyro_data
    gyro = np.empty_like(angle)
    axis_remapping.remap_axis(direction, angle, gyro)

    # print(f'{direction} {current_integrated_gyro_data} -> {gyro}')
    # gyro = np.zeros_like(gyro)  # (testing) disable angular compensation
    rot_mat = R.from_euler('xyz', gyro, degrees=True).as_matrix()

    # todo combine with translation data
    ntd = np.array([0, 0, 0]).reshape(1, 3)
    translation = -axis_remapping.remap_axis(
        '2-0-1-', mvmt, np.empty_like(mvmt)
    ).reshape(3, 1) @ ntd
    hom_upd = K() @ (rot_mat - translation) @ np.linalg.inv(K())
    # print(f'rot_mat = {rot_mat}, \nmvmt @ ntd (translation)= {translation}')
    return cv2.warpPerspective(frame, hom_upd, vinfo.vsize)


direction_mappings = '1-2-0+'


def viewer_process_frame(frame: cv2.Mat, framestamp: float) -> cv2.Mat:
    nearest_gyro_idx = gyro_data.get_nearest_idx(framestamp - hwinfo.gyro_diff)
    gyro_cur = gyro_data[nearest_gyro_idx]
    gyro_prev = gyro_data[nearest_gyro_idx - 1] \
        if nearest_gyro_idx > 0 else None

    viewer_hud._00_framestamp = framestamp
    viewer_hud._01_gyro_data = gyro_cur

    int_angle, int_mvmt = load_gyro_prefix_sum()
    cur_int_angle = int_angle[nearest_gyro_idx]
    cur_int_mvmt = int_mvmt[nearest_gyro_idx]
    viewer_hud._01_gyro_int_angle = cur_int_angle
    viewer_hud._01_gyro_int_mvmt = cur_int_mvmt

    return render_viewer_hud(warp_image(frame, (cur_int_angle, cur_int_mvmt), direction_mappings))
    return render_viewer_hud(frame)


def frame_viewer():
    frames = load_frames()
    store.get_field_by_name('frame_id').bounds = (0, len(frames) - 1)
    while True:
        frame_i, = store['frame_id']

        frame = frames[frame_i]
        framestamp = framestamps[frame_i]
        cv2.imshow('frame', viewer_process_frame(frame, framestamp))
        cv2.imshow('original', frame)

        key = cv2.waitKey(0)
        changed_config = store.handle_key(key)
        if changed_config:
            print(changed_config)
        elif key == ord('q'):
            break


def play_video(*, write_file=False):
    if write_file:
        video_writer = cv2.VideoWriter(
            'corrected_frames.mp4', cv2.VideoWriter_fourcc(*"mp4v"), 30, vinfo.vsize)

    for frame_id, framestamp, time_offset in synced_framestamps():
        ret, frame = vcap.read()
        print(f'frame_id: {frame_id}, framestamp: {framestamp}, time_offset: {time_offset}',
              '[skip]' if time_offset < 0 else '')

        time_offset_millis = int(time_offset * 1000)
        if time_offset_millis > 0:
            key = cv2.waitKey(time_offset_millis)
            if key == ord('s'):
                print("Skipping...")
                break
        elif time_offset < 0:
            continue
        # time.sleep(0.1)
        assert ret

        altered_frame = viewer_process_frame(frame, framestamp)

        if write_file:
            video_writer.write(altered_frame)
        cv2.imshow('frame', altered_frame)
        cv2.imshow('original', frame)
    print('This is', direction_mappings)
    if write_file:
        video_writer.release()
    cv2.waitKey(0)

# Possible:
# 0+2+1+


def play_all_possible_videos():
    starter = input('Starting from? ')
    arr = list(axis_remapping.permutations(3))
    try:
        starter_idx = arr.index(starter)
    except ValueError:
        starter_idx = 0
        input('Not found. Press enter to start from 0')
    for dm in arr[starter_idx:]:
        global direction_mappings
        direction_mappings = dm
        play_video()
        print('it was', dm)
        preload_data()

cv_worker_is_alive = True

def cv_worker():
    global cv_worker_is_alive
    try:
        preload_data()
        # play_all_possible_videos()
        # frame_viewer()
        play_video(write_file=False)
    except Exception as e:
        import traceback
        import signal
        print('cv_worker exception:', e, traceback.format_exc())
    cv_worker_is_alive = False


def lifecycle_worker():
    global cv_worker_is_alive
    try:
        while cv_worker_is_alive:
            time.sleep(0.2)
    except KeyboardInterrupt:
        print('KeyboardInterrupt')

    print("Terminating")
    os.kill(os.getpid(), signal.SIGTERM)

if __name__ == '__main__':
    lifecycle_worker_thread = threading.Thread(target=lifecycle_worker, daemon=True)
    lifecycle_worker_thread.start()

    store = NumberStore(
        data_fields=[
            ConfigItem('frame_id', ', . ', step=1, mod_scale=1),
        ]
    )

    try:
        cv_worker()
    except KeyboardInterrupt:
        print('KeyboardInterrupt (main)')

    
