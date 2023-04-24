import bisect
import time
import math
from collections import namedtuple
from typing import *

import cv2
import threading

from dotdict import dotdict
from numberstore import NumberStore, ConfigItem

files = dotdict({
    'framestamps': r"D:\Documents\CUST\毕业设计\Stage 02-Examples\manifold_motion_smoothing\data\framestamps.txt",
    'gyro': r"D:\Documents\CUST\毕业设计\Stage 02-Examples\manifold_motion_smoothing\data\gyro.txt",
    'video': r"D:\Documents\CUST\毕业设计\Stage 02-Examples\manifold_motion_smoothing\data\video_sample.avi"
})

vinfo = dotdict({
    'fps': 30,
    'vsize': (720, 480)
})


class GyroData:
    GyroEntry = namedtuple('GyroEntry', 't x y z')

    data: list[GyroEntry]

    @staticmethod
    def load_from_file(file_path: str):
        with open(file_path) as f:
            lines = f.readlines()
        ret = GyroData()
        for ln in lines:
            if not ln.strip():
                continue
            time, x, y, z = ln.split(',')[:4]
            ret.data.append(GyroData.GyroEntry._make((time, x, y, z)))

    def __init__(self):
        self.data = []
        pass

    def __len__(self):
        return len(self.data)

    def __getitem__(self, key) -> 'GyroData.GyroEntry':
        return self.data[key]

    def lowerbound(self, value, key: Callable[[GyroEntry], Any] = GyroEntry.t) -> int:
        l = list(map(key, self.data))
        idx = bisect.bisect_left(l, value)
        return idx

    def get_nearest_idx(self, value, key: Callable[[GyroEntry], Any] = GyroEntry.t) -> int:
        idx = self.lowerbound(value, key)
        if idx == 0:
            return idx
        if idx == len(self):
            return idx - 1
        if abs(key(self.data[idx]) - value) < abs(key(self.data[idx - 1]) - value):
            return idx
        else:
            return idx - 1


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
    gyro_data = GyroData.load_from_file(files.gyro)
    vcap = cv2.VideoCapture(files.video)
    framestamps = load_framestamps(files.framestamps)


def synced_framestamps() -> Iterator:
    time_start = time.perf_counter()

    for frame_id, framestamp in enumerate(framestamps):
        time_elapsed = time.perf_counter() - time_start
        time_offset = framestamp - time_elapsed
        yield frame_id, framestamp, time_offset


def compensate_for_frame(frame, framestamp):
    t, x, y, z = gyro_data.get_nearest_idx(framestamp)


def play_video():
    for frame_id, framestamp, time_offset in synced_framestamps():
        print(f'frame_id: {frame_id}, framestamp: {framestamp}, time_offset: {time_offset}',
              '[skip]' if time_offset < 0 else '')
        ret, frame = vcap.read()

        time_offset_millis = int(time_offset * 1000)
        if time_offset_millis > 0:
            cv2.waitKey(time_offset_millis)
        elif time_offset < 0:
            continue
        # time.sleep(0.1)
        assert ret

        altered_frame = compensate_for_frame(frame, framestamp)

        cv2.imshow('frame', altered_frame)
    cv2.waitKey(0)


def load_frames() -> list[cv2.Mat]:
    ret = []
    while True:
        success, frame = vcap.read()
        if not success:
            break
        ret.append(frame)
    return ret


def frame_viewer():
    frames = load_frames()
    store.get_field_by_name('frame_id').bounds = (0, len(frames) - 1)
    while True:
        frame_i, = store['frame_id']
        cv2.imshow('frame', frames[frame_i])
        store.handle_key(cv2.waitKey(0))
        print(store)


def cv_worker():
    preload_data()
    frame_viewer()


if __name__ == '__main__':
    cv_worker_thread = threading.Thread(target=cv_worker, daemon=True)
    cv_worker_thread.start()

    store = NumberStore(
        data_fields=[
            ConfigItem('frame_id', ', . ', step=1, mod_scale=1),
        ]
    )

    try:
        while cv_worker_thread.is_alive():
            time.sleep(0.2)
    except KeyboardInterrupt:
        print('KeyboardInterrupt')
