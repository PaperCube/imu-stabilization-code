import cv2

import pathlib

from typing import *

from dotdict import dotdict

config: dict = dotdict({
    'static_frame_path': pathlib.Path(r"D:\Projects\ML\imu-stabilization\stabilizer-prototype\grid_640.png"),
    'output_parent_dir': pathlib.Path(r"D:\Projects\ML\imu-stabilization\stabilizer-prototype\output"),
    'output_dir': None,

    'vlength': 5,
    'focal_length': 447.9,
    'fps': 30
})


def warp_image(
    image: cv2.Mat,
    rotation_euler,
    translation,
) -> cv2.Mat:
    raise NotImplemented()


def next_config_dir() -> str:
    opd: pathlib.Path = config.output_parent_dir
    subdirnames = [file.name for file in opd.iterdir()]
    name_tmpl = 'forged {:03d}'
    for i in range(999, 0, -1):
        if name_tmpl.format(i) in subdirnames:
            if i == 999:
                raise RuntimeError(f"Too many folders in {opd}")
            return opd / name_tmpl.format(i + 1)


def write_video():
    frame = cv2.imread(config.static_frame_path)
    vwriter = cv2.VideoWriter(str(config.output_dir / "video_sample.avi"),
                              cv2.VideoWriter_fourcc(*"MJPG"), config.fps, (640, 640))
    with open(config.output_dir / 'framestamps.txt', 'w') as fs_file: 
        framestamp = 0.0
        while framestamp < config.vlength:
            vwriter.write(frame)
            fs_file.write(f'{framestamp:.7f}\n')
            framestamp += 1 / config.fps
            


def run():
    config.output_dir = next_config_dir()
    write_video()
    print(config)
    pass


if __name__ == '__main__':
    run()
