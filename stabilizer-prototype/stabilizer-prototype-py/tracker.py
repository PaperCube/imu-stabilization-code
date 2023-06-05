import pathlib

import cv2

from function_menu import *

path = r"D:\Projects\ML\imu-stabilization\stabilizer-prototype\output\sample 013\uncorrected.mp4"

def read_frames():
    vc = cv2.VideoCapture(str(path))
    assert vc.isOpened(), 'Cannot open video file'
    frames = []
    while True:
        ret, frame = vc.read()
        if not ret:
            break
        frame = cv2.resize(frame, (0, 0), fx=2, fy=2)
        # cv2.imshow('frame', frame)
        frames.append(frame)
        # if cv2.waitKey(1) & 0xFF == ord('q'):
        #     break
    print(f'Read {len(frames)} frames')
    return frames

def main():
    global path, output
    path = pathlib.Path(path)
    output = path.parent / f'{path.name}.csv'

    print('Will write tracking result to', output)

    frames = read_frames()

    box = cv2.selectROI('roi', frames[0], True, False)
    # print(box)
    print('selected range :', ','.join(list(map(str, box))))

    track_result_file = open(output, 'w', encoding='utf-8')
    
    tracker = cv2.TrackerCSRT_create()
    tracker.init(frames[0], box)
    for frame in frames:
        success, bbox = tracker.update(frame)
        if success:
            # print(bbox)
            line_str = ','.join(list(map(str, bbox)))
            print(line_str)
            
            track_result_file.write(line_str + '\n')

            x, y, w, h = [int(v) for v in bbox]
            cv2.rectangle(frame, (x, y), (x+w, y+h), (0, 255, 0), 2)
        cv2.imshow('frame', frame)
        if cv2.waitKey(1) & 0xFF == ord('q'):
            break
    track_result_file.close()
    print('Written to', output)
    cv2.destroyAllWindows()

if __name__ == '__main__':
    show_menu_run_once([
        FuncRef(main), 
    ])