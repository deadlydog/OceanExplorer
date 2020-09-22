#ifndef CGAME_H
#define CGAME_H

#include "CApplication.cpp"
#include "CTimer.h"
#include "CStateManager.h"
#include "CCameraSystem.cpp"
#include "CObject.h"
#include "UnitFunctions.cpp"
#include "COgreText.h"
#include <list>
#include "OgreStringConverter.h"
#include <fstream>
#include <string>

using namespace Ogre;
using namespace std;

/////////////////////////////////////////////////////////////////////
// Globals

// NOTE: All coordinates should be defined in meters
const int GiMETER = 25;		// Define how big one "meter" is

// Define the Levels Bounding Cube (defined in meters)
const int GiLEVEL_BACK = -150 * GiMETER;
const int GiLEVEL_FRONT = 150 * GiMETER;
const int GiLEVEL_LEFT = -150 * GiMETER;
const int GiLEVEL_RIGHT = 150 * GiMETER;
const int GiLEVEL_BOTTOM = -150 * GiMETER;
const int GiLEVEL_TOP = 150 * GiMETER;

// Define the Levels Dimensions
const int GiLEVEL_WIDTH = Math::Abs(GiLEVEL_LEFT) + Math::Abs(GiLEVEL_RIGHT);	// X Dimension
const int GiLEVEL_HEIGHT = Math::Abs(GiLEVEL_BOTTOM) + Math::Abs(GiLEVEL_TOP);	// Y Dimension
const int GiLEVEL_LENGTH = Math::Abs(GiLEVEL_BACK) + Math::Abs(GiLEVEL_FRONT);	// Z Dimension

// Define the valid Camera Modes
const int GiCAMERA_MODE_FIRST_PERSON		= 1;
const int GiCAMERA_MODE_THIRD_PERSON_FIXED	= 2;
const int GiCAMERA_MODE_THIRD_PERSON_CHASE	= 3;

// Define the possible level Goals
const int GiLEVEL_GOAL_NONE		= 0;
const int GiLEVEL_GOAL_SCORE	= 1;
const int GiLEVEL_GOAL_COLLECT	= 2;
const int GiLEVEL_GOAL_SURVIVE	= 3;

// Define the number of Levels in the game
const int GiNUMBER_OF_LEVELS	= 6;	// Levels 0 - 5 inclusive

// Define the number of Points needed to earn an extra Health point
const int GiPOINTS_NEEDED_FOR_EXTRA_HEALTH	= 2500;

// Makes the Player flash for a few seconds
bool MakePlayerFlash(void* _vpOptionalDataToPass);


/////////////////////////////////////////////////////////////////////
// Structures

// Holds a neighbor and it's distance
struct SNeighbor
{
	CObject* cpNeighbor;
	unsigned int uiDistanceSquared;

	SNeighbor()
	{
		cpNeighbor = NULL;
		uiDistanceSquared = 4200000000;	// Upper range of unsigned int
	}
};

// Holds the Levels Goal criteria
struct SGoal
{
	int iGoal;						// The specific type of Goal for the Level
	int iRequiredValue;				// The value the Player must achieve (Score, Objects Collected, etc)
	int iSpecificTypeToCollect;		// The type of Object the Player must Collect
	int iNumberCollected;			// The number of Objects the Player has Collected
	String sLevelStory;				// The story for the Level
	String sGoalDescription;		// The Description of the Goal to display to the Player
	String sInGameGoalDescription;	// The short Description of the Goal to display to the Player while playing
	String sGoalAccomplishedMessage;// The message displayed to the Player when they beat the Level

	SGoal()
	{
		Purge();
	}

	void Purge()
	{
		iGoal = GiLEVEL_GOAL_NONE;
		iRequiredValue = iSpecificTypeToCollect = iNumberCollected = 0;
		sLevelStory = sGoalDescription = sInGameGoalDescription= "";
	}
};


/////////////////////////////////////////////////////////////////////
// Game Class
// This class will handle all game processing, including player input
class CGame : public CApplication, public FrameListener, public OIS::KeyListener, public OIS::MouseListener, public OIS::JoyStickListener
{
public:
	// Constructor and Destructor
	CGame();
	~CGame();

	// Attach the Frame Listener and Input Handlers
    virtual void SetListeners();

	// Do Game Initialization Stuff
	virtual void GameInitialization();

	// This function will be called once every frame and will do all game processing
    bool frameStarted(const FrameEvent& _cEvent);

	// This function will be called once every frame and is used to update the FPS and other Debug info
	bool frameEnded(const FrameEvent& _cEvent);


	/////////////////////////////////////////////////////////////////
	// Game State Functions

	// Initial game screen
	static void GameStateMainMenu(void* _vpOptionalDataToPass, EPurpose _ePurpose);

	// Player beat the Level
	static void GameStatePlayerBeatLevel(void* _vpOptionalDataToPass, EPurpose _ePurpose);

	// Player beat the Game
	static void GameStatePlayerBeatGame(void* _vpOptionalDataToPass, EPurpose _ePurpose);

	// Player lost the Game
	static void GameStateGameOver(void* _vpOptionalDataToPass, EPurpose _ePurpose);

	// Loading the Levels resources
	static void GameStateLoadingLevel(void* _vpOptionalDataToPass, EPurpose _ePurpose);

	// Play the game
	static void GameStatePlay(void* _vpOptionalDataToPass, EPurpose _ePurpose);


	/////////////////////////////////////////////////////////////////
	// Helper Functions

	// Sets up the initial scene of the level
	bool SetupLevelScene();

	// Updates all of the Objects (Position, orientation, etc)
	void UpdateObjects();

	// Perform AI on the given Object
	void PerformAI(CObject* _cpObject);

	// Perform collision detection between Player and other entities
	void PerformCollisionDetection();

	// Update the Cameras Position and Orientation based on the mode being used
	void UpdateCamera();

	// Updates the HUDs displaying the in-game information
	void UpdateHUDs(float _fTimeRemaining);

	// Shows or Hides the Debug Overlay (which displays the Frames Per Second)
	bool ShowDebugHUD(bool _bShowDebugHUD = true);

	// Updates the info being shown on the Debug HUD
	void UpdateDebugHUD();

	// Toggles between the Players Camera, the Sharks Camera, and showing both Cameras
	void UseCamera(int _iCamera);

	// Reads in a configuration file and sets the appropriate variables
	bool ReadInConfigurationFile(string _sFileName, bool _bOnlyGetGoalVariables = false);

	// Creates an Object of the specified type
	void CreateObject(CObject& _cObject);

	// Returns how big the Length of the Object is compared to a Meter
	float GetObjectLengthToMeterRatio(CObject* _cpObject);

	// Function creates a new Flocking or Non-Flocking Fish and returns a pointer to it
	// NOTE: Using -1 for ObjectSpecificType will randomly pick which specific type of Fish to create
	CObject* CreateFish(bool _bIsFlockingFish = true, int _iObjectSpecificType = -1);

	// Function creates and returns a pointer to an Item
	CObject* CreateItem(int _iObjectSpecificType);


	/////////////////////////////////////////////////////////////////
	// Input Handler Functions

	// Key Pressed Listener - This function is called everytime a keyboard key is pressed
    virtual bool keyPressed(const OIS::KeyEvent& _cArg);

	// Key Released Listener - This function is called everytime a keyboard key is released
    virtual bool keyReleased(const OIS::KeyEvent& _cArg);


    // Mouse Movement Listener - This function is called everytime the mouse is moved
    virtual bool mouseMoved(const OIS::MouseEvent& _cArg);

	// Mouse Pressed Listener - This function is called everytime a mouse button is pressed
    virtual bool mousePressed(const OIS::MouseEvent& _cArg, OIS::MouseButtonID _ID);

	// Mouse Released Listener - This function is called everytime a mouse button is released
    virtual bool mouseReleased(const OIS::MouseEvent& _cArg, OIS::MouseButtonID _ID);


    // Joystick Pressed Listener - This function is called everytime a joystick button is pressed
    virtual bool buttonPressed(const OIS::JoyStickEvent &_cArg, int _iButton);

	// Joystick Released Listener - This function is called everytime a joystick button is released
    virtual bool buttonReleased(const OIS::JoyStickEvent &_cArg, int _iButton);

	// Joystick Moved Listener - This function is called everytime the joystick axis is moved
    virtual bool axisMoved(const OIS::JoyStickEvent &_cArg, int _iAxis);


	/////////////////////////////////////////////////////////////////
	// Error Handling Functions

	// Writes the Text to the Log File
	void WriteToLogFile(string _sText);

private:
	
	CClock* mcpClock;						// Holds a pointer to the Clock
	CScheduler* mcpScheduler;				// Holds a pointer to the Scheduler

	CStateManager mcGameStateManager;		// Object to manage the Game States

	FrameEvent mcFrameEvent;				// Pointer to the current Frame Event

	Overlay* mcpDebugOverlay;				// Holds a pointer to the Overlay used to display the Debug HUD
	bool mbShowDebugHUD;					// Tells whether or not to show the Debug HUD

	COgreText* mcpObjectiveTextBox;			// Holds a pointer to a TextArea to display the Levels Objective
	bool mbShowObjective;					// Tells whether the Objective should be shown, or Debug info

	COgreText* mcpPlayersHealthTextBox;		// Holds a pointer to a TextArea to display how many Enemies are remaining
	COgreText* mcpPlayersPointsTextBox;		// Holds a pointer to a TextArea to display how many Points the Player has
	bool mbShowHUDs;						// Tells whether or not HUDs (Heads Up Displays) should be displayed

	COgreText* mcpPlayerGotPointsTextBox;	// Holds a pointer to a TextArea to display a message when the Player captures a Fish
	CTimer mcPlayerGotPointsTimer;			// Holds how long it's been since the Player captured a fish

	bool mbReverseYAxis;					// Tells whether the Y-axis should be Reversed or not
	float mfMouseSpeed;						// Specifies the speed the of the Mouse

	COgreText* mcpLevelTimeRemainingTextBox;// Holds a pointer to a TextArea to display how much time is remaining to complete the level
	float mfLevelTime;						// Holds how much time the player has left to complete the level
	bool mbLevelHasTimeLimit;				// Holds if the level has to be beaten within a time limit or not
	
	Viewport* mcpViewport2;					// Holds a pointer to the second Viewport
	int miCameraToUse;						// Holds the Mode the Camera is in

	ColourValue msBackgroundColor;			// The background color of the Viewport

	CCameraSystem mcCameraSystem;			// The system used to control the Camera
	int miCameraMode;						// Holds what Camera Mode to use (First person, third person, etc)
	int miLastFramesCameraMode;				// Holds the Camera Mode which was used last frame
	bool mbLookingAround;					// Tells if the Player is currently looking around or not
	SceneNode* mcpPlayersPositionForCamera;	// Holds the position of the Player for rotating the Camera around the Player
	SceneNode* mcpDesiredCameraPosition;	// Holds the position of where the Camera should be
	SceneNode* mcpDesiredCameraTarget;		// Holds the position of where the Camera should be looking
	CTimer mcCameraTightnessTimer;			// Used to adjust the Cameras Tightness over a duration of time
	float mfCameraTightness;				// Tightness of the Camera (how fast it moves to where it should be)
	float mfCameraTargetTightness;			// Tightness of the Camera Target
	float mfCameraTargetMaxOffsetAngle;		// Max Offset Angle the Camera Target is allowed

	CCameraSystem mcSharkCameraSystem;		// The system used to control the Sharks Camera
	Camera* mcpSharkCamera;					// Holds the Camera following the Shark
	SceneNode* mcpDesiredSharkCameraPosition;	// Holds the position of where the SharkCamera should be
	SceneNode* mcpDesiredSharkCameraTarget;		// Holds the position of where the SharkCamera should be looking

	bool mbAllowMixedFlocking;				// Tells whether Flocking Fish should only Flock with their own kind or not
	CObject mcDefaultFlockingFish;			// Model Flocking Fish to hold default configuration parameters

	CObject* mcpPlayer;						// Pointer to the Players Object
	CObject* mcpShark;						// Pointer to the Shark Object
	list<CObject> mcObjectList;				// Holds a list of all Objects

	int miLevel;							// The Level being played
	SGoal msLevelGoal;						// The Goal of the Level

	float mfLevelFriction;					// Friction applied to all Objects

	int miTotalNumberOfObjectsCreated;		// Used to assign Objects their unique IDs

	int miInitialFlockingFishCount;			// Number of Flocking Fish created initially
	int miFlockingFishCount;				// Current number of Flocking Fish

	int miInitialNonFlockingFishCount;		// Number of Non-Flocking Fish created initially
	int miNonFlockingFishCount;				// Current number of Non-Flocking Fish


	////////////////////////////////////////////////////////////////
	// Make Global Functions a friend of this class so they can access the classes private data
	friend bool MakePlayerFlash(void* _vpOptionalDataToPass);
};

#endif
