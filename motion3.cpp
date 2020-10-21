/*****************************************************
* Author:					Szymon Mrugal
* Last modification date:	03.01.2019
*
* Parameters defined in RunMonitorSys file
*****************************************************/

#include <opencv2/opencv.hpp>
#include <opencv2/tracking.hpp>
#include <opencv2/core/ocl.hpp>
#include <unistd.h>
#include "MotionDetector.cpp"
#include "Saver.cpp"

using namespace std;
using namespace cv;

struct Parameters{  // default values defined below
	string CAMERA_PATH;
	string ROOTPATH;
	string FILENAME;
	int FPS;
	int PRESAVE;
	int POSTSAVE;
	int maxMotion;
	int sendNotif;
} param ;

//Parameters which are not passed
const string SNAPSHOTNAME = "snapshot.jpg";
Size sz;	//pictures size
const int CODECS = CV_FOURCC('X','V','I','D');

int main(int argc, char** argv){
	
	if (argc!=9){
		cerr<<"WARNING: invalid number of input arguments. Check RunMonitorSys file.\n Using default values"<<endl;
		//Dafault parameters values
		param.CAMERA_PATH = "http://localhost:8080/?action=stream";
		param.ROOTPATH =  "/home/pi/Desktop/MotDet_out/";
		param.FILENAME = "__out.avi";
		param.FPS = 10;
		param.PRESAVE = 20;
		param.POSTSAVE = 20;
		param.maxMotion = 6; //how many frames in motion to start saving
		param.sendNotif = 1; //whether to send notifications
	}else{
		param.CAMERA_PATH = argv[1];
		param.ROOTPATH =  argv[2];
		param.FILENAME = argv[3];
		param.FPS = atoi(argv[4]);
		param.PRESAVE = atoi(argv[5]);
		param.POSTSAVE = atoi(argv[6]);
		param.maxMotion = atoi(argv[7]);
		param.sendNotif = atoi(argv[8]);
	}
	
	cout<<"Using parameters: \n"; // for debugging
	cout<<param.CAMERA_PATH<<", "
	<<param.ROOTPATH<<", "
	<<param.FILENAME<<", "
	<<param.FPS<<", "
	<<param.PRESAVE<<", "
	<<param.POSTSAVE<<", "
	<<param.maxMotion<<", "
	<<param.sendNotif<<"\n";

	int motionCount = 0;
	int afterCount = param.POSTSAVE;
	
	Mat frame; // current frame
	VideoCapture camera(param.CAMERA_PATH); //open camera
	if(!camera.isOpened()){cerr<<"ERROR: connot open camera"<<endl; return -1;}
	
	sz = Size(camera.get(CV_CAP_PROP_FRAME_WIDTH),
		camera.get(CV_CAP_PROP_FRAME_HEIGHT)); // get size from source
	
	// initialize MotionDetector and Saver objects
	camera.read(frame); 
	MotionDetector detector(frame);
	Saver saver(param.PRESAVE, param.ROOTPATH, param.FILENAME, CODECS, param.FPS, sz, param.sendNotif);
	saver.pushFrame(frame);


	// main loop
	while(camera.read(frame)){
		saver.pushFrame(frame);
		
		if(detector.isMotion(frame))
			motionCount < param.maxMotion ? motionCount++ : motionCount = param.maxMotion;
		else
			motionCount > 0 ? motionCount-- : motionCount = 0;
		
		if(motionCount == param.maxMotion){
			saver.save_start();
			afterCount = param.POSTSAVE;
		}
			
		if(motionCount == 0){
			if(afterCount <= 0)
				saver.save_stop();
			else
				afterCount--;
		}
				
		
		//imshow("Video" , frame);
		//imshow("THERSHOLD", detector.thersh);
		
		if(waitKey(1) == 27){
			//exit on ESC
			break;
		}
		
	}
	saver.save_stop();
}
