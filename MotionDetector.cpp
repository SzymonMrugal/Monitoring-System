#include <opencv2/opencv.hpp>
#include <opencv2/tracking.hpp>
#include <opencv2/core/ocl.hpp>
#include <unistd.h>
#include <string>

using namespace std;
using namespace cv;

class MotionDetector{
	
	private:
		Mat prev, curr; // lighter versions of previous and current images
		
		
	public:
		Mat thersh; // for debugging
		
		MotionDetector(Mat frame){
			cvtColor(frame, curr, COLOR_BGR2GRAY);
			GaussianBlur(curr, curr, Size(21, 21), 0);
			curr.copyTo(prev);  // both frames are the same at the beginning
		}
		
		void pushFrame(Mat frame){  // push next frame to container
			curr.copyTo(prev);
			cvtColor(frame, curr, COLOR_BGR2GRAY);
			GaussianBlur(curr, curr, Size(21, 21), 0);
		}
		
		bool isMotion(Mat frame){  // frame difference algorithm
			pushFrame(frame);
			vector<vector<Point>> cnts;
			
			absdiff(prev, curr, frame);
			threshold(frame, frame, 5, 255, THRESH_BINARY);
			dilate(frame, frame, Mat(), Point(-1,-1), 2);
			frame.copyTo(thersh); // for debugging
			findContours(frame, cnts, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
			
			bool mot_detected = false;
			for(int i = 0; i < cnts.size(); i++) {  // check if any contour is big enough
				int area = contourArea(cnts[i]);
				if(area > 100 ) {
					mot_detected = true;
					break;
				}
			}
			
			return mot_detected;
		}
		
};
