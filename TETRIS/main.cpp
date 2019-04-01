#include <iostream>
#include <Windows.h> // to grab the screen buffer
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

	// Creating play area

	pField = new unsigned char[nFieldHeight * nFieldWidth]; // dynamical allocation of memory for the game ara
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

	// The main game loop
	bool bGameOVer = false;

	while (!bGameOVer)
	{
		//Draw field: fill the screen contents
		for (int x = 0; x < nFieldWidth; x++) {
			for (int y = 0; y < nFieldHeight; y++) {
				// offset the screen so drawing of play area starts at 2nd coordinate
				screen[(y+2) * nScreenWidth + (x+2)] = L" ABCDEFG=#"[pField[y * nFieldWidth + x]];
			};
		}

		// Display Frame

		//first we set the console size to fit the size we set here
		SMALL_RECT consize;
		consize.Top = 0;  // cooridnates of top left corner
		consize.Left = 0;
		consize.Right = nScreenWidth - 1;  // coordinates of bottom right corner
		consize.Bottom = nScreenHeight - 1;
		// then set the console size (handle, coordinate type: bsolute=TRUE, rect object gives size)
		SetConsoleWindowInfo(hConsole, TRUE, &consize);
		
		//Draw the data to console
		WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);

	}
	return 0;
}