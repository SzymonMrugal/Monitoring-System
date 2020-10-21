#include <opencv2/opencv.hpp>
#include <opencv2/tracking.hpp>
#include <opencv2/core/ocl.hpp>
#include <unistd.h>
#include <queue>
#include "getDate.cpp"

using namespace std;
using namespace cv;

class Saver{
	
	private:
		
		queue<Mat> lastFrames;
		int MAXFRAMES; //how many frames to hold
		string OUTPUT_ROOT; //saving destination path
		string OUTPUT_FILE; //saving file name
		int sendNotif; //determine whether to send email
		int CODECS;
		int FPS;
		Size SZ;  //frame size
		bool newfile;
		bool save_flag;
		VideoWriter writer;
		
	public:

		Saver(int maxframes, string outputroot, string outputfile, int codecs, int fps, Size sz, int sendNotif=0):
			MAXFRAMES(maxframes),
			OUTPUT_ROOT(outputroot),
			OUTPUT_FILE(outputfile),
			CODECS(codecs),
			FPS(fps),
			SZ(sz),
			sendNotif(sendNotif)
			{newfile=true;}
		
		void pushFrame(Mat frame){  // push next frame to container
			if (lastFrames.size() > MAXFRAMES){
				lastFrames.pop();
				lastFrames.push(frame.clone());
			}else{
				lastFrames.push(frame.clone());
			}
			
			if(save_flag){
				writer.write(frame);
			}
		}
		
		void save_start(string SNAPSHOTNAME = "snapshot.jpg"){ // initialize saving procedure
			save_flag = true;
			if(newfile){
				if(sendNotif)
					SendEmail(SNAPSHOTNAME);
				writer.open(OUTPUT_ROOT+getDateStr()+OUTPUT_FILE, CODECS, FPS, SZ);
				queue<Mat> temp = lastFrames;
				while(!temp.empty()){
					writer.write(temp.front());
					temp.pop();
				}
				newfile = false;
			}
		}
		
		void save_stop(){  //stop saving procedure
			writer.release();
			newfile = true;
			save_flag = false;
		}
		
		void SendEmail(string SNAPSHOTNAME){
			imwrite(SNAPSHOTNAME, lastFrames.back());
			string command = "./SendEmail ";
			command.append(SNAPSHOTNAME);
			system(command.c_str());
		}
	
};
