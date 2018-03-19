//
//#include <Windows.h>
//#include <Ole2.h>
//
//#include <gl/GL.h>
//#include <gl/GLU.h>
//#include <glut.h>
//
//#include <Kinect.h>
//
//#include "mykinect.h"
//
//
//const int width = 512;
//const int height = 424;
//
//// OpenGL Variables
//GLuint textureId;              // ID of the texture to contain Kinect RGB Data
//GLubyte data[width*height * 4];  // BGRA array containing the texture data
//
//
//								  //Kinect variables
////IKinectSensor* sensor;         // Kinect sensor
////IColorFrameReader* reader;     // Kinect color data source
//
//
//MyKinect kinect2;
//
//IKinectSensor* sensor;
//IMultiSourceFrameReader* reader;
//ICoordinateMapper* mapper;
//int depthWidth;
//int depthHeight;
//UINT depthDataSize;
//int colorWidth;
//int colorHeight;
//UINT colorDataSize;
//bool initialized;
//GLubyte* displayPixelData;;
//GLubyte* colorPixelData;
//
//void drawKinectData() {
//	glBindTexture(GL_TEXTURE_2D, textureId);
//	
//	 //populates displayPixelData
//	bool dataRead = kinect2.getKinectData();
//	
//	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_BGRA_EXT, GL_UNSIGNED_BYTE, (GLvoid*)(kinect2.displayPixelData));
//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//	glBegin(GL_QUADS);
//	glTexCoord2f(0.0f, 0.0f);
//	glVertex3f(0, 0, 0);
//	glTexCoord2f(1.0f, 0.0f);
//	glVertex3f(width, 0, 0);
//	glTexCoord2f(1.0f, 1.0f);
//	glVertex3f(width, height, 0.0f);
//	glTexCoord2f(0.0f, 1.0f);
//	glVertex3f(0, height, 0.0f);
//	glEnd();
//}
//
//void draw() {
//	drawKinectData();
//	glutSwapBuffers();
//}
//
//bool init(int argc, char* argv[]) {
//	glutInit(&argc, argv);
//	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
//	glutInitWindowSize(width, height);
//	glutCreateWindow("Kinect SDK Tutorial");
//	glutDisplayFunc(draw);
//	glutIdleFunc(draw);
//	return true;
//}
//
//bool initKinect() {
//	HRESULT hr = GetDefaultKinectSensor(&sensor);
//	if (FAILED(hr))
//	{
//		std::cout << "Failed GetDefaultKinectSensor\n";
//		return false;
//	}
//
//	if (sensor)
//	{
//		hr = sensor->Open();
//		if (SUCCEEDED(hr))
//		{
//			hr = sensor->get_CoordinateMapper(&mapper);
//			if (SUCCEEDED(hr))
//			{
//				hr = sensor->OpenMultiSourceFrameReader(
//					FrameSourceTypes::FrameSourceTypes_Depth |
//					FrameSourceTypes::FrameSourceTypes_Color,
//					&reader);
//				if (SUCCEEDED(hr))
//				{
//					std::cout << "sensor initialized\n";
//					return true;
//				}
//			}
//
//		}
//	}
//	return false;
//	/*
//	if (FAILED(GetDefaultKinectSensor(&sensor))) {
//		return false;
//	}
//	if (sensor) {
//		sensor->Open();
//		IColorFrameSource* framesource = NULL;
//		sensor->get_ColorFrameSource(&framesource);
//		framesource->OpenReader(&reader);
//		if (framesource) {
//			framesource->Release();
//			framesource = NULL;
//		}
//		return true;
//	}
//	else {
//		return false;
//	} */
//}
//
////void getKinectData(GLubyte* dest) {
////	
////	IColorFrame* colorFrame = NULL;
////	IColorFrameReference* frameRef = NULL;
////	IFrameDescription* colorFrameDescription = NULL;
////	IMultiSourceFrame* multiFrame = NULL;
////
////	HRESULT hr = reader->AcquireLatestFrame(&multiFrame);
////	if (SUCCEEDED(hr))
////	{
////		HRESULT hr = multiFrame->get_ColorFrameReference(&frameRef);
////		if (SUCCEEDED(hr))
////		{
////			hr = frameRef->AcquireFrame(&colorFrame);
////
////			if (frameRef)
////				frameRef->Release();
////			if (!colorFrame)
////			{
////				std::cout << "colorFrame not captured" << std::endl;
////				return;
////			}
////
////			if (SUCCEEDED(hr))
////			{
////				hr = colorFrame->get_FrameDescription(&colorFrameDescription);
////				if (SUCCEEDED(hr))
////				{
////					colorFrameDescription->get_LengthInPixels(&colorDataSize);
////					colorFrameDescription->get_Height(&colorHeight);
////					colorFrameDescription->get_Width(&colorWidth);
////				}
////
////				 //fills the array with rgb pixel data to be used in display
////				colorFrame->CopyConvertedFrameDataToArray(
////					width * height * 4,
////					dest,
////					ColorImageFormat_Bgra);
////				std::cout << "ColorFrame success\n";
////			}
////		}
////	}
////
////
////	if (colorFrame) colorFrame->Release();
////	if (multiFrame) multiFrame->Release();
////	
////	IColorFrame* frame = NULL;
////	kinect2.getKinectData(0, 5000);
////	if (SUCCEEDED(reader->AcquireLatestFrame(&frame))) {
////		frame->CopyConvertedFrameDataToArray(width*height * 4, , ColorImageFormat_Bgra);
////	}
////	if (frame) frame->Release();
////}
//
//
//
//
//
//void execute() {
//	glutMainLoop();
//}
//
//int main(int argc, char* argv[]) {
//	if (!init(argc, argv)) return 1;
//	//if (!initKinect()) return 1;
//
//	// Initialize textures
//	glGenTextures(1, &textureId);
//	glBindTexture(GL_TEXTURE_2D, textureId);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, (GLvoid*)(kinect2.displayPixelData));
//	glBindTexture(GL_TEXTURE_2D, 0);
//
//	// OpenGL setup
//	glClearColor(0, 0, 0, 0);
//	glClearDepth(1.0f);
//	glEnable(GL_TEXTURE_2D);
//
//	// Camera setup
//	glViewport(0, 0, width, height);
//	glMatrixMode(GL_PROJECTION);
//	glLoadIdentity();
//	glOrtho(0, width, height, 0, 1, -1);
//	glMatrixMode(GL_MODELVIEW);
//	glLoadIdentity();
//
//	// Main loop
//	execute();
//	return 0;
//}
