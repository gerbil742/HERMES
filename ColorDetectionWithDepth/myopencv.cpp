#include "myopencv.h"

/*
	plan
	Kinect: get multisource data from kinect. map the depth to the color data create the array to display
		array has all pixles where the depth is in bounds

	opencv: take the color image from the kinect and create the mat for it. then convert to hsv, do \
	erosins dialations and at the center of the object draw the line on new mat and add to origional mat
	put back inot origional array for opengl to display. 

	opengl: 
*/

MyOpenCV::MyOpenCV(int* depthLower, int* depthHigher)
{
	iLastX = -1;
	iLastY = -1;

	
	hLowerBound = 73;
	sLowerBound = 56;
	vLowerBound = 249;
	hUpperBound = 100;
	sUpperBound = 215;
	vUpperBound = 255;
	depthLowerBound = depthLower;
	depthUpperBound = depthHigher;
	//typedef Vec<UINT, 3> threeChannelVec;
	//namedWindow("Thresholded Image");

	createDepthTrackbars();
	createHSVTrackbars();
	for (int i = 0; i < 20; i++)
	{
		latestCentersOfBall.push_back(std::pair<int, int>(0, 0));
	}
	//namedWindow("RGB Image")
	int nextIndex = 0;
}

void MyOpenCV::createHSVMatFromRGBAArray(GLubyte* RGBAArray, int width, int height)
{
	//// Fills all 3 channels of the Mat with values from the RGBArray parameter
	//for (int i = 0; i < 424; i++)
	//{
	//	for (int j = 0; j < 512; j++)
	//	{
	//		int index = (i * 512) + j;
	//		rgbImage.at<Vec3b>(i, j)[0] = (UCHAR)(RGBAArray[index]);
	//		index++;
	//		rgbImage.at<Vec3b>(i, j)[1] = (UCHAR)(RGBAArray[index]);
	//		index++;
	//		rgbImage.at<Vec3b>(i, j)[2] = (UCHAR)(RGBAArray[index]);
	//		index++;
	//		index++;
	//		// Ignoring alpha channel
	//	}
	//}

	rgbImage = Mat(cv::Size(width, height), CV_8UC4, (BYTE*)RGBAArray);
	//displayData();
	// Converting to hsv image
	cvtColor(rgbImage, hsvImage, COLOR_RGB2HSV);
	//imshow("rgb", rgbImage);
	//imshow("hsv", hsvImage);
	//createTrackingLineImage();
	calculateMoments();
}

void MyOpenCV::createFinalImageToDisplay()
{
	if (!rgbImage.empty())
	{
		for (int i = 0; i < trackingLineImages.size(); i++)
		{
			rgbImage += trackingLineImages[i];
		}
		//rgbImage += trackingLineImages[0];
	}
	else
	{
	}
		//std::cout << "rgbImage is empty" << std::endl;
}

void MyOpenCV::calculateMoments()
{
	createThresholdImage();
	if (imgThreshold.empty())
		return;
	Moments oMoments = moments(imgThreshold); // how does this work??????

	double dM01 = oMoments.m01;
	double dM10 = oMoments.m10;
	double dArea = oMoments.m00;
	//std::cout << dArea << std::endl;
	if (dArea > 1) // try to omit invalid data modify if capturing too much noise or to little ball
	{
		int posX = dM10 / dArea;
		int posY = dM01 / dArea;
		if (posX >=0 && posY >= 0)
			saveCenter(posX, posY);
	}
}

/*Stores the latest 20 points of the center position of the ball*/
void MyOpenCV::saveCenter(int x, int y)
{
	latestCentersOfBall[nextIndex] = std::pair<int, int>(x, y);
	nextIndex++;
	nextIndex = nextIndex % 20; // if nextIndex = 20 reset back to 0.

	/*for (std::pair<int, int> point : latestCentersOfBall)
	{
		std::cout << "x: " << point.first << " y: " << point.second << std::endl;
	}*/
}

// Returns the x y pixel location pair of the latest depth added to LatestCenters of Ball
std::pair<int, int> MyOpenCV::getLatestCenter()
{
	int index = 0;
	if (nextIndex > 0)
	{
		index = nextIndex - 1;
	}
	return latestCentersOfBall[index];
}

void MyOpenCV::createTrackingLineImage()
{
	Mat imgLines = Mat::zeros(rgbImage.size(), CV_8UC4);
	
	createThresholdImage();

	if (!imgThreshold.empty())
	{
		//std::cout << "creating trackingline image\n";
		//Calculate the moments of the thresholded image
		Moments oMoments = moments(imgThreshold); // how does this work??????

		double dM01 = oMoments.m01;
		double dM10 = oMoments.m10;
		double dArea = oMoments.m00;

		//std::cout << dArea << std::endl;
		// if the area <= 10000, I consider that the there are no object in the image and it's because of the noise, the area is not zero 
		if (dArea > 3000)
		{
			//calculate the position of the ball
			int posX = dM10 / dArea;
			int posY = dM01 / dArea;
			
			saveCenter(posX, posY);

			if (iLastX >= 0 && iLastY >= 0 && posX >= 0 && posY >= 0)
			{
				//Draw a red line from the previous point to the current point
				line(imgLines, Point(posX, posY), Point(iLastX, iLastY), Scalar(0, 0, 255), 2);
				//addToBallLatestPixelLocations(Point(posX, posY));
			}

			iLastX = posX;
			iLastY = posY;

			if (trackingLineImages.size() > 10)
			{
				trackingLineImages.erase(trackingLineImages.begin());
			}
			trackingLineImages.push_back(imgLines);
		}
		//imshow("tracking lines", imgLines);
		createFinalImageToDisplay();
	}
	else
	{
	}
		//std::cout << "imgThreshold is empty" << std::endl;
}

void MyOpenCV::createThresholdImage()
{
	if (!hsvImage.empty())
	{
		//std::cout << "creating threshold image\n";

		inRange(hsvImage, Scalar(hLowerBound, sLowerBound, vLowerBound), Scalar(hUpperBound, sUpperBound, vUpperBound), imgThreshold); //Threshold the image

		//morphological opening (removes small objects from the foreground)
		//erode(imgThreshold, imgThreshold, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
		//dilate(imgThreshold, imgThreshold, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));

		////morphological closing (removes small holes from the foreground)
		//dilate(imgThreshold, imgThreshold, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
		//erode(imgThreshold, imgThreshold, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
		
		//imshow("threshold image", imgThreshold);
	}
	else
	{
	}
		//std::cout << "hsvImage is empty" << std::endl;
}
void MyOpenCV::createDepthTrackbars()
{
	namedWindow("Depth Control (mm)");
	cvCreateTrackbar("Low", "Depth Control (mm)", depthLowerBound, 5000); //Max Range of 5 meters
	cvCreateTrackbar("High", "Depth Control (mm)", depthUpperBound, 5000);
}

void MyOpenCV::createHSVTrackbars()
{
	namedWindow("HSV Control", cv::WINDOW_NORMAL);
	resizeWindow("HSV Control", 500, 500);

	cvCreateTrackbar("LowH", "HSV Control", &hLowerBound, 179); //Hue (0 - 179)
	cvCreateTrackbar("HighH", "HSV Control", &hUpperBound, 179);

	cvCreateTrackbar("LowS", "HSV Control", &sLowerBound, 255); //Saturation (0 - 255)
	cvCreateTrackbar("HighS", "HSV Control", &sUpperBound, 255);

	cvCreateTrackbar("LowV", "HSV Control", &vLowerBound, 255); //Value (0 - 255)
	cvCreateTrackbar("HighV", "HSV Control", &vUpperBound, 255);
}

void MyOpenCV::displayData(bool leftWin, bool rightWin, int leftScore, int rightScore)
{
	//createFinalImageToDisplay(); // adds lines to the rgb image
	//std::cout << leftScore << rightScore << std::endl;
	if (!leftWin && !rightWin)
	{
		cv::String left = "Left: " + std::to_string(leftScore);
		cv::String right = "Right: " + std::to_string(rightScore);

		putText(rgbImage, left, Point(30, 60), FONT_HERSHEY_COMPLEX, 1.0, cvScalar(200, 0, 200));
		putText(rgbImage, right, Point(280, 60), FONT_HERSHEY_COMPLEX, 1.0, cvScalar(200, 0, 200));
	}
	else if (leftScore >= 7)
	{
		putText(rgbImage, "LEFT", Point(30, 30), FONT_HERSHEY_COMPLEX, 1.0, cvScalar(200, 0, 0));
		putText(rgbImage, "PLAYER", Point(30, 60), FONT_HERSHEY_COMPLEX, 1.0, cvScalar(0, 200, 0));
		putText(rgbImage, "WIN!!!!!!!", Point(30, 90), FONT_HERSHEY_COMPLEX, 1.0, cvScalar(0, 0, 200));
	}
	else if (rightScore >= 7)
	{
		putText(rgbImage, "RIGHT", Point(30, 30), FONT_HERSHEY_COMPLEX, 1.0, cvScalar(200, 0, 0));
		putText(rgbImage, "PLAYER", Point(30, 60), FONT_HERSHEY_COMPLEX, 1.0, cvScalar(0, 200, 0));
		putText(rgbImage, "WIN!!!!!!!", Point(30, 90), FONT_HERSHEY_COMPLEX, 1.0, cvScalar(0, 0, 200));
	}
	
	else if (leftWin)
	{
		putText(rgbImage, "LEFT", Point(30, 30), FONT_HERSHEY_COMPLEX, 1.0, cvScalar(200, 200, 200));
		putText(rgbImage, "PLAYER", Point(30, 60), FONT_HERSHEY_COMPLEX, 1.0, cvScalar(200, 200, 200));
		putText(rgbImage, "SCORE!!!!!!!", Point(30, 90), FONT_HERSHEY_COMPLEX, 1.0, cvScalar(200, 0, 200));
	}
	else if (rightWin)
	{
		putText(rgbImage, "RIGHT", Point(280, 30), FONT_HERSHEY_COMPLEX, 1.0, cvScalar(200, 0, 200));
		putText(rgbImage, "PLAYER", Point(280, 60), FONT_HERSHEY_COMPLEX, 1.0, cvScalar(200, 0, 200));
		putText(rgbImage, "SCORE!!!!!!!", Point(280, 90), FONT_HERSHEY_COMPLEX, 1.0, cvScalar(200, 0, 200));
	}

	//if (!imgThreshold.empty())
	imshow("Thresholded Image", imgThreshold); //show the thresholded image
	
	//if (!rgbImage.empty())
	imshow("RGB Image", rgbImage); //show the original image with the lines added on
}

void MyOpenCV::addToBallLatestPixelLocations(Point pnt)
{
	ballLatestPixelLocations[index] = pnt;

	if (index >= 9)
	{
		index = 0;
	}
}