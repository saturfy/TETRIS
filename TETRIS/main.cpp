#include <iostream>
#include <thread> // for the game logic
#include <Windows.h> // to grab the screen buffer
#include <vector>
#include <stdio.h> // needed to use sprintf
#include <wchar.h> // needed to set console font
#include <random>  
using namespace std;
// ------------------------IMPORTANT------------------------------- 
//we are using wchar_t whiuc is not UTF but unicode. This affects functions which load based on what is set in the compiler
// to make them woek the general character settings regardisng visual c and compire should be set to be UNICODE


int nFieldWidth = 12; //Width of gameplay area
int nFieldHeight = 18; // Height of gameplay area
unsigned char *pField = nullptr; // this holds the data of the play area in one dimensional array
int nScreenWidth = 80;			// Console Screen Size X (columns)
int nScreenHeight = 30;         // Console Screen Size Y (rows)

wstring tetromino[7]; // holds the tetris blocks

//Rotate function tells us that when the tetromino is rotated
// and we want to draw it on a 4x4 raster which point of the raster 
// correspond to which caharter in the tetromino's definition
// takes in x and y drawing coordinates of the raster, 
// rotation angle r
// 0: 0 degrees, 1: 90 degrees, 2: 180 degrees, 3: 270 degrees
// gives back character coordinate of the tetromino part in the definition
int Rotate(int px, int py, int r) {

	switch (r % 4)
	{
	case 0: return 4 * py + px;
	case 1: return 12 - 4 * px + py;
	case 2: return 15 - 4 * py - px;
	case 3: return 3 - py + 4 * px;
	}
	return 0;
}
// Rotation is the position of the tetromino
// posx posy is the left top part of the 4x4 square in which the tetromino travels
// nTetromino is the tetromino type
bool DoesPieceFit (int nTetromino, int nRotation, int nPosX, int nPosY)
{
	 for(int px = 0; px <4; px++)
		 for (int py = 0; py < 4; py++)
		 {
			 // Get the correct (rotated) index of the piece which has to be loaded to different places in the 4x4 square
			 int pi = Rotate(px, py, nRotation);

			 // Get the index of the field which has to be loaded to the given point in the 4x4 squre
			 int fi = (nPosY + py) * nFieldWidth + (nPosX + px);

			 // Make sure we are not aout of bounds

			 if (nPosX + px >= 0 && nPosX + px < nFieldWidth)
			 {
				 if (nPosY + py >= 0 && nPosY + py < nFieldHeight)
				 {
					 if (tetromino[nTetromino][pi] == L'X' && pField[fi] != 0)
						 return false; // fail on first hit
				 }
			 }
		  }





	return true;
}

// this funciton generates random numbers between 0 and 6
int randomfunc()
{
	int rnum = 0;
	random_device dev; // random device to seed the engine
	mt19937 rng(dev());// random generator with seed
	uniform_int_distribution<mt19937::result_type> dist6(0, 6);  // degines this random number distribuiton function which we can sample
	rnum = dist6(rng);  // use the defined distribution to generate numbers
	return rnum;
}

int main()
{
	// Create Assets
	//this is a sinlge string just appending this way makes it easier
	//to see the shape of the tetromino
	
	//Tetromino 1
	tetromino[0].append(L"..X.");
	tetromino[0].append(L"..X.");
	tetromino[0].append(L"..X.");
	tetromino[0].append(L"..X.");

	//Tetromino 2
	tetromino[1].append(L"..X.");
	tetromino[1].append(L".XX.");
	tetromino[1].append(L"..X.");
	tetromino[1].append(L"....");

	//Tetromino 3
	tetromino[2].append(L"....");
	tetromino[2].append(L".XX.");
	tetromino[2].append(L".XX.");
	tetromino[2].append(L"....");

	//Tetromino 4
	tetromino[3].append(L"..X.");
	tetromino[3].append(L".XX.");
	tetromino[3].append(L".X..");
	tetromino[3].append(L"....");

	//Tetromino 5
	tetromino[4].append(L".X..");
	tetromino[4].append(L".XX.");
	tetromino[4].append(L"..X.");
	tetromino[4].append(L"....");

	//Tetromino 6
	tetromino[5].append(L".X..");
	tetromino[5].append(L".X..");
	tetromino[5].append(L".XX.");
	tetromino[5].append(L"....");

	//Tetromino 7
	tetromino[6].append(L"..X.");
	tetromino[6].append(L"..X.");
	tetromino[6].append(L".XX.");
	tetromino[6].append(L"....");

	// Creating play area: it contains only numbers and characters are associated to numbers when this list is drawn to screen buffer. 

	wstring gr;    // this string here specifies what charcater we draw for a given number in the pField list. THe number corresponds to the place of the character in this string
	gr.append(L" ");  // empty space
	gr.append(L"\x25D8"); // tetromino 0
	gr.append(L"\x25D8"); // tetromino 1
	gr.append(L"\x25D8"); // tetromino 2
	gr.append(L"\x25D8"); // tetromino 3
	gr.append(L"\x25D8"); // tetromino 4
	gr.append(L"\x25D8"); // tetromino 5
	gr.append(L"\x25D8"); // tetromino 6
	gr.append(L"="); // line completion
	gr.append(L"\x2592"); // border

	/*
	zero is empty
	last, 9 is the border
	8 is the line symbol
	letters are corresponding to tetromino
	*/

	pField = new unsigned char[nFieldHeight * nFieldWidth]; // dynamical allocation of memory for the game area
	 // filling the array with data: borders and empty space inside
	for (int x = 0; x < nFieldWidth; x++) {
		for (int y = 0; y < nFieldHeight; y++) {
			pField[y * nFieldWidth + x] = (x == 0 || x == nFieldWidth - 1 || y == nFieldHeight - 1) ? 9 : 0;  // 9 represents border, 0 free space and this will be used to select the given characters form a list which represents these
		}
	}
	
	// Create screen Buffer: this 1Darray stores all of the screen content
	wchar_t *screen = new wchar_t[nScreenWidth*nScreenHeight]; // This is the vector which contains the screen data
	for (int i = 0; i < nScreenWidth*nScreenHeight; i++) screen[i] = L' '; // fills the screen vector with empty characters
	
	// this line declares and initializes hConsole variable as HANDLE type
	// Creates console handle with privileges and given type
	HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	
	// this makes the active buffer the one we created which is hConsole
	SetConsoleActiveScreenBuffer(hConsole);
	DWORD dwBytesWritten = 0;

	//first we set the console size to fit the size we set here
	SMALL_RECT consize;
	consize.Top = 0;  // cooridnates of top left corner
	consize.Left = 0;
	consize.Right = nScreenWidth - 1;  // coordinates of bottom right corner
	consize.Bottom = nScreenHeight - 1;
	// then set the console size (handle, coordinate type: bsolute=TRUE, rect object gives size)
	SetConsoleWindowInfo(hConsole, TRUE, &consize);

	// set windows size
	COORD wsize;
	wsize.X = nScreenWidth;
	wsize.X = nScreenHeight;
	SetConsoleScreenBufferSize(hConsole, wsize);

	// set console  font to the a type which looks better

	CONSOLE_FONT_INFOEX cfi;   // this struct contains font data
	cfi.cbSize = sizeof(cfi);
	cfi.nFont = 0;								// Widht and height set to the same so everything looks like a square																
	cfi.dwFontSize.X = 24;                   // Width of each character in the font 
	cfi.dwFontSize.Y = 24;                  // Height
	cfi.FontFamily = FF_DONTCARE;
	cfi.FontWeight = FW_NORMAL;       // this is the boldness of the font
	wcscpy_s(cfi.FaceName, L"Lucida Console"); // Choose your font
	SetCurrentConsoleFontEx(hConsole, FALSE, &cfi);

	// Game logic stuff

	int nCurrentPiece = randomfunc(); // type of the first tetromino
	int nCurrentRotation = 0;  // roation of the tetromino
	int nCurrentX = nFieldWidth / 2; // the coordinates of the top left corner of the 4x4 termoino raster
	int nCurrentY = 0;
	bool bKey[5]; // vector to store the sate of the four keys the sring in the INPUT fucntion decides which element corresponds to which key
	bool bRotateHold = false; // flag to see whether or not the user holds donw the rotate button. this is to stop the rotation in every game tick and rotates 1 times per key press
	bool bLeftHold = false; // flags for the other keys. ONly keystrokes will move the piece, holding keys down wont WE DONT DO THIS FOR THE DOWN KEY
	bool bRightHold = false;
	bool bFastLeft = false;  // if the left or right button is hold down for a time we move the piece faster
	bool bFastRight = false;
	bool bPausePress = false; // mimic pause buttion as a sticking button: if pressed remains pressed until another press
	int nFastT = 10;  // number of game ticks after fast moving occurs
	int nFastL = 0;  // counters for how long the button was pressed
	int nFastR = 0;  // counters for how long the button was pressed
	int nSpeed = 20; // the game speed in 50ms units
	int nSpeedCounter = 0; // counters game ticks. When the speed equals game ticks the pieces are forced down on step
	bool bForceDown = false;
	// The main game loop
	bool bGameOVer = false;

	// keeping score
	int nPieceCount = 0;
	int nScore = 0;

	
	vector<int> vLines; // to store the completed lines

	while (!bGameOVer)
	{
		// GAME TIMING======================================
		this_thread::sleep_for(50ms); // one game tick 
		if (!bPausePress) // we count only when ther is no pause
		{
			nSpeedCounter++;
			bForceDown = (nSpeedCounter == nSpeed);
		}

		

		// INPUT ===========================================
		// checks whether or not the given key was pressed
		for (int k = 0; k < 5; k++)
			/*
			getasynkeystate has retunr value short int 16 bit, and sets the highest bit to 1 if the buttin was pressed. So the return value is 1000 0000
			Because of the way the negative number representation works this is the lowest possible negative value in short int. In hexadecimal 0x8000
			The bitwise & operation just cheks this value. The mode to write hexadecimal codes of characters into strings is \x number, and this string is converted
			to unsigned char which is used for the input to the system function.
			The unsigned char can be used as int for small numbers the != forces the conversion of bit number into a logical type variable


			*/                                           //
			//                                                      R   L   D  SPACEBAR ENTER
			bKey[k] = (0x8000 & GetAsyncKeyState((unsigned char)("\x27\x25\x28\x20\x0D"[k]))) != 0;  // if you change this you have to change the one in the pause button code


		// GAME LOGIC =======================================
		// Decided what to do when keys pressed: check whether or not the piese would fit in the direction the player want to move it
		/*
		In the case of left and right keys there is a feature that if you press it more than game tick * nSpeedT it turns on fast moving and the piece moves
		into that direction as fast as possible. For shorter presses it just jumps one space. 

		Every game tick the keys are checked. If the key is pressed we increase the time counter nFastL. (we do this only when bFastLeft is not already on)
		If the counter reaches nSpeedT we turn on fast moving: bFastLeft = true, and reset the counter.
		The piece moves if bfastleft is on, or the keystate is bLeftHold = false (this means it was pressed just right now)
		we turn off fastmoving if the key is not released, which means bkey returned false.

		*/
		
		if (bKey[1]) // left key
		{
			bPausePress = false;  // we release thze pause button if any buttun pressed

			if (!bFastLeft)
			{
				nFastL++;
				if (nFastL == nFastT)
				{
					bFastLeft = true;
					nFastL = 0;
				}
			}

			if (( bFastLeft || !bLeftHold) && DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX - 1, nCurrentY))
			{
				nCurrentX = nCurrentX - 1;
				bLeftHold = true;
			}
		}
		else
		{
			bLeftHold = false;
			bFastLeft = false;
		}

		if (bKey[0]) // right key
		{
			bPausePress = false;  // we release thze pause button if any buttun pressed

			if (!bFastRight)
			{
				nFastR++;
				if (nFastR == nFastT)
				{
					bFastRight = true;
					nFastR = 0;
				}
			}
			
			if ((  bFastRight || !bRightHold) && DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX + 1, nCurrentY))
			{
				nCurrentX = nCurrentX + 1;
				bRightHold = true;
			}
		}
		else
		{
			bRightHold = false;
			bFastRight = false;
		}


		if (bKey[2]) // down key
		{
			bPausePress = false;  // we release thze pause button if any buttun pressed
			
			if ( DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY + 1))
			{
				nCurrentY = nCurrentY + 1;
			}
		}
		

		if (bKey[3]) // SPACEBAR key, rotation
		{
			bPausePress = false;  // we release thze pause button if any buttun pressed
			
			if (!bRotateHold && DoesPieceFit(nCurrentPiece, nCurrentRotation + 1, nCurrentX, nCurrentY)) //If the button was pressed we proceed to this check which fails when bRotateHold is ture
			{
				nCurrentRotation = nCurrentRotation + 1;
				bRotateHold = true; // if we already rotated the piece in this game tick this will be ture so the test will fail in this game tick 
			}
		}
		else
			bRotateHold = false; // if they did not pressed the key in this game tick the hold is false
		// how this rotation works
		/*
		How this works: 
		game checks whether or not the key was pressed. If it was pressed first time the flag is false and rotation happens, and the flag becomes true. 
		then execution stops and game waits 50 ms to check again. If the key is not pressed than flag becomes false and roation is possible at the next check. 
		If the key is pressed than we assume it was presed for 50 ms (last check and this one) and rotation does not happen and flag remain true. 
		SO we have to release the button at least for one check to be able to rotate again. SO if you push it continously it will rotate only once. 
		*/
		
		if (bKey[4]) // ENTER key, pause
		{
			
			bPausePress = true;
			
		}

		if (bForceDown && (! bPausePress)) 
		{    // check wheter or not the piece can be forced down
			if (DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY + 1))
				nCurrentY = nCurrentY + 1; // it moves down if it can
			else  // if it reached the pieces at the bottom
			{
				// lock current piece into the field: cycle through the tetromino and copy it into the field array
				for (int px = 0; px < 4; px++)
					for (int py = 0; py < 4; py++)
						if (tetromino[nCurrentPiece][Rotate(px, py, nCurrentRotation)] == L'X')  // we draw only when there is anything but '.' which represent empty space in tetromino definition
							pField[(nCurrentY + py)* nFieldWidth + (nCurrentX + px)] = nCurrentPiece + 1; // This is the position in the string  gr  we use to decide what character to draw

				nPieceCount++; // counting the pieces 
				if (nPieceCount % 10 == 0)     // we increase the speed after ten piece
					if (nSpeed >= 10) nSpeed--; // the game speed increases from 1 force down step/ sec to 1 force down per 0.5 sec
				
												// check have we got  any lines it is enough to check where the last tetromino stopped (this must be before we create a new one!!)
				for (int py = 0; py < 4; py++)
					if (nCurrentY + py < nFieldHeight - 1) // check only check the parts which are in the playing field
					{
						bool bline = true; // assume there is a line
						for (int px = 1; px < nFieldWidth - 1; px++)  // check for zeros in the line
							bline &= (pField[(nCurrentY + py) * nFieldWidth + px]) != 0; // right side is true when the current postision is not empty; than bline = bline & result  is true£; if bline becomes false it remains false no matter what becaus of the AND table
					
						if (bline) // if there is a line color it We remove it later
						{
							for (int px = 1; px < nFieldWidth - 1; px++)
								pField[(nCurrentY + py)* nFieldWidth + px] = 8; // set the field value to 8 which will be translated to a character when we draw

							vLines.push_back(nCurrentY + py); // store the y coordinate of the completed line; the smaller ones are standing to the left
						}
					}

				nScore += 25; // we give 25 pints per new piece
				if (!vLines.empty()) nScore += (( vLines.size() * (200 + (vLines.size() - 1) * 100))/2 ); // we give 100 points for 1st line 200 more for second etc..

				// choose next piece
				nCurrentPiece = randomfunc();
				nCurrentRotation = 0;  
				nCurrentX = nFieldWidth / 2; 
				nCurrentY = 0;

				// if piece does not fit we end the game
				bGameOVer = !DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY + 1);
			}
			
			nSpeedCounter = 0; // Restart the counter 
		}



		// RENDER OUTPUT=====================================

		//Draw the playing field into the buffer this is where we convert pField numbers into real characters
		for (int x = 0; x < nFieldWidth; x++) {
			for (int y = 0; y < nFieldHeight; y++) {
				// offset the screen so drawing of play area starts at 2nd coordinate

				screen[(y+2) * nScreenWidth + (x+2)] = (gr[pField[y * nFieldWidth + x]]);  // this is where we choose from the gr vector what to draw
			};
		}

		// Draw the current piece into the buffer
		for (int px = 0; px < 4; px++)
			for (int py = 0; py < 4; py++)
				if (tetromino[nCurrentPiece][Rotate(px, py, nCurrentRotation)] != L'.')  // we draw only when there is anything but '.' which represent empty space in tetromino definition
					screen[(nCurrentY + py + 2)* nScreenWidth + (nCurrentX + px + 2)] = gr[(nCurrentPiece + 1)]; // we look up from gr what character corresponds to this piece
		
		// draw score
				swprintf_s(&screen[2*nScreenWidth + nFieldWidth + 2 + 2], 16, L"SCORE: %8d", nScore);  // pointer to output char buffer,size, format string, data

		// Animate line completion
		if (!vLines.empty())
		{
			// if we have completed lines, we do an animation. We draw the frame than wait to show it
			WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);
			this_thread::sleep_for(400ms); // Delay for a bit

			// after we have shown the frame we delete the line we move everything down

			for (auto &v : vLines)  // range based looping: auto element : container -> accesses each element if we use auto  & element : conatiner we just observe the elemenets ; v is reference for the element
				for (int px = 1; px < nFieldWidth - 1; px++)
				{
					for (int py = v; py > 0; py--) // We jump to the upper most y line where we had a full line and move bacwards to redraw all lines
						pField[py * nFieldWidth + px] = pField[(py - 1) * nFieldWidth + px];  // copy the contents of line above the full line into the full line
					pField[px] = 0; // top row is set to zero
				}
			vLines.clear();
		}

		//Draw the data to console
		WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);

	}

	// the game ended
	CloseHandle(hConsole);
	cout << "Game Over !! Score: " << nScore << endl;
	system("pause");
	return 0;
}