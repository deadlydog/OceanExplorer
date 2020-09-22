/////////////////////////////////////////////////////////////////////
// CS 805 Assignment 2
// Written by Daniel Schroeder, Student # 200223894 in September 2007
/////////////////////////////////////////////////////////////////////

// Include Game components
#include "CGame.h"

// If we are running on Windows, do extra includes and use WinMain() function
#if OGRE_PLATFORM == PLATFORM_WIN32 || OGRE_PLATFORM == OGRE_PLATFORM_WIN32
	#define WIN32_LEAN_AND_MEAN
	#include "windows.h"

	INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT)
// Else we are not running on Windows, so use normal main() function
#else
	int main(int argc, char **argv)
#endif
{
    try
    {
		// Create the Game object and start running the Game
		CGame cApp;
        cApp.Go();		// Start running the Game
    }
	// If there was a problem
    catch(Exception& e)
    {
		// If we are running on Windows
		#if OGRE_PLATFORM == PLATFORM_WIN32 || OGRE_PLATFORM == OGRE_PLATFORM_WIN32
			// Display a Windows Message Box with the error
			MessageBoxA(NULL, e.getFullDescription().c_str(), "An exception has occurred!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
		// Else we are not running on Windows
		#else
			// Write the error message to the standard error console
			fprintf(stderr, "An exception has occurred: %s\n", e.getFullDescription().c_str());
		#endif
    }

    return 0;
}