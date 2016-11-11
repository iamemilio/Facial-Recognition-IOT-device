/*
Created by Emilio Garcia
code sourced from lab 5 and 9.
*/

#include "opencv2/objdetect.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/core/core.hpp"

#include <iostream>
#include <stdio.h>
#include <windows.h>
#include <string>


using namespace std;
using namespace cv;


/** Global variables */
String face_cascade_name = "haarcascade_frontalface_alt.xml";
CascadeClassifier face_cascade;
bool DEBUG = true;  //will reduce frame rates and performance metrics due to increased pixel operations and conditional checks!


//	Method Declarations

//simple program for capturing samples
//user sits at computer and presses space to capture and write a frame to the projects working directory
static void sampleCapture(string fileName);

//main driver for facial recognition algorithm.
static void facialRecognition();

//searches for a face in the current frame using Haar Cascades
//returns a vecotr of rectangular coordinates of the detected faces
static void detectFaces(Mat& frame, vector<Rect>& facePositions);



/** @function main */
int main(void)
{
	//-- 1. Load the cascades
	if (!face_cascade.load(face_cascade_name)){ printf("--(!)Error loading face cascade\n"); return -1; };
	//if (!eyes_cascade.load(eyes_cascade_name)){ printf("--(!)Error loading eyes cascade\n"); return -1; };
	
	for (;;) {
		string input = "";
		string debug = "";
		printf("Do you want to collect samples,run the facial recognition, or quit?\n[samples | detection | quit]: ");
		getline(cin, input);
		printf("\n");

		if (input.compare("samples") == 0){
			printf("Enter the fileName you want to save your samples as.\n");
			string path = "";
			getline(cin, path);
			sampleCapture(path);
		}
		else if (input.compare("quit") == 0){
			return 0;
		}
		else if (input.compare("detection") == 0){
			printf("Turn on Debug Mode? Disclaimer: this will affect any speed metrics adversely!\nWhen debug is on, it will show visuals at different stages of the algorithm.\n ( y | n | Default = y): ");
			getline(cin, debug);
			printf("\n");

			if (debug.compare("n") == 0) DEBUG = false;
			else DEBUG = true;
			facialRecognition();
		}
	}	
}


static void sampleCapture(string fileName){
	printf("Press space bar to capture an image, and press the escape key to quit out.\n");
	VideoCapture cap(0);
	if (!cap.isOpened())
		return;

	Mat frame;

	for (int i = 0;; i++){
		cap >> frame;

		imshow("Sample Capturing", frame);

		if (waitKey(30) == 32){
			String path = fileName + std::to_string(i) + ".png";

			vector<int> compression_params;
			compression_params.push_back(CV_IMWRITE_PNG_COMPRESSION);
			compression_params.push_back(9);

			printf("Image Saved: %s\n", path);
			imwrite(path, frame, compression_params);
		}
		else if (waitKey(30) == 27){
			destroyWindow("Sample Capturing");
			return;
		}
	}
}


static void facialRecognition(){
	//initiate video capture
	VideoCapture cap(0);
	if (!cap.isOpened()) return;

	Mat frame, boundedFaces, face, frame_gray, eigenface;
	std::vector<Rect> faces;
	Rect largestFace;
	
	//event loop
	while (true){
		cap >> frame;
		if (frame.empty()) break;

		/** 
		-----------------------------------------------------
		Face Detection
		-----------------------------------------------------
		*/

		cvtColor(frame, frame_gray, CV_BGR2GRAY);
		equalizeHist(frame_gray, frame_gray);

		//coppies equalized frame into soon to be eigenface
		frame_gray.copyTo(eigenface);

		//adding a gaussian Blur Drastically reduces the number of false positives --> not optimized
		GaussianBlur(frame_gray, frame_gray, Size(3, 3), 3);

		//-- Detect faces
		//uses a lower accuracy in order to achieve faster response time
		face_cascade.detectMultiScale(frame_gray, faces, 1.2, 2, 0 | CASCADE_SCALE_IMAGE, Size(30, 30));

		//find faces in the current frame
		//detectFaces(frame_gray, faces);

		//takes the largest face
		if (faces.size() > 0){
			largestFace = faces[0];

			if (DEBUG){
				frame.copyTo(boundedFaces); //coppies frame so its not modified
				rectangle(boundedFaces, faces[0], Scalar(255, 0, 0), 2);
				imshow("faces", boundedFaces);
				imshow("gray", frame_gray);
				//will exit on 'esc'
				if (waitKey(30) == 27){
					destroyWindow("faces");
					destroyWindow("gray");
					return;
				}
			}
			/**
			-----------------------------------------------------
			Face Recognition
			-----------------------------------------------------
			*/

			//takes submatrix of the normalized frame to isolate only the face
			eigenface = eigenface(largestFace);  


		}
		else if (DEBUG && faces.size() == 0){
			imshow("faces", frame);
			imshow("gray", frame_gray);
			//will exit on 'esc'
			if (waitKey(30) == 27){
				destroyWindow("faces");
				destroyWindow("gray");
				return;
			}
		}
	}
}

/** @function detectAndDisplay
Overwrites facePositions!!!
*/

//notes!!  the face detection seems to be bounded to 45 degrees. At the outer extremities of its range, ~40degrees, it starts to detect false positives!! --> depending on how fast the recogintion
//algirithm is, this could be a major slowdown

//also, this algoirithm works better at a ~ 1-8 meter distance.

//fairly reliable with noise, and low light
void detectFaces(Mat& frame_gray, vector<Rect>& facePositions)
{
	
	
}
