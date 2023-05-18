import cv2

path = r'D:\Projects\ML\imu-stabilization\stabilizer-prototype\output\sample 012\video_sample.avi'

def read_frames():
    vc = cv2.VideoCapture(path)
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
    return frames

def main():
    frames = read_frames()

    box = cv2.selectROI('roi', frames[0], True, False)
    # print(box)
    # print(','.join(list(map(str, box))))
    
    tracker = cv2.TrackerCSRT_create()
    tracker.init(frames[0], box)
    for frame in frames:
        success, bbox = tracker.update(frame)
        if success:
            # print(bbox)
            print(','.join(list(map(str, bbox))))

            x, y, w, h = [int(v) for v in bbox]
            cv2.rectangle(frame, (x, y), (x+w, y+h), (0, 255, 0), 2)
        cv2.imshow('frame', frame)
        if cv2.waitKey(1) & 0xFF == ord('q'):
            break

if __name__ == '__main__':
    main()