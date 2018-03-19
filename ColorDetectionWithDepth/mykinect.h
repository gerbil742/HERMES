#pragma once
#ifndef MYKINECT_H
#define MYKINECT_H
#include <iostream>
#include <Kinect.h>
#include <Windows.h>
#include <gl\GL.h>
#include <gl\GLU.h>
#include <glut.h>
#include <vector>
#define DEPTHWIDTH 512
#define DEPTHHEIGHT 424
#define COLORWIDTH 1920
#define COLORHEIGHT 1080

class MyKinect
{

public:
	MyKinect();
	~MyKinect();
	bool getKinectData();
	void createDisplayFrameArray(int, int);
	bool createDisplayFrameArray();
	BYTE* displayPixelData;
	BYTE* colorPixelData;
	int getColorWidth() { return colorWidth; }
	int getColorHeight() { return colorHeight; }
	int getColorDataSize() { return colorDataSize; }
	bool isInitialized() { return initialized; }
	USHORT* depthFrameDepthData;
	USHORT* depthFrameData;// Raw data of the entire frame. Needed for the coord mapper
	int maxDepth;
	int minDepth;
	int depthWidth;
	int depthHeight;
	int colorWidth;
	int colorHeight;
	USHORT getDepthAtCertianPixel(float x, float y);
	std::vector<float> depthsOfCenter;
	void MyKinect::saveDepths(std::pair<int, int> point);
	float MyKinect::getLatestDepth();
	int depthsIndex;

private:
	IKinectSensor* sensor;
	IMultiSourceFrameReader* reader;
	ICoordinateMapper* mapper;
	CameraSpacePoint* xyzDataForDepthPixels;
	ColorSpacePoint* rgbDataForDepthPixels;
	bool getDepthData(IMultiSourceFrame*);
	bool getColorData(IMultiSourceFrame*);
	void initializeKinect();
	
	UINT depthDataSize;
	
	UINT colorDataSize;
	bool initialized;
	UINT depthFrameDataSize;

	// for saving each depth value in the correct index
	
};


#endif // !KINECT_H
