#pragma once
#include "myopencv.h"
#include "mykinect.h"
#include <time.h>

using namespace std;
using namespace cv;

class PingPong
{
private:
	// boolean flag for ball being on right(true) or left(false) side of table
	bool currentSide;
	// keeps previous position of the ball
	bool previousSide;
	// Score for right player
	int rightScore;
	// Score for left player
	int leftScore;
	// Start time for a round
	clock_t volleyTime;
	// Pixel width of the depth/color image we are using
	int imageWidth;
	bool rightWin;
	bool leftWin;
	clock_t lastTimeVisible;
	bool restartedGame = 1;
	

public:
	PingPong();
	void startVolley();
	void checkPosition(int pos);
	bool getLeftWin() const { return leftWin; }
	bool getRightWin() const { return rightWin; }
	int getLeftScore() const { return leftScore; }
	int getRightScore() const { return rightScore; }
	void resetScore();
};
