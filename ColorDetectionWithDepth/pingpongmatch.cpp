#include "pingpongmatch.h"

PingPong::PingPong()
{
	cout << "Start Game!!! Right player serve.\n";
	//currentSide = 1;
	
	rightScore = 0;
	leftScore = 0;
	imageWidth = 512;
	previousSide = 1;
	rightWin = 1;
	leftWin = 0;
	volleyTime = clock();
	restartedGame = 1;
}


// if ball crosses the center then it counts as a serve. needs to cross back in order to add a point to each side
void PingPong::checkPosition(int position)
{
	//if (somebodyWon) // somebody just won. reset to initial condition
	//{
	//	// Setting to initial Condition
	//	previousSide = 1; // right is always the server
	//	rightWin = 1;
	//	leftWin = 1;
	//	return;
	//}

	clock_t currentTime = clock();
	if (currentTime - volleyTime < 750) // a volley has to be at least .75s
		return;

	if (!restartedGame && currentTime - lastTimeVisible > 2000)
	{
		if (previousSide) // ball was on rightSide and dissapeared for too long
		{
			cout << "Right Player Scores!!! Right player serves\n" << endl;
			leftWin = 0;
			rightWin = 1;
			rightScore++;
			cout << "Score: " << leftScore << "   " << rightScore << endl;
			previousSide = 0;
			restartedGame = 1;
			return;
		}
		else // ball was on leftSide and dissappeared for too long
		{
			cout << "Right Player Scores!!! Right player serves\n" << endl;
			leftWin = 0;
			rightWin = 1;
			rightScore++;
			cout << "Score: " << leftScore << "   " << rightScore << endl;
			previousSide = 0;
			restartedGame = 1;
			return;
		}
	}

	if (position == 0) // ball not visible
	{
		lastTimeVisible = clock();
		restartedGame = 0;
		return;
	}

	if (position > 0 && position < imageWidth)
	{
		restartedGame = 1; // ball was detected
		if (position > ((imageWidth / 2) + 25) ) // ball is on the RIGHT SIDE of the frame little past center
		{
			// This function does nothing if rightWin is true. 

			if((!previousSide && !rightWin)) // if ball was previously on the left but not if left side has just scored
			{
				cout << "Left Volley\n"; // Ball came from the left
				startVolley();
				previousSide = 1;
				rightWin = 0;
				leftWin = 0;
				return;
			}
			else if (!rightWin && !leftWin)
			{
				if (currentTime - volleyTime >= 2000) // 3s for ball to be returned 
				{
					cout << "Left Player Scores!!! Left player serves\n" << endl;
					leftScore++;
					cout << "Score: " << leftScore << "   " << rightScore << endl;
					leftWin = 1;
					rightWin = 0;
					previousSide = 1; // Left just scored so make it so left has to activate the timer
					return;
				}
			}
			previousSide = 1;
		}
		else if (position < ((imageWidth / 2) - 25)) // ball is on LEFT SIDE
		{
			//if (initialCondition) // Check to see if initial condition (somebody won) then just set to left side 
			//{
			//	previousSide = 0;
			//	rightWin = 0;
			//	leftWin = 0;
			//	return;
			//}
			
			if ((previousSide && !leftWin)) // if ball was previously on the right but right did not just score
			{
				cout << "Right Volley\n";
				startVolley();
				previousSide = 0; // previous side is now left (it came from the right)
				rightWin = 0;
				leftWin = 0;
				return;
			}
			else if (!leftWin && !rightWin) // changed both to opposite
			{
				if (currentTime - volleyTime >= 2000) // 3s for ball to be returned 
				{
					cout << "Right Player Scores!!! Right player serves\n" << endl;
					leftWin = 0;
					rightWin = 1;
					rightScore++;
					cout << "Score: " << leftScore << "   " << rightScore << endl;
					previousSide = 0; 
					return;
				}
			}
			previousSide = 0;
		}
	}
}

// Begins the timer for a single round. If the ball does not change sides in enough time during a round then it is a score for the false side
void PingPong::startVolley()
{
	volleyTime = clock();
}


void PingPong::resetScore()
{
	leftScore = 0;
	rightScore = 0;
}
