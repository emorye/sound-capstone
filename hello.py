#Setting OpenPose parameters
import cv2
import numpy as np
from openpose import pyopenpose as op

HEAD = 0
CHEST = 1
RIGHT_ELBOW = 3
RIGHT_SHOULDER = 2
RIGHT_HAND = 4

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
        params["model_folder"] = "../../models/"
        return params

def main():

	print "hello"

        params = set_params()

        #Constructing OpenPose object allocates GPU memory
        opWrapper = op.WrapperPython()
	opWrapper.configure(params)
	opWrapper.start()

        #Opening OpenCV stream
        stream = cv2.VideoCapture(1)

        font = cv2.FONT_HERSHEY_SIMPLEX


        while True:
		datum = op.Datum()
		
                ret,img = stream.read()
		datum.cvInputData = img

		# Send the data to open pose
		opWrapper.emplaceAndPop([datum])

		keypoints = datum.poseKeypoints
		if keypoints is not None and keypoints.size > 1:
			keypoints = keypoints[0]
			if keypoints[RIGHT_HAND][1] < keypoints[CHEST][1] and keypoints[RIGHT_HAND][2] > 0.1:
				print "right hand raised"
		overlay = cv2.addWeighted(img, 0.9, datum.cvOutputData, 0.5, 0)
                cv2.imshow('Human Pose Estimation', overlay)

                key = cv2.waitKey(1)

                if key==ord('q'):
                        break

        stream.release()
        cv2.destroyAllWindows()

if __name__ == "__main__":
	main()
