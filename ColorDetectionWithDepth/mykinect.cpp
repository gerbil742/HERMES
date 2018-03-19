#include <Windows.h>
#include "mykinect.h"
#include <Exception>


MyKinect::MyKinect()
{
	initializeKinect();
	
	//colorPixelData = new GLubyte[COLORWIDTH * COLORHEIGHT * 4]; // Will be an RGBA array
	//displayPixelData = new GLubyte[512 * 424 * 4];
	xyzDataForDepthPixels = new CameraSpacePoint[512 * 424];
	rgbDataForDepthPixels = new ColorSpacePoint[512 * 424];
	//depthFrameDepthData = new unsigned short[DEPTHWIDTH * DEPTHHEIGHT];
	//depthFrameData = new UINT16[DEPTHHEIGHT * DEPTHWIDTH];
	depthFrameDepthData = new USHORT[DEPTHWIDTH * DEPTHHEIGHT];
	displayPixelData = new BYTE[512 * 424 * 4];
	/*for (int i = 0; i < (512 * 424 * 4); i++)
	{
		displayPixelData[i] = 0;
	}*/
	colorPixelData = new BYTE[1920 * 1080 * 4];
	depthFrameData = new USHORT[512 * 424];
	
	for(int i = 0; i < 20; i++)
		depthsOfCenter.push_back(0);

	minDepth = 850;
	maxDepth = 2350;

	depthsIndex = 0;
}

MyKinect::~MyKinect()
{
	delete[] colorPixelData;
	delete[] depthFrameDepthData;
	delete[] xyzDataForDepthPixels;
	delete[] displayPixelData;
	delete[] depthFrameData;
	delete[] rgbDataForDepthPixels;
	if (sensor)
		sensor->Close();
}

/*
so I want to map colorframetocameraspace because that will turn the rgb array into an array of xyz values 
once i have the xyz values of each pixel, i can change change the rgb values to fade to black or fade to white
*/

bool MyKinect::getDepthData(IMultiSourceFrame* multiFrame)
{
	IDepthFrame* depthFrame = NULL;
	IDepthFrameReference* frameRef = NULL;
	IFrameDescription* depthFrameDescription = NULL;
	bool success = false;
	HRESULT hr = multiFrame->get_DepthFrameReference(&frameRef);
	if (SUCCEEDED(hr))
	{
		hr = frameRef->AcquireFrame(&depthFrame);
		if (SUCCEEDED(hr))
		{
			if (!depthFrame)
				return false;
			if (frameRef)
				frameRef->Release();

			//depthFrame->get_DepthMaxReliableDistance(&maxDepth);
			//depthFrame->get_DepthMinReliableDistance(&minDepth);
			hr = depthFrame->get_FrameDescription(&depthFrameDescription);
			if (SUCCEEDED(hr))
			{
				depthFrameDescription->get_LengthInPixels(&depthFrameDataSize);
				depthFrameDescription->get_Height(&depthHeight);
				depthFrameDescription->get_Width(&depthWidth);
				

				// This populates the depthframeData variable for the class
				// will be used to map the regular camera pixles to their corresponding xyz coords
				hr = depthFrame->AccessUnderlyingBuffer(&depthDataSize, &depthFrameDepthData);
				//std::cout << getDepthAtCertianPixel(0, 0) << "\n";

				if (!(SUCCEEDED(hr)))
				{
					//std::cout << "fail access buffer" << std::endl;
					return false;
				}
				hr = depthFrame->CopyFrameDataToArray(depthWidth* depthHeight, depthFrameData);

				if (!(SUCCEEDED(hr)))
				{
					//std::cout << "fail frame data" << std::endl;
					return false;
				}
				success = true;
				//std::cout << "depthFrame success\n";
			}
		}
	}

	if (depthFrame) depthFrame->Release();
	
	return success;
	/*
	const unsigned short* curr = (const unsigned short*)buf;
	const unsigned short* dataEnd = curr + (DEPTHWIDTH*DEPTHHEIGHT);

	while (curr < dataEnd) 
	{
		// Get depth in millimeters
		unsigned short depth = (*curr++);

		//// Chanaging the background to be black if the pixles 
		//// are not in the spwcified range
		//if (depth < minDepth || depth > maxDepth)
		//{
		//	for (int i = 0; i < 3; i++)
		//	{
		//		*dest++ = 0;
		//	}
		//	*dest++ = 0xff;
		//}

		//else
		//{
		
		// Draw a grayscale image of the depth:
		// B,G,R are all set to depth%256, alpha set to 1.
		for (int i = 0; i < 3; ++i)
			*dest++ = (BYTE)depth % 256;
		*dest++ = 0xff;
		//}
	}*/
}
bool MyKinect::getColorData(IMultiSourceFrame* multiFrame)
{
	IColorFrame* colorFrame = NULL;
	IColorFrameReference* frameRef = NULL;
	IFrameDescription* colorFrameDescription = NULL;

	HRESULT hr = multiFrame->get_ColorFrameReference(&frameRef);
	if (SUCCEEDED(hr))
	{
		hr = frameRef->AcquireFrame(&colorFrame);
		
		if (frameRef)
			frameRef->Release();
		if (!colorFrame)
		{
			//std::cout << "colorFrame not captured" << std::endl;
			return false;
		}

		if (SUCCEEDED(hr))
		{
			hr = colorFrame->get_FrameDescription(&colorFrameDescription);
			if (SUCCEEDED(hr))
			{
				colorFrameDescription->get_LengthInPixels(&colorDataSize);
				colorFrameDescription->get_Height(&colorHeight);
				colorFrameDescription->get_Width(&colorWidth);

				// fills the array with rgb pixel data to be used in display
				if (SUCCEEDED(hr))
				{
					hr = colorFrame->CopyConvertedFrameDataToArray(
						colorWidth * colorHeight * 4,
						colorPixelData,
						ColorImageFormat::ColorImageFormat_Bgra);

					
					if (SUCCEEDED(hr))
					{
						//std::cout << "ColorFrame success\n";
						if (colorFrame) colorFrame->Release();
						return true;
					}
					std::cout << "fail" << std::endl;
				}
			}
		}
	} 
	
	if (colorFrame) colorFrame->Release();
	return false;
}

bool MyKinect::getKinectData()
{
	IMultiSourceFrame* frame = NULL;

	HRESULT hr = reader->AcquireLatestFrame(&frame);
	if (SUCCEEDED(hr))
	{	
		//std::cout << "multisourceFrame grabbed" << std::endl;
		bool depthSuccess = getDepthData(frame);
		bool colorSuccess = getColorData(frame);
		
		if (depthSuccess && colorSuccess)
		{
			// This fills the rgb DataFor.each pixel in a depth frame
			//std::cout << "got depth data and color data\n";
			hr = mapper->MapDepthFrameToColorSpace(
				depthDataSize,
				depthFrameData,
				depthDataSize,
				rgbDataForDepthPixels);
			if (SUCCEEDED(hr))
			{
				//std::cout << "Coords mapped depth to color\n";
				hr = mapper->MapDepthFrameToCameraSpace(
					depthDataSize,
					depthFrameData,
					depthDataSize,
					xyzDataForDepthPixels);
				
				// Creates the frame without any pixles outside of the specified depth range (initially the max/minReliable depths)
				
				/*if (frame)
					frame->Release();
				return true;*/
				if (SUCCEEDED(hr))
				{
					//std::cout << "Coords mapped depth to camera\n";
					bool frameProcessed = createDisplayFrameArray();

					if (frameProcessed)
					{
						//std::cout << "Frame Processed\n";
						if (frame)
							frame->Release();
						return true;
					}
					return true;
				}
			}
		}
	}

	if (frame)
		frame->Release();
	return false;
}

// Modifies the depthsOfCenter vector with the latestCentersOfBall pair that was just updated in MyOpenCV
void MyKinect::saveDepths(std::pair<int, int> point)
{
	int linearPixelIndex = point.first + (point.second * 512); // tricky this is a point rows are the y value which is the second value
	if (linearPixelIndex > 512 * 420 || linearPixelIndex < 0)
		return;

	CameraSpacePoint pnt = xyzDataForDepthPixels[linearPixelIndex];
	float depth = pnt.Z;
	//std::cout << depth << std::endl;
	int index = linearPixelIndex;
	if (depth > float(minDepth)/1000 && depth < float(maxDepth)/1000) // in range
	{
		depthsOfCenter[depthsIndex] = depth;
	}
	else
		return;
	
	depthsIndex++;
	depthsIndex = depthsIndex % 20;

	/*for (int i = 0; i < 20; i++)
	{
		std::cout << depthsOfCenter[i] << " ";
		

	}
	std::cout << std::endl;*/
	//std::cout << depthsIndex << std::endl;
	/*for each (float x in depthsOfCenter)
	//{
	//	std::cout << x << std::endl;
	//}*/
	//std::cout << std::endl;
}

// gets the last added depth to the depthOfCenter vector
float MyKinect::getLatestDepth()
{
	int index = 0;
	if (depthsIndex > 0)
	{
		index = depthsIndex - 1;
	}
	return depthsOfCenter[index];
}

bool MyKinect::createDisplayFrameArray()
{
	//const USHORT* curr = (const USHORT*)depthFrameDepthData;
	//const USHORT* dataEnd = curr + depthWidth * depthHeight;

	//while (curr < dataEnd) {
	//	// Get depth in millimeters
	//	USHORT depth = *curr;
	//	curr++;

	//	int index = 0;

	//	// Draw a grayscale image of the depth:
	//	// B,G,R are all set to depth%256, alpha set to 1.
	//	for (int i = 0; i < 3; ++i)
	//	{
	//		displayPixelData[index++] = (BYTE)depth % 256;
	//	}
	//	displayPixelData[index++] = 0xff;
//}

	////////int displayPixelIndex = 0;// use for stepping through depth

	////////for (int i = 0; i < colorWidth * colorHeight * 4; )//++i)
	////////{
	////////	//USHORT depth = depthFrameData[i];
	////////	displayPixelData[i++] = colorPixelData[i]; // for depth set = to depth / 256;
	////////	displayPixelData[i++] = colorPixelData[i];
	////////	displayPixelData[i++] = colorPixelData[i];
	////////	displayPixelData[i++] = 255;
	////////}
	try
	{
		int pixelIndex = 0;
		for (int i = 0; i < depthWidth * depthHeight; ++i)
		{
			ColorSpacePoint p = rgbDataForDepthPixels[i];
			CameraSpacePoint c = xyzDataForDepthPixels[i];
			//If the depth is out of user specified range then make the pixles black
			if (p.X < 0 || p.X > colorWidth || // X and Y are pixel indexes (0-1920, 0-1080) but they are decimals because teh conversion is not perfect
				p.Y < 0 || p.Y > colorHeight ||
				c.Z < float(minDepth) / 1000 || c.Z > float(maxDepth) / 1000) // float(minDepth) / 1000)
			{
				//if (pixelIndex > 0)
				//{
				//	int previousIndex = pixelIndex - 4;
				//	displayPixelData[pixelIndex++] = displayPixelData[previousIndex++]; // The B value
				//	displayPixelData[pixelIndex++] = displayPixelData[previousIndex++]; // The G value
				//	displayPixelData[pixelIndex++] = displayPixelData[previousIndex++]; // The R value
				//	displayPixelData[pixelIndex++] = displayPixelData[previousIndex++]; // A value
				//
				//}
				//else
				//{
					displayPixelData[pixelIndex++] = 0; // The B value
					displayPixelData[pixelIndex++] = 0; // The G value
					displayPixelData[pixelIndex++] = 0; // The R value
					displayPixelData[pixelIndex++] = 0; // A value
				//}
			}
			else
			{
				int colorPixelIndex = 4 * ( (int)(p.X) + ((int)(p.Y)  * colorWidth));
				displayPixelData[pixelIndex++] = colorPixelData[colorPixelIndex++]; // The B value
				displayPixelData[pixelIndex++] = colorPixelData[colorPixelIndex++]; // The G value
				displayPixelData[pixelIndex++] = colorPixelData[colorPixelIndex++]; // The R value
				displayPixelData[pixelIndex++] = colorPixelData[colorPixelIndex++]; // A value
			
				//int xIndex = (int)(p.X);
				//int yIndex = (int)(p.Y);
				//int pixelXIndex = ((float)(xIndex) / (float)(colorWidth)) * depthWidth;
				//int pixelYIndex = ((float)(yIndex) / (float)(colorHeight)) * depthHeight;

				//int colorPixelIndex = 4 * (xIndex + (yIndex * colorWidth));
				//pixelIndex = 4 * (pixelXIndex + (pixelYIndex * depthWidth));

				//displayPixelData[pixelIndex++] = colorPixelData[colorPixelIndex++]; // The B value
				//displayPixelData[pixelIndex++] = colorPixelData[colorPixelIndex++]; // The G value
				//displayPixelData[pixelIndex++] = colorPixelData[colorPixelIndex++]; // The R value
				//displayPixelData[pixelIndex++] = colorPixelData[colorPixelIndex++]; // A value

			}
		}
		
		//std::cout << displayPixelData
		return true;
	}
	catch (std::exception ex)
	{
		//std::cout << "Frame processing failed\n";
		return false;
	}
}
//
//void MyKinect::createDisplayFrameArray(int lowerBound, int upperBound)
//{
//	for (int i = 0; i < colorWidth * colorHeight; i++)
//	{
//		// If the depth is out of user specified range then make the pixles black
//		//if (xyzDataForColorPixels && (xyzDataForColorPixels[i].Z < lowerBound || xyzDataForColorPixels[i].Z > upperBound))
//		//{
//		//	displayPixelData[i * 4 + 0] = 0; // The R value
//		//	displayPixelData[i * 4 + 1] = 0; // The G value
//		//	displayPixelData[i * 4 + 2] = 0; // The B value
//		//	displayPixelData[i * 4 + 3] = 0; // A value
//		//}
//		//else
//		//{
//			displayPixelData[i * 4 + 0] = colorPixelData[i * 4 + 0]; // The R value
//			displayPixelData[i * 4 + 1] = colorPixelData[i * 4 + 1]; // The G value
//			displayPixelData[i * 4 + 2] = colorPixelData[i * 4 + 2]; // The B value
//			displayPixelData[i * 4 + 3] = colorPixelData[i * 4 + 3]; // A value
//		//}
//
//		//std::cout << (int)displayPixelData[i] << " " << i <<"\n";
//	}
//}


void MyKinect::initializeKinect() 
{
	HRESULT hr = GetDefaultKinectSensor(&sensor);
	if (FAILED(hr))
	{
		//std::cout << "Failed GetDefaultKinectSensor\n";
		initialized = false;
		return;
	}

	if (sensor)
	{
		hr = sensor->Open();
		if (SUCCEEDED(hr))
		{
			hr = sensor->get_CoordinateMapper(&mapper);
			if (SUCCEEDED(hr))
			{
				hr = sensor->OpenMultiSourceFrameReader(
					    FrameSourceTypes::FrameSourceTypes_Depth |
					    FrameSourceTypes::FrameSourceTypes_Color,
					    &reader);
				if (SUCCEEDED(hr))
				{
					//std::cout << "sensor initialized\n";
					initialized = true;
					return;
				}
			}
		}
	}
	initialized = false;
}

USHORT MyKinect::getDepthAtCertianPixel(float x, float y)
{
	if (x < 0 || x > 424 || y < 0 || y > 512)
	{
		//std::cout << "invalid point: " << x << " " << y << "\n";
		return 0;
	}

	int index = x + y; // each row if 512 pixels long. there are 424 rows
	return depthFrameDepthData[0];
}


















/*


#include <Kinect.h>

#include <Ole2.h>
#include <iostream>
#include <gl/gl.h>
#include <gl/GLU.h>
#include <glut.h>



#define width 512
#define height 480

#define colorWidth 1920
#define colorHeight 1080

//using namespace std;

// OpenGL Variables
GLuint textureId;              // ID of the texture to contain Kinect RGB Data
GLubyte data[width*height * 4];  // BGRA array containing the texture data

								 // Kinect variables
IKinectSensor* sensor;         // Kinect sensor
IDepthFrameReader* reader;     // Kinect color data source

short maxDepth = MAXSHORT, minDepth = 0;

bool initKinect() {
	HRESULT hr = GetDefaultKinectSensor(&sensor);
	if (FAILED(hr)) {
		return false;
	}
	if (sensor) {
		sensor->Open();

		IDepthFrameSource* framesource = NULL;
		sensor->get_DepthFrameSource(&framesource);
		framesource->OpenReader(&reader);
		if (framesource) {
			framesource->Release();
			framesource = NULL;
		}
		return true;
	}
	else {
		return false;
	}
}

void getKinectData(GLubyte* dest) 
{
	IDepthFrame* frame = NULL;
	if (SUCCEEDED(reader->AcquireLatestFrame(&frame))) 
	{
		unsigned int sz;
		unsigned short* buf; // unsigned short is 2 bytes
		frame->AccessUnderlyingBuffer(&sz, &buf);

		const unsigned short* curr = (const unsigned short*)buf;
		const unsigned short* dataEnd = curr + (width*height);

		while (curr < dataEnd) 
		{
			// Get depth in millimeters
			unsigned short depth = (*curr++);

			// Chanaging the background to be black if the pixles 
			// are not in the spwcified range
			if (depth < minDepth || depth > maxDepth)
			{
				for (int i = 0; i < 3; i++)
				{
					*dest++ = 0;
				}
				*dest++ = 0xff;
			}

			else
			{
				// Draw a grayscale image of the depth:
				// B,G,R are all set to depth%256, alpha set to 1.
				for (int i = 0; i < 3; ++i)
					*dest++ = (BYTE)depth % 256;
				*dest++ = 0xff;
			}
		}
	}
	if (frame) frame->Release();
}

void drawKinectData() {
	glBindTexture(GL_TEXTURE_2D, textureId);
	getKinectData(data);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_BGRA_EXT, GL_UNSIGNED_BYTE, (GLvoid*)data);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0, 0, 0);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(width, 0, 0);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(width, height, 0.0f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0, height, 0.0f);
	glEnd();
}

void draw() {
	drawKinectData();
	glutSwapBuffers();
}

void execute() {
	glutMainLoop();
}

bool init(int argc, char* argv[]) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(width, height);
	glutCreateWindow("Kinect SDK Tutorial");
	glutDisplayFunc(draw);
	glutIdleFunc(draw);
	return true;
}

// Functions to handle keypresses below
void keyPressed(unsigned char key, int x, int y)
{
	switch (key)
	{
	case '1': { // extend max depth
		maxDepth = maxDepth == MAXSHORT ? 0 : maxDepth;
		if (maxDepth <= (MAXSHORT - 100))
			maxDepth += 100;
		else
			maxDepth = MAXSHORT;
		std::cout << "Max Depth is " << maxDepth << std::endl;
		break;
	}

	case '2': { // decrease max depth
		maxDepth = maxDepth == MAXSHORT ? 0 : maxDepth;
		if (maxDepth >= 100)
			maxDepth -= 100;
		else
			maxDepth = 0;
		std::cout << "Max Depth is " << maxDepth << std::endl;
		break;
	}

	case '3': { // inscrease min depth
		if (minDepth <= (MAXSHORT - 100))
			minDepth += 100;
		else
			minDepth = MAXSHORT;
		std::cout << "Min Depth is " << minDepth << std::endl;
		break;
	}

	case '4': { // decrease min depth
		if (minDepth >= 100)
			minDepth -= 100;
		else
			minDepth = 0;
		std::cout << "Min Depth is " << minDepth << std::endl;
		break;
	}

	case '0': { // reset values
		minDepth = 0;
		maxDepth = MAXSHORT;
		std::cout << "Reset\n";
	}
	}
}

void initGlut()
{
	// Initialize textures
	glGenTextures(1, &textureId);
	glBindTexture(GL_TEXTURE_2D, textureId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height,
		0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, (GLvoid*)data);
	glBindTexture(GL_TEXTURE_2D, 0);

	// OpenGL setup
	glClearColor(0, 0, 0, 0);
	glClearDepth(1.0f);
	glEnable(GL_TEXTURE_2D);

	// Camera setup
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, width, height, 0, 1, -1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Keyboard stuff
	glutKeyboardFunc(keyPressed);
}

// Starting the OpenCV color detection processing


*/