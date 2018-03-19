
//#include <Windows.h>
//#include <Ole2.h>
#include "mykinect.h"
#include "myopencv.h"
#include "pingpongmatch.h"
//#include "myopengl.h"

// Dont understand what happens when you do the array cast
int main(int argc, char** argv)
{
	//yopengl gl;

	//if (!gl.init(argc, argv)) return 1;
	//if (!gl.initkinect()) return 1;
	//std::cout << "press 1 - 4 to modify the depth range to display.\npress 0 to reset." << std::endl;
	// ...opengl texture and camera initialization... 
	//gl.initglut();

	//main loop
	//gl.execute(); 
	//std::cout << "asfasdfv";
	MyKinect kinect;
	MyOpenCV opencv((int*)(&kinect.maxDepth), (int*)(&kinect.minDepth));
	PingPong pp;

	// Working on using this main 
	// Think make the colordata pixels a regular byte and then check in opencv


	if (kinect.isInitialized())
	{
		while (true)
		{
			bool dataRead = kinect.getKinectData();
			if (dataRead)
			{
				//std::cout << "Createing Mat\n";
				opencv.createHSVMatFromRGBAArray(kinect.colorPixelData, kinect.colorWidth, kinect.colorHeight);
				//kinect.saveDepths(opencv.getLatestCenter());
				//pp.checkPosition(opencv.getLatestCenter().first);
				//std::cout << kinect.depthsIndex << " " << kinect.getLatestDepth() << std::endl;
				
				opencv.displayData(pp.getLeftWin(), pp.getRightWin(), pp.getLeftScore(), pp.getRightScore());
				//std::cout << kinect.getDepthAtCertianPixel(opencv.iLastX, opencv.iLastY);
				// only works if the funcion is called when the depth data is made. 

				if (cv::waitKey(30) == 32)
				{
					pp.resetScore();
				}
			}
			else
			{
				std::cout << "fail capture frame\n";
				continue;
			}
			

			//if (waitkey(30) == 27) //wait for 'esc' key press for 30ms. if 'esc' key is pressed, break loop. framerate is capped at 1/.03 = 33fps
			//{
			//	break;
			//}
			//opencv.displayData();
			
		}
		std::cout << "end grabbing frames\n";
	}
	else
		std::cout << "kinect not initialized correctly\n";

	return 0;
}