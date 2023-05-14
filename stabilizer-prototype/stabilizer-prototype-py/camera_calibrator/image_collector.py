import cv2
import time
import datetime
import pathlib


output_path = r'D:\Projects\ML\imu-stabilization\stabilizer-prototype\output\calibration 001\\'


def main():
    camera = cv2.VideoCapture(0)
    assert camera.isOpened(), "Failed to open camera"

    while True:
        ret, frame = camera.read()
        if not ret:
            break
        cv2.imshow("frame", frame)
        key = cv2.waitKey(1)
        if key == ord('q'):
            break
        if key == ord('s'):
            current_time_str_url_safe = str(
                datetime.datetime.now()).replace(" ", "_").replace(":", "-")
            path = pathlib.Path(
                output_path + current_time_str_url_safe + ".jpg")
            path.parent.mkdir(parents=True, exist_ok=True)
            result = cv2.imwrite(str(path), frame)
            print('saved to', path, f' - result = {result}')

def test_chessboard():
    camera = cv2.VideoCapture(0)
    assert camera.isOpened(), "Failed to open camera"

    while True:
        ret, frame = camera.read()
        if not ret:
            break
        cv2.imshow("frame", frame)
        key = cv2.waitKey(1)
        if key == ord('s'):
            chessboard = cv2.findChessboardCorners(frame, (12, 5))
            print(chessboard)


if __name__ == '__main__':
    # main()
    test_chessboard()
