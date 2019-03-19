#Setting OpenPose parameters
from webcamvideostream import WebcamVideoStream
import ctrl
import cv2
import numpy as np
import os
import threading
import time

from openpose import pyopenpose as op


HEAD = 0
CHEST = 1
RIGHT_ELBOW = 3
RIGHT_SHOULDER = 2
RIGHT_HAND = 4

LEFT_ELBOW = 6
LEFT_SHOULDER = 5
LEFT_HAND = 7

CONFIDENCE_THRESH = 0.1

def set_params():
    params = dict()
    params["logging_level"] = 3
    params["camera_resolution"] = "256x256"
    params["output_resolution"] = "640x480"
    params["net_resolution"] = "128x96"
    params["model_pose"] = "BODY_25"
    params["keypoint_scale"] = "3"
    params["alpha_pose"] = 0.6
    params["scale_gap"] = 0.3
    params["scale_number"] = 1
    params["render_threshold"] = 0.05

    # If GPU version is built, and multiple GPUs are available, set the ID here
    params["num_gpu_start"] = 0
    params["disable_blending"] = False

    # Ensure you point to the correct path where models are located
    params["model_folder"] = os.path.expanduser("~/openpose/models/")
    return params

## geometry helper fx

def distance(points1, points2):
    return np.linalg.norm(points1[:2] - points2[:2])

def slope(points1, points2):
    return (points2[1]-points1[1])/(points2[0]-points1[0])

def standard_body_unit(keypoints):
    return np.linalg.norm(keypoints[HEAD][:2] - keypoints[CHEST][:2])

def inner_angle(a, b, c):
        a_b = distance(a, b)
        a_c = distance(a, c)
        b_c = distance(b, c)
        return np.arccos((a_b ** 2 + b_c ** 2 - a_c ** 2) / (2 * a_b * b_c))

## pose detectors

def left_hand_raised(keypoints, prev):
    return keypoints[LEFT_HAND][1] < keypoints[CHEST][1] and keypoints[LEFT_HAND][2] > CONFIDENCE_THRESH

def right_hand_raised(keypoints, prev):
    return keypoints[RIGHT_HAND][1] < keypoints[CHEST][1] and keypoints[RIGHT_HAND][2] > CONFIDENCE_THRESH

def arms_folded(keypoints, prev):
    # "close enough" distance?
    close_enough = .5 * standard_body_unit(keypoints)
    right_hand = distance(keypoints[RIGHT_ELBOW], keypoints[LEFT_HAND])
    # print "close_enough: " + str(close_enough) + ", right_hand: " + str(right_hand) 
    return right_hand < close_enough \
             and distance(keypoints[LEFT_ELBOW], keypoints[RIGHT_HAND]) < close_enough 

def left_hand_crossbody(keypoints, prev):
    return keypoints[LEFT_HAND][0] < keypoints[CHEST][0]

def right_hand_crossbody(keypoints, prev):
    return keypoints[RIGHT_HAND][0] > keypoints[CHEST][0]

def left_arm_horiz(keypoints, prev):
    if keypoints[LEFT_SHOULDER][2] > CONFIDENCE_THRESH and keypoints[LEFT_ELBOW][2] > CONFIDENCE_THRESH and keypoints[LEFT_HAND][2] > CONFIDENCE_THRESH:
        close_enough = .3 * standard_body_unit(keypoints)
        elbow = abs(keypoints[LEFT_SHOULDER][1] - keypoints[LEFT_ELBOW][1])
        hand =  abs(keypoints[LEFT_SHOULDER][1] - keypoints[LEFT_HAND][1]) 
        # print "hand: " + str(hand) + ", elbow: " + str(elbow) + ", close enough: " + str(close_enough)
        return elbow < close_enough and hand < close_enough
    else:
        return False


def right_arm_horiz(keypoints, prev):
    if keypoints[RIGHT_SHOULDER][2] > CONFIDENCE_THRESH and keypoints[RIGHT_ELBOW][2] > CONFIDENCE_THRESH and keypoints[RIGHT_HAND][2] > CONFIDENCE_THRESH:
        close_enough = .3 * standard_body_unit(keypoints)
        elbow = abs(keypoints[RIGHT_SHOULDER][1] - keypoints[RIGHT_ELBOW][1])
        hand =  abs(keypoints[RIGHT_SHOULDER][1] - keypoints[RIGHT_HAND][1]) 
        # print "hand: " + str(hand) + ", elbow: " + str(elbow) + ", close enough: " + str(close_enough)
        return elbow < close_enough and hand < close_enough
    else:
        return False

def left_arm_straight(keypoints, prev): 
    if keypoints[LEFT_SHOULDER][2] > CONFIDENCE_THRESH and keypoints[LEFT_ELBOW][2] > CONFIDENCE_THRESH and keypoints[LEFT_HAND][2] > CONFIDENCE_THRESH:
        elbow_angle = inner_angle(keypoints[LEFT_SHOULDER], keypoints[LEFT_ELBOW], keypoints[LEFT_HAND])
        # print "elbow_angle: " + str(elbow_angle)
        return abs(elbow_angle - np.pi) < .3
    else:
        return False

def right_arm_straight(keypoints, prev): 
    if keypoints[RIGHT_SHOULDER][2] > CONFIDENCE_THRESH and keypoints[RIGHT_ELBOW][2] > CONFIDENCE_THRESH and keypoints[RIGHT_HAND][2] > CONFIDENCE_THRESH:
        elbow_angle = inner_angle(keypoints[RIGHT_SHOULDER], keypoints[RIGHT_ELBOW], keypoints[RIGHT_HAND])
        # print "elbow_angle: " + str(elbow_angle)
        return abs(elbow_angle - np.pi) < .3
    else:
        return False

# handle both hands:
# both hands up -> play melody a
# distance between hands determines velocity
# if your hands are together and down, where they end up, signifies which creature to play melody a
# 

def left_hand_ctrl(cc):
    m = ctrl.MEL1()

    def step(keypoints, prev):
        m.start()
        offset = keypoints[LEFT_HAND][1] * 20 + 50
        velocity = int(keypoints[RIGHT_HAND][1] * 100)
        print "VELOCITY: " + str(velocity)
        m.set_transpose(offset)
        m.set_velocity(velocity)
        if m.done():
            m.restart()

    cc.add_melody(m)

    return step


def main(cc):
    params = set_params()

    # Constructing OpenPose object allocates GPU memory
    opWrapper = op.WrapperPython()
    opWrapper.configure(params)
    opWrapper.start()

    # Opening OpenCV stream
    #stream = cv2.VideoCapture(1)
    stream = WebcamVideoStream(src=1)
    stream.start()

    prev_keypoints = None
    pose_detectors = [left_hand_raised, right_hand_raised, 
                      arms_folded, left_hand_crossbody, right_hand_crossbody, 
                      left_arm_horiz, right_arm_horiz, left_arm_straight, right_arm_straight]

    count = 0
    curtime = time.time()

    cc.start()

    lhc = left_hand_ctrl(cc)

    while True:
        count += 1
        datum = op.Datum()

        img = stream.read()
        datum.cvInputData = img

        # Send the data to open pose
        opWrapper.emplaceAndPop([datum])
	poses = []

        keypoints = datum.poseKeypoints
        if keypoints is not None and keypoints.size > 1:
            keypoints = keypoints[0]

            lhc(keypoints, prev_keypoints)
            # left_hand = keypoints[LEFT_HAND][1] * 0.01
            # right_hand = 
            # m1 = ctrl.MEL1()

            # cc.set_tempo_scale(left_hand)
            # cc.set_offset(right_hand)

        overlay = cv2.addWeighted(img, 0.9, datum.cvOutputData, 0.5, 0)
        cv2.imshow('Human Pose Estimation', overlay)

        prev_keypoints = keypoints

        key = cv2.waitKey(1)

        if key==ord('q'):
            stream.stop()
            break

    print "freq: " + str(count / (time.time() - curtime))
    stream.release()
    cv2.destroyAllWindows()
    cc.join()

if __name__ == '__main__':
    cc = ctrl.CreatureController(port='/dev/ttyACM0',
                                 event='note_on',
                                 channel=1)
    main(cc)
