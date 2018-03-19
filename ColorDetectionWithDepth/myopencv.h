#pragma once
#ifndef MYOPENCV_H
#define MYOPENCV_H
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <vector>
#include <Windows.h>
#include <iostream>
#include <glut.h>
#include <string>

using namespace cv;

class MyOpenCV
{
public:
	MyOpenCV(int*, int*);
	void MyOpenCV::createHSVMatFromRGBAArray(GLubyte* RGBAArray, int width, int height);
	void createFinalImageToDisplay();
	void displayData(bool, bool, int, int);
	int* depthLowerBound;
	int* depthUpperBound;
	void createDepthTrackbars();
	void createHSVTrackbars();
	Point ballLatestPixelLocations[10];
	int index = 0;
	int iLastX;
	int iLastY;
	std::vector <std::pair<int, int>> latestCentersOfBall;
	std::pair<int, int> MyOpenCV::getLatestCenter();

private:
	Mat imgThreshold;
	int hLowerBound, hUpperBound;
	int sLowerBound, sUpperBound;
	int vLowerBound, vUpperBound;
	Mat rgbImage;
	Mat hsvImage;
	std::vector<Mat> trackingLineImages;
	void createTrackingLineImage();
	void createThresholdImage();
	void addToBallLatestPixelLocations(Point pnt);
	void MyOpenCV::calculateMoments();
	void MyOpenCV::saveCenter(int, int);
	// Holds the index for which index to use in the centerpoits vector;
	int nextIndex;
};

#endif // !OPENCV_H
