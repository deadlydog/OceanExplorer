#include "CGame.h"

// Constructor
// Initializes private data as NULL
CGame::CGame()
{
	// Seed the random number generator so that we actually get random numbers
	srand((unsigned int)time(NULL));
	rand();

	mcpClock = CClock::ReturnSingletonInstance();
	mcpScheduler = CScheduler::ReturnSingletonInstance();

	mbShowDebugHUD = true;

	mcpObjectiveTextBox = NULL;
	mbShowObjective = true;

	mcpPlayersHealthTextBox = NULL;
	mcpPlayersPointsTextBox = NULL;
	mbShowHUDs = true;

	mcpPlayerGotPointsTextBox = NULL;

	mbReverseYAxis = false;
	mfMouseSpeed = 1.0;

	mcpLevelTimeRemainingTextBox = NULL;
	mfLevelTime = 0.0;
	mbLevelHasTimeLimit = false;

	mcpViewport2 = NULL;
	miCameraToUse = 1;

	miCameraMode = GiCAMERA_MODE_THIRD_PERSON_CHASE;
	miLastFramesCameraMode = 0;
	mbLookingAround = false;
	mcpPlayersPositionForCamera = NULL;
	mcpDesiredCameraPosition = NULL;
	mcpDesiredCameraTarget = NULL;
	mfCameraTightness = 0.07f;
	mfCameraTargetTightness = 0.1f;
	mfCameraTargetMaxOffsetAngle = 20.0f;

	mbAllowMixedFlocking = false;

	mcpPlayer = NULL;
	mcpShark = NULL;

	mcpSharkCamera = NULL;
	mcpDesiredSharkCameraPosition = NULL;
	mcpDesiredSharkCameraTarget = NULL;

	miLevel = 0;

	mfLevelFriction = 0.0f;

	miTotalNumberOfObjectsCreated = 0;
	
	miInitialFlockingFishCount = 0;
	miFlockingFishCount = 0;

	miInitialNonFlockingFishCount = 0;
	miNonFlockingFishCount = 0;
}

// Destructor
// Deletes all allocated memory
CGame::~CGame()
{
	if (mcpObjectiveTextBox != NULL)
	{	
		delete mcpObjectiveTextBox;
		mcpObjectiveTextBox = NULL;
	}

	if (mcpPlayersHealthTextBox != NULL)
	{	
		delete mcpPlayersHealthTextBox;
		mcpPlayersHealthTextBox = NULL;
	}

	if (mcpPlayersPointsTextBox != NULL)
	{	
		delete mcpPlayersPointsTextBox;
		mcpPlayersPointsTextBox = NULL;
	}

	if (mcpLevelTimeRemainingTextBox != NULL)
	{	
		delete mcpLevelTimeRemainingTextBox;
		mcpLevelTimeRemainingTextBox = NULL;
	}
}

// Attach the Frame Listener and Input Handlers
void CGame::SetListeners()
{
	// Set this class as the Keyboard Input Handler
    if (mcpKeyboard != NULL)
	{	mcpKeyboard->setEventCallback(this);}

	// Set this class as the Mouse Input Handler
    if (mcpMouse != NULL)
	{	mcpMouse->setEventCallback(this);}

	// Set this class as the Joystick Input Handler
    if (mcpJoystick != NULL)
	{	mcpJoystick->setEventCallback(this);}

	// Set this class to be the Frame Listener
	mcpRoot->addFrameListener(this);
}

// Do Game Initialization Stuff
void CGame::GameInitialization()
{
	// Get a handle to the Debug Overlay
	mcpDebugOverlay = OverlayManager::getSingletonPtr()->getByName("Core/DebugOverlay");

	// Display the Debug HUD if it should be shown
	ShowDebugHUD(mbShowDebugHUD);

	// Set the starting Game State
	mcGameStateManager.SetState(GameStateMainMenu, this);
}

// This function will be called once every frame and will do all game processing
bool CGame::frameStarted(const FrameEvent& _cEvent)
{
	// Get the Keyboard, Mouse, and Joystick input for this frame
    if (mcpKeyboard != NULL)
	{	mcpKeyboard->capture();}
	if (mcpMouse != NULL)
	{	mcpMouse->capture();}
	if (mcpJoystick != NULL)
	{	mcpJoystick->capture();}

	// Save a pointer to the current Frame Event
	mcFrameEvent = _cEvent;

	// Call the current Game State function
	// This will return false if there are no more Game States to run, causing the Game to exit
	return mcGameStateManager.Process();
}

// This function will be called once every frame and is used to update the FPS and other Debug info
bool CGame::frameEnded(const FrameEvent& _cEvent)
{
	// Restart the Clock
	mcpClock->ReturnElapsedTimeAndResetClock();

	// Process Scheduled Events
	mcpScheduler->ProcessAllEvents();

	// Update the Debug HUD
	UpdateDebugHUD();

	return true;
}



/////////////////////////////////////////////////////////////////////
// Game State Functions

// Initial game screen
void CGame::GameStateMainMenu(void* _vpOptionalDataToPass, EPurpose _ePurpose)
{
	// Get the Game object pointer
	CGame* cpGame = (CGame*)_vpOptionalDataToPass;

	// Create pointers to the TextBoxes
	static COgreText* ScpTitleTextBox = NULL;
	static COgreText* ScpControlsTextBox = NULL;
	static COgreText* ScpPressToStartTextBox = NULL;

	// If this State is being entered for the first time
	if (_ePurpose == eInitialize)
	{
		// Make sure the Camera Systems aren't setup yet
		cpGame->mcCameraSystem.Purge();
		cpGame->mcSharkCameraSystem.Purge();

		// Clear the Scene to make sure it is empty with a black background
		cpGame->mcpSceneManager->clearScene();
		cpGame->mcpViewport->setBackgroundColour(ColourValue(0, 0, 0));

		// Create the Text strings to display
		String sTitle = "Daniel Schroeder's\n\tOcean Explorer";
		String sControls = "Controls:\n";
		sControls += " Esc - Return to Main Menu / Exit the game\n";
		sControls += " Tab - Toggle Frames Per Second\n";
		sControls += " Y - Toggle Y-axis reversal\n\n";
		sControls += " Enter - Toggle between the player and shark cameras\n";
		sControls += " 1 / 2 / 3 - Change camera mode (Player camera only)\n";
		sControls += " Z / X or Mouse Scroll Wheel - Zoom camera in / out\n\n";
		sControls += " Up / Down Arrows - Move submarine forward / backward\n";
		sControls += " Left / Right Arrows - Rotate submarine left / right\n";
		sControls += " Space Bar or Left Mouse Button - Move submarine upward\n";
		sControls += " Right Mouse Button (hold and move mouse) - Look around";

		String sPressToStart = "Press Space Bar to start a new game\n\n   Press Enter to just swim freely";

		// Display the Title
		ScpTitleTextBox = new COgreText();
		ScpTitleTextBox->SetAll(sTitle, 0.18, 0.03, 0.06, ColourValue(), "DansStarWars");
		ScpTitleTextBox->SetColour(ColourValue(0, 0.8, 0.8), ColourValue(0, 0.2, 0.2));

		// Display the Controls
		ScpControlsTextBox = new COgreText();
		ScpControlsTextBox->SetAll(sControls, 0.02, 0.15, 0.04, ColourValue(0.8, 0.8, 0.8));

		// Display what to press to start the game
		ScpPressToStartTextBox = new COgreText();
		ScpPressToStartTextBox->SetAll(sPressToStart, 0.02, 0.7, 0.04, ColourValue(), "DansWaterReflection");
		ScpPressToStartTextBox->SetColour(ColourValue(0, 0.8, 0.8), ColourValue(0, 0.2, 0.2));
	}
	// Else if this State should Shutdown
	else if (_ePurpose == eShutdown)
	{
		// Delete all memory allocated in the Initialize clause
		if (ScpTitleTextBox != NULL)
		{	
			delete ScpTitleTextBox;
			ScpTitleTextBox = NULL;
		}
		if (ScpControlsTextBox != NULL)
		{	
			delete ScpControlsTextBox;
			ScpControlsTextBox = NULL;
		}
		if (ScpPressToStartTextBox != NULL)
		{
			delete ScpPressToStartTextBox;
			ScpPressToStartTextBox = NULL;
		}
		return;
	}
	// Else if this State is being Returned to
	else if (_ePurpose == eReturning)
	{

	}
	// Else this is a Normal call to this function

}

// Player beat the Game
void CGame::GameStatePlayerBeatLevel(void* _vpOptionalDataToPass, EPurpose _ePurpose)
{
	// Get the Game object pointer
	CGame* cpGame = (CGame*)_vpOptionalDataToPass;

	static COgreText* ScpTitleTextBox = NULL;
	static COgreText* ScpControlsTextBox = NULL;

	// If this State is being entered for the first time
	if (_ePurpose == eInitialize)
	{
		// Create the Text strings to display
		String sControls = "Press Space Bar to continue";
		
		ScpTitleTextBox = new COgreText();
		ScpTitleTextBox->SetAll(cpGame->msLevelGoal.sGoalAccomplishedMessage, 0.05, 0.15, 0.05);
		ScpTitleTextBox->SetColour(ColourValue(0.8, 0.4, 0), ColourValue(0.2, 0.2, 0));

		ScpControlsTextBox = new COgreText();
		ScpControlsTextBox->SetAll(sControls, 0.25, 0.7, 0.05);
		ScpControlsTextBox->SetColour(ColourValue(0, 0.8, 0.8), ColourValue(0, 0.2, 0.2));
	}
	// Else if this State should Shutdown
	else if (_ePurpose == eShutdown)
	{
		// Delete all memory allocated in the Initialize clause
		if (ScpTitleTextBox != NULL)
		{	
			delete ScpTitleTextBox;
			ScpTitleTextBox = NULL;
		}
		if (ScpControlsTextBox != NULL)
		{	
			delete ScpControlsTextBox;
			ScpControlsTextBox = NULL;
		}

		return;
	}
	// Else if this State is being Returned to
	else if (_ePurpose == eReturning)
	{

	}
	// Else this is a Normal call to this function
}

// Player beat the Game
void CGame::GameStatePlayerBeatGame(void* _vpOptionalDataToPass, EPurpose _ePurpose)
{
	// Get the Game object pointer
	CGame* cpGame = (CGame*)_vpOptionalDataToPass;

	static COgreText* ScpTitleTextBox = NULL;
	static COgreText* ScpControlsTextBox = NULL;

	// If this State is being entered for the first time
	if (_ePurpose == eInitialize)
	{
		// Make sure the Camera Systems aren't setup yet
		cpGame->mcCameraSystem.Purge();
		cpGame->mcSharkCameraSystem.Purge();

		// Clear the Scene to make sure it is empty with a black background
		cpGame->mcpSceneManager->clearScene();
		cpGame->mcpViewport->setBackgroundColour(ColourValue(0, 0, 0));

		// Create the Text strings to display
		String sTitle = "        You beat the game! You Rock!!\n\n You and your crew were able to safely make\n it back to the surface. Your colleagues have\n sent the submarine to be repaired, and are\n all looking forward to getting back down there\n as soon as possible to see what other mysteries\n you can uncover\n\n Cheat - When in the Main Menu, press number\n         1 - 5 to go directly to that level";
		String sControls = "Press Space Bar to return to the Main Menu";
		
		ScpTitleTextBox = new COgreText();
		ScpTitleTextBox->SetAll(sTitle, 0.05, 0.1, 0.05);
		ScpTitleTextBox->SetColour(ColourValue(0.8, 0.4, 0), ColourValue(0.2, 0.2, 0));

		ScpControlsTextBox = new COgreText();
		ScpControlsTextBox->SetAll(sControls, 0.1, 0.7, 0.05);
		ScpControlsTextBox->SetColour(ColourValue(0, 0.8, 0.8), ColourValue(0, 0.2, 0.2));
	}
	// Else if this State should Shutdown
	else if (_ePurpose == eShutdown)
	{
		// Delete all memory allocated in the Initialize clause
		if (ScpTitleTextBox != NULL)
		{	
			delete ScpTitleTextBox;
			ScpTitleTextBox = NULL;
		}
		if (ScpControlsTextBox != NULL)
		{	
			delete ScpControlsTextBox;
			ScpControlsTextBox = NULL;
		}

		return;
	}
	// Else if this State is being Returned to
	else if (_ePurpose == eReturning)
	{

	}
	// Else this is a Normal call to this function
}

// Player lost the Game
void CGame::GameStateGameOver(void* _vpOptionalDataToPass, EPurpose _ePurpose)
{
	// Get the Game object pointer
	CGame* cpGame = (CGame*)_vpOptionalDataToPass;

	static COgreText* ScpTitleTextBox = NULL;
	static COgreText* ScpControlsTextBox = NULL;

	// If this State is being entered for the first time
	if (_ePurpose == eInitialize)
	{
		// Create the Text strings to display
		String sTitle = "You did not achieve your goal";
		String sControls = "Press Space Bar to restart the level";
		
		ScpTitleTextBox = new COgreText();
		ScpTitleTextBox->SetAll(sTitle, 0.2, 0.2, 0.05);
		ScpTitleTextBox->SetColour(ColourValue(0.8, 0, 0), ColourValue(0.2, 0.2, 0));

		ScpControlsTextBox = new COgreText();
		ScpControlsTextBox->SetAll(sControls, 0.15, 0.7, 0.05);
		ScpControlsTextBox->SetColour(ColourValue(0, 0.8, 0.8), ColourValue(0, 0.2, 0.2));
	}
	// Else if this State should Shutdown
	else if (_ePurpose == eShutdown)
	{
		// Delete all memory allocated in the Initialize clause
		if (ScpTitleTextBox != NULL)
		{	
			delete ScpTitleTextBox;
			ScpTitleTextBox = NULL;
		}
		if (ScpControlsTextBox != NULL)
		{	
			delete ScpControlsTextBox;
			ScpControlsTextBox = NULL;
		}

		// Clear the Scene
		cpGame->mcpSceneManager->clearScene();

		return;
	}
	// Else if this State is being Returned to
	else if (_ePurpose == eReturning)
	{

	}
	// Else this is a Normal call to this function
}

// Loading the Levels resources
void CGame::GameStateLoadingLevel(void* _vpOptionalDataToPass, EPurpose _ePurpose)
{
	// Get the Game object pointer
	CGame* cpGame = (CGame*)_vpOptionalDataToPass;

	// Textbox to display Loading message
	static COgreText* ScpLoadingMsgTextBox = NULL;
	static COgreText* ScpObjectiveTextBox = NULL;
	
	// Boolean to keep track if we have loaded the level already or not
	static bool SbLevelLoaded = false;

	// If this State is being entered for the first time
	if (_ePurpose == eInitialize)
	{
		// Clear the Scene to make sure it is empty with a black background
		cpGame->mcpSceneManager->clearScene();
		cpGame->mcpViewport->setBackgroundColour(ColourValue(0, 0, 0));

		// Record that the Level still needs to be loaded
		SbLevelLoaded = false;

		// Load the Levels Goal
		cpGame->msLevelGoal.Purge();
		cpGame->ReadInConfigurationFile("../../Level" + StringConverter::toString(cpGame->miLevel) + ".txt", true);

		// Display the Objective
		ScpObjectiveTextBox = new COgreText();
		ScpObjectiveTextBox->SetAll(cpGame->msLevelGoal.sLevelStory + "\n\nObjective:\n" + cpGame->msLevelGoal.sGoalDescription, 0.05, 0.1, 0.05);
		ScpObjectiveTextBox->SetColour(ColourValue(0.7, 0.7, 0), ColourValue(0.2, 0.2, 0));

		// Display the Loading message
		ScpLoadingMsgTextBox = new COgreText();
		ScpLoadingMsgTextBox->SetAll("Loading Level " + StringConverter::toString(cpGame->miLevel) + "...Please Wait...", 0.25, 0.7, 0.05);
		ScpLoadingMsgTextBox->SetColour(ColourValue(0, 0.8, 0.8), ColourValue(0, 0.2, 0.2));
		
		// Exit function so that the msg is displayed before loading resources, as it
		// takes a bit of time to load them and the screen is not updated while it happens
		return;
	}
	// Else if this State should Shutdown
	else if (_ePurpose == eShutdown)
	{
		// Delete all allocated TextBoxes
		if (ScpLoadingMsgTextBox != NULL)
		{
			delete ScpLoadingMsgTextBox;
			ScpLoadingMsgTextBox = NULL;
		}
		if (ScpObjectiveTextBox != NULL)
		{
			delete ScpObjectiveTextBox;
			ScpObjectiveTextBox = NULL;
		}
		return;
	}
	// Else if this State is being Returned to
	else if (_ePurpose == eReturning)
	{
	}
	// Else this is a Normal call to this function


	// If the Level still needs to be Loaded
	if (!SbLevelLoaded)
	{
		/////////////////////////////////////////////////////////////
		// Clear the Level's Scene

		// Clear the Scene to make sure it is empty to start with
		cpGame->mcpSceneManager->clearScene();

		// Clear out any Objects from the list
		cpGame->mcObjectList.clear();

		// Release any unused resources
		ResourceGroupManager::getSingleton().unloadUnreferencedResourcesInGroup(ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

		// Reset Counters
	//	cpGame->miTotalNumberOfObjectsCreated = 0;
		cpGame->miFlockingFishCount = 0;
		cpGame->miNonFlockingFishCount = 0;


		/////////////////////////////////////////////////////////////
		// Setup the Level's Scene

		// Setup the Level's Scene
		if (!cpGame->SetupLevelScene())
		{
			// Exit the Game since the Level could not be setup
			cpGame->mcGameStateManager.ShutdownAllStates(cpGame);
			return;
		}

		// Record that the Level has been Loaded
		SbLevelLoaded = true;
	}

	// Display message to start level
	ScpLoadingMsgTextBox->SetPosition(0.05, 0.7);
	ScpLoadingMsgTextBox->SetText("Press Space Bar or Left Mouse Button to continue");
}

// Play the game
void CGame::GameStatePlay(void* _vpOptionalDataToPass, EPurpose _ePurpose)
{
	// Get the Game object pointer
	CGame* cpGame = (CGame*)_vpOptionalDataToPass;

	// If this State is being entered for the first time
	if (_ePurpose == eInitialize)
	{
		/////////////////////////////////////////////////////////////
		// Setup HUDs

		// Create and initialize the TextBox to display the Players current Position
		cpGame->mcpObjectiveTextBox = new COgreText();
		cpGame->mcpObjectiveTextBox->SetAll("", 0.25, 0.88, 0.04);
		cpGame->mcpObjectiveTextBox->SetColour(ColourValue(0.8, 0, 0), ColourValue(0.2, 0, 0));

		// Show the Debug HUD if enabled
		cpGame->ShowDebugHUD(cpGame->mbShowDebugHUD);

		// Create and initialize the TextBox to display the Players Health
		cpGame->mcpPlayersHealthTextBox = new COgreText();
		cpGame->mcpPlayersHealthTextBox->SetAll("", 0.01, 0.01, 0.05);
		cpGame->mcpPlayersHealthTextBox->SetColour(ColourValue(0.8, 0, 0), ColourValue(0.2, 0, 0));

		// Create and initialize the TextBox to display how much time is remaining to complete the level
		cpGame->mcpLevelTimeRemainingTextBox = new COgreText();
		cpGame->mcpLevelTimeRemainingTextBox->SetAll("", 0.4, 0.01, 0.05);
		cpGame->mcpLevelTimeRemainingTextBox->SetColour(ColourValue(0.8, 0, 0), ColourValue(0.2, 0, 0));

		// Create and initialize the TextBox to display how many points the player has
		cpGame->mcpPlayersPointsTextBox = new COgreText();
		cpGame->mcpPlayersPointsTextBox->SetAll("", 0.7, 0.01, 0.05);
		cpGame->mcpPlayersPointsTextBox->SetColour(ColourValue(0.8, 0, 0), ColourValue(0.2, 0, 0));

		// Create and initialize the TextBox to display when the Player captures a fish
		cpGame->mcpPlayerGotPointsTextBox = new COgreText();
		cpGame->mcpPlayerGotPointsTextBox->Show(false);
	}
	// Else if this State should Shutdown
	else if (_ePurpose == eShutdown)
	{
		// Delete all memory allocated in the Initialize clause

		// If we have a handle to our Game object
		if (cpGame)
		{
			// Delete TextBoxes
			if (cpGame->mcpPlayersPointsTextBox != NULL)
			{
				delete cpGame->mcpPlayersPointsTextBox;
				cpGame->mcpPlayersPointsTextBox = NULL;		
			}
			if (cpGame->mcpPlayersHealthTextBox != NULL)
			{
				delete cpGame->mcpPlayersHealthTextBox;
				cpGame->mcpPlayersHealthTextBox = NULL;		
			}
			if (cpGame->mcpObjectiveTextBox != NULL)
			{	
				cpGame->mcpObjectiveTextBox->Show(false);
				delete cpGame->mcpObjectiveTextBox;
				cpGame->mcpObjectiveTextBox = NULL;
			}
			if (cpGame->mcpPlayerGotPointsTextBox != NULL)
			{	
				delete cpGame->mcpPlayerGotPointsTextBox;
				cpGame->mcpPlayerGotPointsTextBox = NULL;
			}

			// Clear the Camera Systems
			cpGame->mcCameraSystem.Purge();
			cpGame->mcSharkCameraSystem.Purge();
		}

		return;
	}
	// Else if this State is being Returned to
	else if (_ePurpose == eReturning)
	{

	}
	// Else this is a Normal call to this function

	
	//---------- Update All Objects ----------

	// Update all Objects AI, position, and orientation
	cpGame->UpdateObjects();


	//---------- Process Collisions ----------

	// Perform Player-Enemy collision detection
	cpGame->PerformCollisionDetection();


	//---------- Update HUDs ----------

	// If we are using the time for this level
	if (cpGame->mbLevelHasTimeLimit)
	{
		// Update time remaining to complete the level
		cpGame->mfLevelTime -= cpGame->mcFrameEvent.timeSinceLastFrame;
	}
	else
	{
		// Update the time spent in this level so far
		cpGame->mfLevelTime += cpGame->mcFrameEvent.timeSinceLastFrame;
	}
	
	// Display the Time in the HUD
	cpGame->UpdateHUDs(cpGame->mfLevelTime);


	//---------- Update the Camera ----------

	// Update the Cameras Position and Orientation
	cpGame->UpdateCamera();


	//---------- Check if Level is over ----------

	// Check if the Levels Goal was completed yet or not
	bool bPlayerBeatLevel = false;
	switch (cpGame->msLevelGoal.iGoal)
	{
		// If there is no Goal for this Level
		default:
		case GiLEVEL_GOAL_NONE:
			// Do nothing
		break;

		// If the Goal of the Level is to obtain a certain Score
		case GiLEVEL_GOAL_SCORE:
			// If the Player has the required Score
			if (cpGame->mcpPlayer->mcAttributes.ReturnNumberOfPoints() >= cpGame->msLevelGoal.iRequiredValue)
			{
				// Record that the Player beat this level
				bPlayerBeatLevel = true;
			}
		break;

		// If the Goal of the Level is to Collect a certain number of Objects
		case GiLEVEL_GOAL_COLLECT:
			// If the Player has collected the required number of Objects
			if (cpGame->msLevelGoal.iNumberCollected >= cpGame->msLevelGoal.iRequiredValue)
			{
				// Record that the Player beat this level
				bPlayerBeatLevel = true;
			}
		break;

		// If the Goal of the Level is to survive for a duration of time
		case GiLEVEL_GOAL_SURVIVE:
			// If the time duration has passed
			if (cpGame->mfLevelTime <= 0.0)
			{
				// Record that the Player beat this level
				bPlayerBeatLevel = true;
			}
		break;
	}

	// If the Player Beat the Level
	if (bPlayerBeatLevel)
	{
		// Switch to the Player Beat Level State
		cpGame->mcGameStateManager.ShutdownCurrentStateAndSetNextState(GameStatePlayerBeatLevel, cpGame);
		return;
	}


	// If this Level has a Time Limit and it has ran out
	if (cpGame->mbLevelHasTimeLimit && cpGame->mfLevelTime <= 0.0)
	{		
		// Switch to the Game Over state
		cpGame->mcGameStateManager.ShutdownCurrentStateAndSetNextState(GameStateGameOver, cpGame);
		return;
	}

	// If the Player has died
	if (cpGame->mcpPlayer->mcAttributes.ReturnHealth() < 1)
	{
		// Hide the Player
		cpGame->mcpPlayer->mcpSceneNode->setVisible(false);

		// Switch to the Game Over state
		cpGame->mcGameStateManager.ShutdownCurrentStateAndSetNextState(GameStateGameOver, cpGame);
		return;
	}
}



/////////////////////////////////////////////////////////////////
// Helper Functions

// Sets up the initial scene of the level
bool CGame::SetupLevelScene()
{
	// Temporary local variables
	int iIndex = 0;
	Entity* cpEntity = NULL;
	CObject cTempObject;

	
	/////////////////////////////////////////////////////////////
	// Set common Level variables

	// Friction applied to all Objects
	mfLevelFriction = 10.0 * GiMETER;

	// How far the Player can see
	float fPlayerViewDistance = GiLEVEL_WIDTH * 0.75f;


	/////////////////////////////////////////////////////////////
	// Setup Player

	// Create the Player in the middle of the ocean (0, 0, 0)
	cTempObject.SetID(miTotalNumberOfObjectsCreated++);
	cTempObject.SetGeneralObjectType(GiOBJECT_GENERAL_TYPE_PLAYER);
	cTempObject.SetName("Player");
	cTempObject.mcpEntity = mcpSceneManager->createEntity("PlayerEntity", "DansSubmarine.mesh");
	cTempObject.mcpSceneNode = mcpSceneManager->getRootSceneNode()->createChildSceneNode("PlayerSceneNode");
	cTempObject.mcpSceneNode->attachObject(cTempObject.mcpEntity);
	cTempObject.mcpEntity->setCastShadows(true);

	// Specify the direction the mesh faces by default
	cTempObject.SetMeshesDefaultForwardDirection(Vector3::UNIT_Z);
	cTempObject.SetMeshesDefaultUpDirection(Vector3::UNIT_Y);
	cTempObject.SetMeshesDefaultRightDirection(Vector3::UNIT_X);

	// Scale the Player to the desired size in Meters
	float fPlayerLengthInMeters = 4.0;
	float fPlayerSizeToMeterRatio = GetObjectLengthToMeterRatio(&cTempObject);
	fPlayerLengthInMeters *= fPlayerSizeToMeterRatio;
	cTempObject.mcpSceneNode->scale(fPlayerLengthInMeters, fPlayerLengthInMeters, fPlayerLengthInMeters);

	// Create a Spot Light on the front of the Player
	Light* cpPlayerSpotLight = mcpSceneManager->createLight("PlayerSpotLight");
	cTempObject.mcpSceneNode->attachObject(cpPlayerSpotLight);
	cpPlayerSpotLight->setType(Light::LT_SPOTLIGHT);
	cpPlayerSpotLight->setPosition(0.0, 0.0, 1.9 * GiMETER);
	cpPlayerSpotLight->setDirection(0.0, 0.0, 1.0);
	cpPlayerSpotLight->setSpotlightRange(Radian(Degree(20)), Radian(Degree(30)));
	cpPlayerSpotLight->setAttenuation(GiLEVEL_WIDTH, 1.0, 0, 0);
	cpPlayerSpotLight->setDiffuseColour(ColourValue(0.8, 0.8, 0.8));
	cpPlayerSpotLight->setSpecularColour(ColourValue(0.8, 0.8, 0.8));
	cpPlayerSpotLight->setVisible(true);
	

	// Specify Players Acceleration and Velocities
	float fMovementAcceleration = 20.0 * GiMETER;	// Players Acceleration rate
	float fMaxMovementVelocity = ((mfLevelFriction / GiMETER) + 15.0) * GiMETER;	// Players Max Velocity
	float fRotationAcceleration = 180;				// Players Rotation Acceleration
	float fMaxRotationVelocity = 90.0;				// Players Max Rotation Velocity

	// Set Players Movement Acceleration and Max Velocities
	cTempObject.SetDefaultAccelerationRate(fMovementAcceleration);
	cTempObject.SetMaxVelocitySpeed(fMaxMovementVelocity);

	// Set Players Rotation Acceleration and Max Velocities
	cTempObject.SetDefaultRotationAccelerationRate(fRotationAcceleration);
	cTempObject.SetMaxRotationVelocitySpeed(fMaxRotationVelocity);

	// Apply Level Friction
	cTempObject.SetFriction(mfLevelFriction);

	// Apply gravity as an external force
	cTempObject.mcExternalForceList.push_front(SExternalForce(Vector3::NEGATIVE_UNIT_Y * 7.0 * GiMETER, 0));

	// Specify how fast Mouse should rotate the Player
	mfMouseSpeed = 3.0;

	// Insert the Object into the Object List
	mcObjectList.push_front(cTempObject);

	// Save a handle to the Player Object
	mcpPlayer = &mcObjectList.front();


	/////////////////////////////////////////////////////////////
	// Setup Camera

	// Setup the Camera System
	mcCameraSystem.SetupCamera("CameraSystem", mcpSceneManager, mcpCamera);

	// Specify where we would like the Camera to be positioned and where it should point towards
	mcpPlayersPositionForCamera = mcpPlayer->mcpSceneNode->createChildSceneNode("PlayersPositionForCameraSceneNode");
	mcpPlayersPositionForCamera->setFixedYawAxis(true);
	mcpDesiredCameraPosition = mcpPlayersPositionForCamera->createChildSceneNode("DesiredCameraPositionSceneNode", Vector3(0, 2 * GiMETER, -15 * GiMETER));
	mcpDesiredCameraTarget = mcpPlayer->mcpSceneNode->createChildSceneNode("DesiredCameraTargetSceneNode", Vector3(0, 0, 5 * GiMETER));

	// Move the actual Camera and Target positions to the desired positions
	mcCameraSystem.SetCameraPosition(mcpDesiredCameraPosition->getWorldPosition());
	mcCameraSystem.SetTargetPosition(mcpDesiredCameraTarget->getWorldPosition());

	// Set the Max Camera Offset Distance and Max Target Offset Angle
	mcCameraSystem.SetMaxCameraOffsetDistance(fPlayerViewDistance * 0.75);
	mcCameraSystem.SetMaxTargetOffsetAngle(mfCameraTargetMaxOffsetAngle);

	// Set how close and how far the Camera should start clipping at
	mcpCamera->setNearClipDistance(5.0);
	mcpCamera->setFarClipDistance(0);

	// Specify the Camera Mode to use
	miCameraMode = GiCAMERA_MODE_THIRD_PERSON_CHASE;
	miLastFramesCameraMode = 0;


	/////////////////////////////////////////////////////////////
	// Setup Level Lighting and the Background Color

	// Set up the Sky Box
//	mcpSceneManager->setSkyBox(true, "DansMedia/OceanSkyBox", fPlayerViewDistance, false);

	// Set the background and fog colour
	msBackgroundColor = ColourValue(0, 0.2, 0.2);
	mcpViewport->setBackgroundColour(msBackgroundColor);
	mcpSceneManager->setFog(FOG_LINEAR, msBackgroundColor, 0.0, 0.0, fPlayerViewDistance);

	// Set ambient light
	mcpSceneManager->setAmbientLight(ColourValue(0.2, 0.2, 0.2));

	// Create a directional light to mimic sunlight
	Light* cpSunLight = mcpSceneManager->createLight("SunLight");
	cpSunLight->setType(Light::LT_DIRECTIONAL);
	cpSunLight->setDirection(0.0, -1.0, 0.5);
	cpSunLight->setDiffuseColour(ColourValue(0.4, 0.4, 0.4));
	cpSunLight->setSpecularColour(ColourValue(0.4, 0.4, 0.4));

	// Set to show Shadows
	mcpSceneManager->setShadowTechnique(/*SHADOWTYPE_TEXTURE_MODULATIVE*//*SHADOWTYPE_STENCIL_MODULATIVE*/SHADOWTYPE_STENCIL_ADDITIVE);
	mcpSceneManager->setShadowUseInfiniteFarPlane(false);


	/////////////////////////////////////////////////////////////
	// Setup Ocean Floor and Surface

	// Specify the size of the Ground (make it larger than needed so World appears larger than it is)
	int iGroundWidth = GiLEVEL_WIDTH * 4;
	int iGroundLength = GiLEVEL_LENGTH * 4;

	// If we haven't created the Ground resource yet
	if (MeshManager::getSingleton().getByName("Ground").isNull())
	{
		// Create Ground plane and position it on the oceans floor
		Plane cGroundPlane(Vector3::UNIT_Y, GiLEVEL_BOTTOM - (2 * GiMETER));
		MeshManager::getSingleton().createPlane("Ground",
			ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, cGroundPlane,
			iGroundWidth, iGroundLength, 20, 20, true, 1, 50, 50, Vector3::UNIT_Z);
	}

	// If we haven't created the Surface resource yet
	if (MeshManager::getSingleton().getByName("Surface").isNull())
	{
		// Create Surface plane and position it on the oceans surface
		Plane cSurfacePlane(Vector3::NEGATIVE_UNIT_Y, -GiLEVEL_TOP - (2 * GiMETER));
		MeshManager::getSingleton().createPlane("Surface",
			ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, cSurfacePlane,
			iGroundWidth, iGroundLength, 20, 20, true, 1, 30, 30, Vector3::UNIT_Z);
	}

	// If we haven't created the Front Wall resource yet
	if (MeshManager::getSingleton().getByName("FrontWall").isNull())
	{
		// Create Surface plane and position it on the oceans surface
		Plane cSurfacePlane(Vector3::NEGATIVE_UNIT_Z, -GiLEVEL_FRONT - (2 * GiMETER));
		MeshManager::getSingleton().createPlane("FrontWall",
			ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, cSurfacePlane,
			iGroundWidth, iGroundLength, 20, 20, true, 1, 30, 30, Vector3::UNIT_Y);
	}

	// If we haven't created the Back Wall resource yet
	if (MeshManager::getSingleton().getByName("BackWall").isNull())
	{
		// Create Surface plane and position it on the oceans surface
		Plane cSurfacePlane(Vector3::UNIT_Z, GiLEVEL_BACK - (2 * GiMETER));
		MeshManager::getSingleton().createPlane("BackWall",
			ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, cSurfacePlane,
			iGroundWidth, iGroundLength, 20, 20, true, 1, 30, 30, Vector3::UNIT_Y);
	}

	// If we haven't created the Left Wall resource yet
	if (MeshManager::getSingleton().getByName("LeftWall").isNull())
	{
		// Create Surface plane and position it on the oceans surface
		Plane cSurfacePlane(Vector3::UNIT_X, GiLEVEL_LEFT - (2 * GiMETER));
		MeshManager::getSingleton().createPlane("LeftWall",
			ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, cSurfacePlane,
			iGroundWidth, iGroundLength, 20, 20, true, 1, 30, 30, Vector3::UNIT_Y);
	}

	// If we haven't created the Right Wall resource yet
	if (MeshManager::getSingleton().getByName("RightWall").isNull())
	{
		// Create Surface plane and position it on the oceans surface
		Plane cSurfacePlane(Vector3::NEGATIVE_UNIT_X, -GiLEVEL_RIGHT - (2 * GiMETER));
		MeshManager::getSingleton().createPlane("RightWall",
			ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, cSurfacePlane,
			iGroundWidth, iGroundLength, 20, 20, true, 1, 30, 30, Vector3::UNIT_Y);
	}
	
	// Insert the Ground plane into the scene and set its Material to use
    cpEntity = mcpSceneManager->createEntity("GroundEntity", "Ground");
	mcpSceneManager->getRootSceneNode()->createChildSceneNode()->attachObject(cpEntity);
    cpEntity->setMaterialName("DansMedia/OceanBottom");
    cpEntity->setCastShadows(false);

	// Insert the Ground plane into the scene and set its Material to use
    cpEntity = mcpSceneManager->createEntity("SurfaceEntity", "Surface");
	mcpSceneManager->getRootSceneNode()->createChildSceneNode()->attachObject(cpEntity);
    cpEntity->setMaterialName("DansMedia/OceanSurface");
    cpEntity->setCastShadows(false);

	// Insert the Front Wall plane into the scene and set its Material to use
    cpEntity = mcpSceneManager->createEntity("FrontWallEntity", "FrontWall");
	mcpSceneManager->getRootSceneNode()->createChildSceneNode()->attachObject(cpEntity);
    cpEntity->setMaterialName("DansMedia/OceanWall");
    cpEntity->setCastShadows(true);

	// Insert the Back Wall plane into the scene and set its Material to use
    cpEntity = mcpSceneManager->createEntity("BackWallEntity", "BackWall");
	mcpSceneManager->getRootSceneNode()->createChildSceneNode()->attachObject(cpEntity);
    cpEntity->setMaterialName("DansMedia/OceanWall");
    cpEntity->setCastShadows(true);

	// Insert the Left Wall plane into the scene and set its Material to use
    cpEntity = mcpSceneManager->createEntity("LeftWallEntity", "LeftWall");
	mcpSceneManager->getRootSceneNode()->createChildSceneNode()->attachObject(cpEntity);
    cpEntity->setMaterialName("DansMedia/OceanWall");
    cpEntity->setCastShadows(true);

	// Insert the Right Wall plane into the scene and set its Material to use
    cpEntity = mcpSceneManager->createEntity("RightWallEntity", "RightWall");
	mcpSceneManager->getRootSceneNode()->createChildSceneNode()->attachObject(cpEntity);
    cpEntity->setMaterialName("DansMedia/OceanWall");
    cpEntity->setCastShadows(true);


	/////////////////////////////////////////////////////////////
	// Setup the Shark

	// Set the Sharks ID, Name, and General Type
	cTempObject.SetID(miTotalNumberOfObjectsCreated++);
	cTempObject.SetName("Enemy" + StringConverter::toString(cTempObject.GetID()));
	cTempObject.SetGeneralObjectType(GiOBJECT_GENERAL_TYPE_ENEMY);
	cTempObject.SetSpecificObjectType(GiOBJECT_SPECIFIC_TYPE_SHARK);

	// Setup the Shark
	CreateObject(cTempObject);

	// Set how often this Objects Neighbor list should be updated
	cTempObject.mcAIHelper.SetUpdateTimeInterval((int)Math::RangeRandom(750, 1250));

	// Insert the Object into the Object List
	mcObjectList.push_front(cTempObject);

	// Erase the temp Objects data
	cTempObject.Purge();

	// Save a handle to the Shark Object
	mcpShark = &mcObjectList.front();


	// Setup the Sharks Camera System
	mcpSharkCamera = mcpSceneManager->createCamera("SharkCamera");
	mcSharkCameraSystem.SetupCamera("SharkCameraSystem", mcpSceneManager, mcpSharkCamera);
	mcSharkCameraSystem.SetCameraTightness(0.2);
	mcSharkCameraSystem.SetTargetTightness(0.2);

	// Specify where we would like the Sharks Camera to be positioned and where it should point towards
	mcpDesiredSharkCameraPosition = mcpShark->mcpSceneNode->createChildSceneNode("DesiredSharkCameraPositionSceneNode", Vector3(0, 4 * GiMETER, -150 * GiMETER));
	mcpDesiredSharkCameraTarget = mcpShark->mcpSceneNode->createChildSceneNode("DesiredSharkCameraTargetSceneNode", Vector3(0, 0, 5 * GiMETER));

	// Move the actual Camera and Target positions to the desired positions
	mcSharkCameraSystem.SetCameraPosition(mcpDesiredSharkCameraPosition->getWorldPosition());
	mcSharkCameraSystem.SetTargetPosition(mcpDesiredSharkCameraTarget->getWorldPosition());

	// Set the Max Camera Offset Distance and Max Target Offset Angle
	mcSharkCameraSystem.SetMaxCameraOffsetDistance(50 * GiMETER);
	mcSharkCameraSystem.SetMaxTargetOffsetAngle(20);

	// Set how close and how far the Shark Camera should start clipping at
	mcpSharkCamera->setNearClipDistance(5.0);
	mcpSharkCamera->setFarClipDistance(0);


	/////////////////////////////////////////////////////////////
	// Read in the Default Configuration File (after we have a handle to the Player and Shark)

	// Initialize the Default Flocking Fish model
	mcDefaultFlockingFish.mcAIHelper.SetIsAFlockingFish(true);

	// Read in the Default Configuration File to set default values
	ReadInConfigurationFile("../../Defaults.txt");


	/////////////////////////////////////////////////////////////
	// Setup Level Specific Variables

	// Load the Levels Configuration File
	ReadInConfigurationFile("../../Level" + StringConverter::toString(miLevel) + ".txt");

	// Loop through and create the Flocking fish
	for (iIndex = 0; iIndex < miInitialFlockingFishCount; iIndex++)
	{
		CreateFish(true);
	}

	// Loop through and create the Non-Flocking fish
	for (iIndex = 0; iIndex < miInitialNonFlockingFishCount; iIndex++)
	{
		CreateFish(false);
	}

	// Return that the Level was setup successfully
	return true;
}

// Updates the Players Position based on input, and makes sure they stay in-bounds
void CGame::UpdateObjects()
{
	// Temp local variables
	Vector3 sObjectOldPosition = Vector3::ZERO;		// Holds objects position before moving
	Vector3 sObjectNewPosition = Vector3::ZERO;		// Holds objects position after moving
	Vector3 sVector = Vector3::ZERO;				// Temporary vector
	list<CObject>::iterator cObjectListIterator;	// Used to loop through the Objects

	// Loop through all Objects
	for (cObjectListIterator = mcObjectList.begin(); cObjectListIterator != mcObjectList.end(); cObjectListIterator++)
	{
		// If this Object is Dead
		if (cObjectListIterator->GetState() == GiOBJECT_STATE_DEAD)
		{
			// Don't do any updating
			continue;
		}

		// If this Object is an Enemy
		if (cObjectListIterator->GetGeneralObjectType() == GiOBJECT_GENERAL_TYPE_ENEMY)
		{
			// If this is a Flocking Fish or the Shark
			if (cObjectListIterator->mcAIHelper.GetIsAFlockingFish() ||
				cObjectListIterator->GetSpecificObjectType() == GiOBJECT_SPECIFIC_TYPE_SHARK)
			{
				// If it's time to update this Objects Neighbor List
				if (cObjectListIterator->mcAIHelper.mcNeighborList.empty() || 
					cObjectListIterator->mcAIHelper.mcUpdateTimer.XTimeHasPassedUsingSavedTime(cObjectListIterator->mcAIHelper.GetUpdateTimeInterval()))
				{
					// Variable to iterate through the Object List
					list<CObject>::iterator cNeighborIterator;

					// Max number of Neighbors this Object can have
					int iMaxNumOfNeighbors = cObjectListIterator->mcAIHelper.GetMaxNumberOfNeighbors();

					// Variable to hold the nearest Neighbors so far, and their distance
					SNeighbor saTempNeighborList[20];

					// Forget all previous neighbors
					cObjectListIterator->mcAIHelper.mcNeighborList.clear();

					// Set the current number of Neighbors
					int iNumOfNeighbors = 0;

					// Loop through all Objects and look for closest neighbors
					for (cNeighborIterator = mcObjectList.begin(); cNeighborIterator != mcObjectList.end(); cNeighborIterator++)
					{
						// If this Neighbor is Dead
						if (cNeighborIterator->GetState() == GiOBJECT_STATE_DEAD)
						{
							// Move to the next Object
							continue;
						}

						// Only flocking fish should be considered as Neighbors
						if (cNeighborIterator->mcAIHelper.GetIsAFlockingFish() &&
							cNeighborIterator->GetID() != cObjectListIterator->GetID())
						{
							// If Mixed Flocking is not allowed, AND this is not the Shark AND 
							// the Neighbor is not the same Specific type as this fish
							if (!mbAllowMixedFlocking && cObjectListIterator->GetSpecificObjectType() != GiOBJECT_SPECIFIC_TYPE_SHARK &&
								cObjectListIterator->GetSpecificObjectType() != cNeighborIterator->GetSpecificObjectType())
							{
								continue;
							}

							// Calculate the squared distance to this fish (faster than calculating the actual distance)
							unsigned int uiDistanceSquared = cObjectListIterator->mcpSceneNode->getWorldPosition().squaredDistance(cNeighborIterator->mcpSceneNode->getWorldPosition());
							
							// Insert this Neighbor into the end of the Temp Neighbor List
							saTempNeighborList[iNumOfNeighbors].uiDistanceSquared = uiDistanceSquared;
							saTempNeighborList[iNumOfNeighbors].cpNeighbor = &*cNeighborIterator;

							// Use insertion-sort to sort Neighbors
							int i, j;
							CObject* cpObject;
							for (i = 1; i <= iNumOfNeighbors; i++)
							{
								uiDistanceSquared = saTempNeighborList[i].uiDistanceSquared;
								cpObject = saTempNeighborList[i].cpNeighbor;
								j = i;
								while ((j > 0) && (saTempNeighborList[j - 1].uiDistanceSquared > uiDistanceSquared))
								{
									saTempNeighborList[j] = saTempNeighborList[j - 1];
									j = j - 1;
								}
								saTempNeighborList[j].uiDistanceSquared = uiDistanceSquared;
								saTempNeighborList[j].cpNeighbor = cpObject;
							}

							if (iNumOfNeighbors < iMaxNumOfNeighbors)
							{
								// Increment the number of Neighbors found
								iNumOfNeighbors++;
							}
						}
					}

					// Insert the closest Neighbors into the Objects Neighbor list
					int iPosition = 0;
					while (saTempNeighborList[iPosition].cpNeighbor != NULL && iPosition < iMaxNumOfNeighbors)
					{
						// Insert this Object into the Neighbor list
						cObjectListIterator->mcAIHelper.mcNeighborList.push_back(saTempNeighborList[iPosition].cpNeighbor);
						iPosition++;
					}
				}
			}

			// Perform this Objects AI
			PerformAI(&*cObjectListIterator);
		}

		// Save Objects Position and Orientation before moving
		sObjectOldPosition = cObjectListIterator->mcpSceneNode->getWorldPosition();

		// Update the Objects position, rotation, animation, etc
		cObjectListIterator->Update((float)mcFrameEvent.timeSinceLastFrame);


		// Make sure the Object has not gone out of bounds

		// Get the Objects new Position
		sObjectNewPosition = cObjectListIterator->mcpSceneNode->getWorldPosition();

		// If the Object has gone past the Left or Right boundary
		if (sObjectNewPosition.x < GiLEVEL_LEFT|| sObjectNewPosition.x > GiLEVEL_RIGHT)
		{
			// Move the Objects X-coordinate back to its previous position
			sObjectNewPosition.x = sObjectOldPosition.x;
		}

		// If the Object has gone past the Bottom or Top boundary
		if (sObjectNewPosition.y < GiLEVEL_BOTTOM || sObjectNewPosition.y > GiLEVEL_TOP)
		{
			// Move the Objects Y-coordinate back to its previous position
			sObjectNewPosition.y = sObjectOldPosition.y;
		}

		// If the Object has gone past the Back or Front boundary
		if (sObjectNewPosition.z < GiLEVEL_BACK || sObjectNewPosition.z > GiLEVEL_FRONT)
		{
			// Move the Objects Z-coordinate back to its previous position
			sObjectNewPosition.z = sObjectOldPosition.z;
		}

		// If the Object is out of bounds and should be moved back in bounds
		if (!cObjectListIterator->mcpSceneNode->getWorldPosition().positionEquals(sObjectNewPosition))
		{
			// Move the Object back into a valid position
			cObjectListIterator->mcpSceneNode->setPosition(sObjectNewPosition);
		}
	}
}

// Perform AI on the given Object
void CGame::PerformAI(CObject* _cpObject)
{
	// If this Object is Dead, or this is an Item
	if (_cpObject->GetState() == GiOBJECT_STATE_DEAD || 
		_cpObject->GetGeneralObjectType() == GiOBJECT_GENERAL_TYPE_ITEM)
	{
		// Don't do any AI for this Object
		return;
	}

	// If this is a Flocking Fish
	if (_cpObject->mcAIHelper.GetIsAFlockingFish())
	{
		// If this Flocking Fish should Age
		if (_cpObject->mcAIHelper.mcDegradationTimer.XTimeHasPassedUsingSavedTime(1000))
		{
			// Increment Fish's Age
			_cpObject->mcAIHelper.SetAge(_cpObject->mcAIHelper.GetAge() + 1);
		}

		// If this Fish can spawn
		if (miFlockingFishCount < miInitialFlockingFishCount &&
			_cpObject->mcAIHelper.GetAge() > _cpObject->mcAIHelper.GetSpawnAge() &&
			(int)Math::RangeRandom(1, 50) == 25)
		{
			// If the Fish cannot sense the Shark
			float fSenseSharkRangeSquared = _cpObject->mcAIHelper.GetSenseSharkRange();
			fSenseSharkRangeSquared *= fSenseSharkRangeSquared;
			if (_cpObject->mcpSceneNode->getWorldPosition().squaredDistance(mcpShark->mcpSceneNode->getWorldPosition()) > fSenseSharkRangeSquared)
			{
				// Spawn a new Flocking Fish of the same type
				CObject* cpNewFish = CreateFish(true, _cpObject->GetSpecificObjectType());

				// Set both Fishes Age to zero
				_cpObject->mcAIHelper.SetAge(0);
				cpNewFish->mcAIHelper.SetAge(0);

				// Position and orient the new Fish the same as this one
				cpNewFish->mcpSceneNode->setPosition(_cpObject->mcpSceneNode->getWorldPosition());
				cpNewFish->PointObjectInDirection(_cpObject->GetDirectionObjectIsFacing());
			}
		}

		CObject* cpNeighbor = NULL;		// Temp pointer to a Neighbor

		// Loop through Fish's Neighbors until we find a valid one
		while (!_cpObject->mcAIHelper.mcNeighborList.empty() && cpNeighbor == NULL)
		{
			// Get the Neighbor closest to the Fish
			cpNeighbor = _cpObject->mcAIHelper.mcNeighborList.front();

			// If the Neighbor is Dead
			if (cpNeighbor == NULL || cpNeighbor->GetState() == GiOBJECT_STATE_DEAD)
			{
				// Remove the Neighbor from the Neighbor list
				_cpObject->mcAIHelper.mcNeighborList.pop_front();
				cpNeighbor = NULL;
			}
		}

		// If the Fish can sense a Neighbor (it is flocking)
		float fSenseNeighborRangeSquared = _cpObject->mcAIHelper.GetSenseNeighborRange();
		fSenseNeighborRangeSquared *= fSenseNeighborRangeSquared;
		if (cpNeighbor != NULL && _cpObject->mcpSceneNode->getWorldPosition().squaredDistance(cpNeighbor->mcpSceneNode->getWorldPosition()) < fSenseNeighborRangeSquared)
		{
			// Turn off Auto-Tracking so we have control over this Fish
			_cpObject->SetIfObjectShouldMoveToDestinationAutomatically(false, false);
			_cpObject->SetRotationVelocity(Vector3::ZERO);

			// Set that this Fish is flocking
			_cpObject->SetState(GiOBJECT_STATE_FLOCKING);

			int iNumberOfNeighbors = 0;				// Used to record how many Neighbors this fish has
			list<CObject*>::iterator cNeighborIterator;	// Used to traverse the Neighbor list

			Vector3 sCumulativePosition = Vector3::ZERO;			// Cumulative Position of all Neighbors
			Vector3 sCumulativeSeparationDirection = Vector3::ZERO;	// Cumulative Direction away from all Neighbors
			Vector3 sCumulativeAlignmentDirection = Vector3::ZERO;	// Cumulative Alignment (facing) Direction of all Neighbors
			float fCumulativeVelocity = 0.0f;						// Cumulative Velocity of all Neighbors

			// Store the Fish's Position locally as it's used many times
			Vector3 sFishPosition = _cpObject->mcpSceneNode->getWorldPosition();

			// Loop through the Neighbor List and get the Neighbors within range of this fish
			for (cNeighborIterator = _cpObject->mcAIHelper.mcNeighborList.begin(); cNeighborIterator != _cpObject->mcAIHelper.mcNeighborList.end(); cNeighborIterator++)
			{
				cpNeighbor = *cNeighborIterator;

				// If this Neighbor is Dead
				if (cpNeighbor == NULL || cpNeighbor->GetState() == GiOBJECT_STATE_DEAD)
				{
					// Move on to the next Neighbor
					continue;
				}

				// If this Neighbor is within range of the Fish
				if (cpNeighbor != NULL && _cpObject->mcpSceneNode->getWorldPosition().squaredDistance(cpNeighbor->mcpSceneNode->getWorldPosition()) < fSenseNeighborRangeSquared)
				{
					// Find the Direction away from this Neighbor (Separation), and the Distance between the Fish
					Vector3 sAwayDirection = sFishPosition - cpNeighbor->mcpSceneNode->getWorldPosition();
					float fDistance = sAwayDirection.normalise();

					// Max distance for a fish to be considered a Neighbor
					float fMaxNeighborDistance = cpNeighbor->mcAIHelper.GetSenseNeighborRange();

					// Calculate the Distance Coefficient for Separation with this Neighbor
					float fSeparationDistanceFactor = ((fMaxNeighborDistance - fDistance) / fMaxNeighborDistance);
					fSeparationDistanceFactor *= fSeparationDistanceFactor;

					// Add this Neighbors distance scaled Away Direction to the Cumulative Away Direction
					sCumulativeSeparationDirection += (fSeparationDistanceFactor * sAwayDirection);

					// Add this Neighbors Position to the Cumulative Position (Cohesion)
					sCumulativePosition += cpNeighbor->mcpSceneNode->getWorldPosition();

					// Add this Neighbors Orientation (Alignment) to the Cumulative Alignment Direction
					sCumulativeAlignmentDirection += cpNeighbor->GetDirectionObjectIsFacing();

					// Add this Neighbors Velocity to the Cumulative Velocity
					fCumulativeVelocity += cpNeighbor->GetVelocitySpeed();

					// Increment the number of Neighbors found
					iNumberOfNeighbors++;
				}
			}

			// Set this Fish's Velocity to the average velocity of all Neighbors
			_cpObject->SetVelocitySpeed(fCumulativeVelocity / iNumberOfNeighbors);

			// Find the average Position of the Neighbors (Cohesion)
			sCumulativePosition /= iNumberOfNeighbors;

			// Find the Direction to the average Position
			Vector3 sCohesionDirection = sCumulativePosition - sFishPosition;

			// Direction away from the Shark and Sub
			Vector3 sAwayFromPreditorDirection = Vector3::ZERO;

			// If the Fish can sense the Shark
			float fSenseSharkRangeSquared = _cpObject->mcAIHelper.GetSenseSharkRange();
			fSenseSharkRangeSquared *= fSenseSharkRangeSquared;
			if (_cpObject->mcpSceneNode->getWorldPosition().squaredDistance(mcpShark->mcpSceneNode->getWorldPosition()) < fSenseSharkRangeSquared)
			{
				// Calculate Direction away from the Shark
				sAwayFromPreditorDirection = sFishPosition - mcpShark->mcpSceneNode->getWorldPosition();
				sAwayFromPreditorDirection.normalise();
			}

			// If the Fish can sense the Submarine
			float fSenseSubmarineRangeSquared = _cpObject->mcAIHelper.GetSenseSubmarineRange();
			fSenseSubmarineRangeSquared *= fSenseSubmarineRangeSquared;
			if (_cpObject->mcpSceneNode->getWorldPosition().squaredDistance(mcpPlayer->mcpSceneNode->getWorldPosition()) < fSenseSubmarineRangeSquared)
			{
				// Calculate Direction away from the Submarine
				Vector3 sAwayFromSubmarineDirection = sFishPosition - mcpPlayer->mcpSceneNode->getWorldPosition();
				sAwayFromSubmarineDirection.normalise();

				// Add this to the Direction away from the Shark
				sAwayFromPreditorDirection += sAwayFromSubmarineDirection;
			}

			// Normalise all Direction factors
			sCumulativeAlignmentDirection.normalise();
			sCohesionDirection.normalise();
			sCumulativeSeparationDirection.normalise();
			sAwayFromPreditorDirection.normalise();

			// Sum the vectors to find this Fish's new Direction
			Vector3 sNewDirection = ((_cpObject->mcAIHelper.GetAlignmentCoefficient() * sCumulativeAlignmentDirection) +
									(_cpObject->mcAIHelper.GetCohesionCoefficient() * sCohesionDirection) +
									(_cpObject->mcAIHelper.GetSeparationCoefficient() * sCumulativeSeparationDirection) + 
									(_cpObject->mcAIHelper.GetPreditorAvoidanceCoefficient() * sAwayFromPreditorDirection));


			// If the Fish is steering into the left or right Ocean Boundary
			if ((sFishPosition.x < (GiLEVEL_LEFT + (2 * GiMETER)) && sNewDirection.x < 0) ||
				(sFishPosition.x > (GiLEVEL_RIGHT - (2 * GiMETER)) && sNewDirection.x > 0))
			{
				// Calculate middle position of the ocean directly above/below Fish's current location
				Vector3 sOceanMiddleDirection = sFishPosition;
				sOceanMiddleDirection.x = 0;

				// Calculate Direction towards the middle of the ocean
				sOceanMiddleDirection = sOceanMiddleDirection - sFishPosition;
				sOceanMiddleDirection.normalise();

				// Factor new Direction into the Fish's final direction
				sNewDirection.normalise();
				sNewDirection += (sOceanMiddleDirection * (float)Math::RangeRandom(0.7, 1.0));
			}

			// If the Fish is steering into the bottom or top Ocean Boundary
			if ((sFishPosition.y < (GiLEVEL_BOTTOM + (2 * GiMETER)) && sNewDirection.y < 0) ||
				(sFishPosition.y > (GiLEVEL_TOP - (2 * GiMETER)) && sNewDirection.y > 0))
			{
				// Calculate middle position of the ocean directly above/below Fish's current location
				Vector3 sOceanMiddleDirection = sFishPosition;
				sOceanMiddleDirection.y = 0;

				// Calculate Direction towards the middle of the ocean
				sOceanMiddleDirection = sOceanMiddleDirection - sFishPosition;
				sOceanMiddleDirection.normalise();

				// Factor new Direction into the Fish's final direction
				sNewDirection.normalise();
				sNewDirection += (sOceanMiddleDirection * (float)Math::RangeRandom(0.7, 1.0));
			}

			// If the Fish is steering into the back or front Ocean Boundary
			if ((sFishPosition.z < (GiLEVEL_BACK + (2 * GiMETER)) && sNewDirection.z < 0) ||
				(sFishPosition.z > (GiLEVEL_FRONT - (2 * GiMETER)) && sNewDirection.z > 0))
			{
				// Calculate middle position of the ocean directly above/below Fish's current location
				Vector3 sOceanMiddleDirection = sFishPosition;
				sOceanMiddleDirection.z = 0;

				// Calculate Direction towards the middle of the ocean
				sOceanMiddleDirection = sOceanMiddleDirection - sFishPosition;
				sOceanMiddleDirection.normalise();

				// Factor new Direction into the Fish's final direction
				sNewDirection.normalise();
				sNewDirection += (sOceanMiddleDirection * (float)Math::RangeRandom(0.7, 1.0));
			}

			// Make the Fish point in the new Direction
			_cpObject->PointObjectInDirection(sNewDirection);
		}
		// Else it cannot sense any Neighbors
		else
		{
			// Make sure Auto-Tracking is turned on and it is moving at full speed
			_cpObject->SetIfObjectShouldMoveToDestinationAutomatically(true, true);
			_cpObject->SetAccelerationRate(_cpObject->GetDefaultAccelerationRate());
			_cpObject->SetRotationVelocitySpeed(_cpObject->GetDefaultRotationVelocitySpeed());

			// If the Fish can sense the Shark
			float fSenseSharkRangeSquared = _cpObject->mcAIHelper.GetSenseSharkRange();
			fSenseSharkRangeSquared *= fSenseSharkRangeSquared;
			if (_cpObject->mcpSceneNode->getWorldPosition().squaredDistance(mcpShark->mcpSceneNode->getWorldPosition()) < fSenseSharkRangeSquared)
			{
				// If the Fish is not already Avoiding the Shark
				if (_cpObject->GetState() != GiOBJECT_STATE_AVOID_SHARK ||
					_cpObject->GetIfObjectHasReachedAllDestinations())
				{
					// Clear the Fish's Destination list
					_cpObject->mcDestinationList.clear();

					// Calculate direction away from Shark
					Vector3 sAwayDirection = _cpObject->mcpSceneNode->getWorldPosition() - mcpShark->mcpSceneNode->getWorldPosition();
					sAwayDirection.normalise();

					// Make the Fish swim away from the Shark
					Vector3 sAwayDestination = _cpObject->mcpSceneNode->getWorldPosition() + (sAwayDirection * 200 * GiMETER);
					SDestination* spNewDestination =  new SDestination(NULL, &sAwayDestination);
					_cpObject->mcDestinationList.push_front(spNewDestination);
					_cpObject->GetNextDestination();

					// Set Fish's State
					_cpObject->SetState(GiOBJECT_STATE_AVOID_SHARK);
				}
			}
			// Else the Fish can't sense the Shark
			else
			{
				// If the Fish can sense another fish nearby
				float fSenseFishRangeSquared = _cpObject->mcAIHelper.GetSenseFishRange();
				fSenseFishRangeSquared *= fSenseFishRangeSquared;
				if (cpNeighbor != NULL && _cpObject->mcpSceneNode->getWorldPosition().squaredDistance(cpNeighbor->mcpSceneNode->getWorldPosition()) < fSenseFishRangeSquared)
				{
					// If the Fish is not already tracking this Fish
					if (_cpObject->GetState() != GiOBJECT_STATE_TRACKING_FISH ||
						_cpObject->GetIfObjectHasReachedAllDestinations() ||
						_cpObject->GetCurrentDestination() != cpNeighbor->mcpSceneNode)
					{
						// Clear the Fish's Destination list
						_cpObject->mcDestinationList.clear();

						// Track this Fish
						SDestination* spNewDestination =  new SDestination(cpNeighbor->mcpSceneNode);
						_cpObject->mcDestinationList.push_front(spNewDestination);
						_cpObject->GetNextDestination();

						// Set Fish's State
						_cpObject->SetState(GiOBJECT_STATE_TRACKING_FISH);
					}
				}
				// Else the Fish cannot sense the Shark or any Fish nearby, so go exploring
				else 
				{
					// If the Fish is not already exploring
					if (_cpObject->GetState() != GiOBJECT_STATE_EXPLORING ||
						_cpObject->GetIfObjectHasReachedAllDestinations())
					{
						// Clear the Fish's Destination list
						_cpObject->mcDestinationList.clear();

						// Give them a new random Destination to go to
						Vector3 sRandomDestination = Vector3::ZERO;
						sRandomDestination.x = Math::RangeRandom(GiLEVEL_LEFT, GiLEVEL_RIGHT);
						sRandomDestination.y = Math::RangeRandom(GiLEVEL_BOTTOM, GiLEVEL_TOP);
						sRandomDestination.z = Math::RangeRandom(GiLEVEL_BACK, GiLEVEL_FRONT);

						// Add the new Destination to the Object
						SDestination* spNewDestination =  new SDestination(NULL, &sRandomDestination);
						_cpObject->mcDestinationList.push_front(spNewDestination);
						_cpObject->GetNextDestination();

						// Set the Fish's State
						_cpObject->SetState(GiOBJECT_STATE_EXPLORING);
					}
				}
			}
		}
	}
	// Else if this is the Shark
	else if (_cpObject->GetSpecificObjectType() == GiOBJECT_SPECIFIC_TYPE_SHARK)
	{
		// If it's time for the Shark to lose some more Health (get hungry)
		if (_cpObject->mcAIHelper.mcDegradationTimer.XTimeHasPassedUsingSavedTime(_cpObject->mcAIHelper.GetHealthDegradeDelay()))
		{
			// Decrement Sharks Health
			_cpObject->mcAttributes.SetHealthRelative(-1);
		}

		// Sharks actions are based on how hungry it is, so get Sharks Health (hunger)
		int iSharkHealth = _cpObject->mcAttributes.ReturnHealth();

		// Calculate Sharks max speed based on it's Health
		_cpObject->SetMaxVelocitySpeed(mfLevelFriction + ((iSharkHealth * _cpObject->mcAIHelper.GetVelocityCoefficient()) * GiMETER));

		// If the Shark attacked the Player recently
		if (!_cpObject->mcAIHelper.mcHitDelayTimer.XTimeHasPassedUsingSavedTimeWithoutRestartingTimer(_cpObject->mcAIHelper.GetHitDelay()))
		{
			// If the Shark just attacked the Player last frame
			if (_cpObject->GetState() != GiOBJECT_STATE_AVOID_PLAYER)
			{
				// Clear the Sharks Destination list
				_cpObject->mcDestinationList.clear();

				// Calculate direction to swim away from Player
				Vector3 sAwayDirection = _cpObject->mcpSceneNode->getWorldPosition() - mcpPlayer->mcpSceneNode->getWorldPosition();
				sAwayDirection.normalise();

				// Make the Shark swim away from the Player
				Vector3 sAwayDestination = _cpObject->mcpSceneNode->getWorldPosition() + (sAwayDirection * 200 * GiMETER);
				SDestination* spNewDestination =  new SDestination(NULL, &sAwayDestination);
				_cpObject->mcDestinationList.push_front(spNewDestination);
				_cpObject->GetNextDestination();

				// Set Sharks State
				_cpObject->SetState(GiOBJECT_STATE_AVOID_PLAYER);
			}

			// Exit the function
			return;
		}

		// Else If the Shark is Full
		if (iSharkHealth > _cpObject->mcAIHelper.GetFullHealth())
		{
			// If the Shark can sense the Submarine (Player), chase it
			float fSenseSubmarineRangeSquared = _cpObject->mcAIHelper.GetSenseSubmarineRange();
			fSenseSubmarineRangeSquared *= fSenseSubmarineRangeSquared;
			if (_cpObject->mcpSceneNode->getWorldPosition().squaredDistance(mcpPlayer->mcpSceneNode->getWorldPosition()) < fSenseSubmarineRangeSquared)
			{
				// If the Shark is not already tracking the Submarine
				if (_cpObject->GetState() != GiOBJECT_STATE_TRACKING_PLAYER ||
					_cpObject->GetIfObjectHasReachedAllDestinations())
				{
					// Clear the Sharks Destination list
					_cpObject->mcDestinationList.clear();

					// Track the Submarine
					SDestination* spNewDestination =  new SDestination(mcpPlayer->mcpSceneNode);
					_cpObject->mcDestinationList.push_front(spNewDestination);
					_cpObject->GetNextDestination();

					// Set Sharks State
					_cpObject->SetState(GiOBJECT_STATE_TRACKING_PLAYER);
				}
			}
			// Else the Shark should go off exploring
			else
			{
				// If the Shark is not already exploring
				if (_cpObject->GetState() != GiOBJECT_STATE_EXPLORING ||
					_cpObject->GetIfObjectHasReachedAllDestinations())
				{
					// Clear the Sharks Destination list
					_cpObject->mcDestinationList.clear();

					// Give them a new random Destination to go to
					Vector3 sRandomDestination = Vector3::ZERO;
					sRandomDestination.x = Math::RangeRandom(GiLEVEL_LEFT, GiLEVEL_RIGHT);
					sRandomDestination.y = Math::RangeRandom(GiLEVEL_BOTTOM, GiLEVEL_TOP);
					sRandomDestination.z = Math::RangeRandom(GiLEVEL_BACK, GiLEVEL_FRONT);

					// Add the new Destination to the Object
					SDestination* spNewDestination =  new SDestination(NULL, &sRandomDestination);
					_cpObject->mcDestinationList.push_front(spNewDestination);
					_cpObject->GetNextDestination();

					// Set the Sharks State
					_cpObject->SetState(GiOBJECT_STATE_EXPLORING);
				}
			}
		}
		// Else if the Shark is Nourished
		else if (iSharkHealth > _cpObject->mcAIHelper.GetNourishedHealth())
		{
			// If the Shark can sense the Submarine (Player), approach it
			float fSenseSubmarineRangeSquared = _cpObject->mcAIHelper.GetSenseSubmarineRange();
			fSenseSubmarineRangeSquared *= fSenseSubmarineRangeSquared;
			if (_cpObject->mcpSceneNode->getWorldPosition().squaredDistance(mcpPlayer->mcpSceneNode->getWorldPosition()) < fSenseSubmarineRangeSquared)
			{
				// If the Shark is not already approaching the Submarine
				if (_cpObject->GetState() != GiOBJECT_STATE_APPROACHING_PLAYER ||
					_cpObject->GetIfObjectHasReachedAllDestinations())
				{
					// Clear the Sharks Destination list
					_cpObject->mcDestinationList.clear();

					Vector3 sDestination = Vector3::ZERO;
					do
					{
						// Calculate a position near the Submarine
						sDestination = mcpPlayer->mcpSceneNode->getWorldPosition();
						sDestination.x += (-1 * (int)Math::RangeRandom(1, 3)) * (UnitCubed(Math::UnitRandom()) * 25 * GiMETER);
						sDestination.y += (-1 * (int)Math::RangeRandom(1, 3)) * (UnitCubed(Math::UnitRandom()) * 25 * GiMETER);
						sDestination.z += (-1 * (int)Math::RangeRandom(1, 3)) * (UnitCubed(Math::UnitRandom()) * 25 * GiMETER);
					// Make sure Sharks new Destination is within the ocean boundaries
					}while (sDestination.x < GiLEVEL_LEFT || sDestination.x > GiLEVEL_RIGHT ||
							sDestination.y < GiLEVEL_BOTTOM || sDestination.y > GiLEVEL_TOP ||
							sDestination.z < GiLEVEL_BACK || sDestination.z > GiLEVEL_FRONT);
					
					// Approach the Submarine using the new Destination
					SDestination* spNewDestination =  new SDestination(NULL, &sDestination);
					_cpObject->mcDestinationList.push_front(spNewDestination);
					_cpObject->GetNextDestination();

					// Set Sharks State
					_cpObject->SetState(GiOBJECT_STATE_APPROACHING_PLAYER);
				}
			}
			// Else the Shark can not sense the Submarine
			else
			{
				CObject* cpSharkNeighbor = NULL;

				// Loop through Sharks Neighbors until we find a valid one
				while (!_cpObject->mcAIHelper.mcNeighborList.empty() && cpSharkNeighbor == NULL)
				{
					// Get the Neighbor closest to the Shark
					cpSharkNeighbor = _cpObject->mcAIHelper.mcNeighborList.front();

					// If this Neighbor is Dead
					if (cpSharkNeighbor == NULL || cpSharkNeighbor->GetState() == GiOBJECT_STATE_DEAD)
					{
						// Remove the Neighbor from the Neighbor list
						_cpObject->mcAIHelper.mcNeighborList.pop_front();
						cpSharkNeighbor = NULL;
					}
				}

				// If the Shark can sense a nearby fish
				float fSenseFishRangeSquared = _cpObject->mcAIHelper.GetSenseFishRange();
				fSenseFishRangeSquared *= fSenseFishRangeSquared;
				if (cpSharkNeighbor != NULL && _cpObject->mcpSceneNode->getWorldPosition().squaredDistance(cpSharkNeighbor->mcpSceneNode->getWorldPosition()) < fSenseFishRangeSquared)
				{
					// If the Shark is not already tracking this Fish
					if (_cpObject->GetState() != GiOBJECT_STATE_TRACKING_FISH ||
						_cpObject->GetIfObjectHasReachedAllDestinations() ||
						_cpObject->GetCurrentDestination() != cpSharkNeighbor->mcpSceneNode)
					{
						// Clear the Sharks Destination list
						_cpObject->mcDestinationList.clear();

						// Track this Fish
						SDestination* spNewDestination =  new SDestination(cpSharkNeighbor->mcpSceneNode);
						_cpObject->mcDestinationList.push_front(spNewDestination);
						_cpObject->GetNextDestination();

						// Set Sharks State
						_cpObject->SetState(GiOBJECT_STATE_TRACKING_FISH);
					}
				}
				// Else it can't sense any nearby fish, so go off exploring
				else
				{
					// If the Shark is not already exploring
					if (_cpObject->GetState() != GiOBJECT_STATE_EXPLORING ||
						_cpObject->GetIfObjectHasReachedAllDestinations())
					{
						// Clear the Sharks Destination list
						_cpObject->mcDestinationList.clear();

						// Give them a new random Destination to go to
						Vector3 sRandomDestination = Vector3::ZERO;
						sRandomDestination.x = Math::RangeRandom(GiLEVEL_LEFT, GiLEVEL_RIGHT);
						sRandomDestination.y = Math::RangeRandom(GiLEVEL_BOTTOM, GiLEVEL_TOP);
						sRandomDestination.z = Math::RangeRandom(GiLEVEL_BACK, GiLEVEL_FRONT);

						// Add the new Destination to the Object
						SDestination* spNewDestination =  new SDestination(NULL, &sRandomDestination);
						_cpObject->mcDestinationList.push_front(spNewDestination);
						_cpObject->GetNextDestination();

						// Set the Sharks State
						_cpObject->SetState(GiOBJECT_STATE_EXPLORING);
					}
				}
			}
		}
		// Else the Shark is hungry
		else
		{
			CObject* cpSharkNeighbor = NULL;

			// Loop through Sharks Neighbors until we find a valid one
			while (!_cpObject->mcAIHelper.mcNeighborList.empty() && cpSharkNeighbor == NULL)
			{
				// Get the Neighbor closest to the Shark
				cpSharkNeighbor = _cpObject->mcAIHelper.mcNeighborList.front();

				// If this Neighbor is Dead
				if (cpSharkNeighbor == NULL || cpSharkNeighbor->GetState() == GiOBJECT_STATE_DEAD)
				{
					// Remove the Neighbor from the Neighbor list
					_cpObject->mcAIHelper.mcNeighborList.pop_front();
					cpSharkNeighbor = NULL;
				}
			}

			// If the Shark can sense a nearby fish
			float fSenseFishRangeSquared = _cpObject->mcAIHelper.GetSenseFishRange();
			fSenseFishRangeSquared *= fSenseFishRangeSquared;
			if (cpSharkNeighbor != NULL && _cpObject->mcpSceneNode->getWorldPosition().squaredDistance(cpSharkNeighbor->mcpSceneNode->getWorldPosition()) < fSenseFishRangeSquared)
			{
				// If the Shark is not already tracking this Fish
				if (_cpObject->GetState() != GiOBJECT_STATE_TRACKING_FISH ||
					_cpObject->GetIfObjectHasReachedAllDestinations() ||
					_cpObject->GetCurrentDestination() != cpSharkNeighbor->mcpSceneNode)
				{
					// Clear the Sharks Destination list
					_cpObject->mcDestinationList.clear();

					// Track this Fish
					SDestination* spNewDestination =  new SDestination(cpSharkNeighbor->mcpSceneNode);
					_cpObject->mcDestinationList.push_front(spNewDestination);
					_cpObject->GetNextDestination();

					// Set Sharks State
					_cpObject->SetState(GiOBJECT_STATE_TRACKING_FISH);
				}
			}
			// Else it can't sense any nearby fish, so go off exploring
			else
			{
				// If the Shark is not already exploring
				if (_cpObject->GetState() != GiOBJECT_STATE_EXPLORING ||
					_cpObject->GetIfObjectHasReachedAllDestinations())
				{
					// Clear the Sharks Destination list
					_cpObject->mcDestinationList.clear();

					// Give them a new random Destination to go to
					Vector3 sRandomDestination = Vector3::ZERO;
					sRandomDestination.x = Math::RangeRandom(GiLEVEL_LEFT, GiLEVEL_RIGHT);
					sRandomDestination.y = Math::RangeRandom(GiLEVEL_BOTTOM, GiLEVEL_TOP);
					sRandomDestination.z = Math::RangeRandom(GiLEVEL_BACK, GiLEVEL_FRONT);

					// Add the new Destination to the Object
					SDestination* spNewDestination =  new SDestination(NULL, &sRandomDestination);
					_cpObject->mcDestinationList.push_front(spNewDestination);
					_cpObject->GetNextDestination();

					// Set the Sharks State
					_cpObject->SetState(GiOBJECT_STATE_EXPLORING);
				}
			}
		}
	}
	// Else this is a Non-Flocking Fish
	else
	{
		// If this Fish can spawn
		if (miNonFlockingFishCount < miInitialNonFlockingFishCount)
		{
			// Use some randomness to make sure the same Fish isn't doing all the spawning
			if ((int)Math::RangeRandom(1, 20) == 10)
			{
				// Spawn a new Non-Flocking Fish of the same type
				CObject* cpNewFish = CreateFish(false, _cpObject->GetSpecificObjectType());

				// Position and orient the new Fish the same as this one
				cpNewFish->mcpSceneNode->setPosition(_cpObject->mcpSceneNode->getWorldPosition());
				cpNewFish->PointObjectInDirection(_cpObject->GetDirectionObjectIsFacing());
			}
		}

		// If the Fish does not have a Destination to go to
		if (_cpObject->GetIfObjectHasReachedAllDestinations())
		{
			// Give them a new random Destination to go to
			Vector3 sRandomDestination = Vector3::ZERO;
			sRandomDestination.x = Math::RangeRandom(GiLEVEL_LEFT, GiLEVEL_RIGHT);
			sRandomDestination.y = Math::RangeRandom(GiLEVEL_BOTTOM, GiLEVEL_TOP);
			sRandomDestination.z = Math::RangeRandom(GiLEVEL_BACK, GiLEVEL_FRONT);

			// Add the new Destination to the Object
			SDestination* spNewDestination =  new SDestination(NULL, &sRandomDestination);
			_cpObject->mcDestinationList.push_front(spNewDestination);
			_cpObject->GetNextDestination();
		}
	}
}

// Perform collision detection between Player and other entities
void CGame::PerformCollisionDetection()
{
	// Temp local variables
	int iIndex = 0;									// Temp counting variable
	list<CObject>::iterator cObjectListIterator;	// Used to loop through the Object list
	float fRadius = 0.0f;							// Radius of the Object
	float fSharkCollisionDistance = 0.0f;			// Distance away from Object Shark must be to attack
	float fPlayerCollisionDistance = 0.0f;			// Distance away from Fish Player must be to capture it
	bool bObjectIsAnItem = false;
	
	// Store the Player and Sharks Bounding Radius
	float fPlayerRadius = mcpPlayer->mcpEntity->getBoundingRadius();
	float fSharkRadius = mcpShark->mcpEntity->getBoundingRadius();
	
	// Get the current Number of Objects
	int iNumberOfObjects = (int)mcObjectList.size();
	
	// Loop through all of the Objects and check for a collision
	cObjectListIterator = mcObjectList.begin();
	for (iIndex = 0; iIndex < iNumberOfObjects; iIndex++)
	{
		// If this Object is Dead
		if (cObjectListIterator->GetState() == GiOBJECT_STATE_DEAD)
		{
			// If this Object is a Ring
			if (cObjectListIterator->GetSpecificObjectType() == GiOBJECT_SPECIFIC_TYPE_RING)
			{
				// If the Ring was retrieved over 1 second ago
				if (cObjectListIterator->mcAIHelper.mcDegradationTimer.ReturnTimePassedSinceLastRestartUsingSavedTime() > 1000)
				{
					// Hide the Ring
					cObjectListIterator->mcpSceneNode->setVisible(false);

					// Delete all of the Rings children (other "world" Enemies)
					cObjectListIterator->mcpSceneNode->removeAndDestroyAllChildren();

					// Delete the Rings Entity and Scene Node
					cObjectListIterator->mcpSceneNode->detachObject(cObjectListIterator->mcpEntity);
					mcpSceneManager->destroyEntity(cObjectListIterator->mcpEntity);
					mcpSceneManager->destroySceneNode(cObjectListIterator->mcpSceneNode->getName());

					// Delete the Ring from the Object List (and move to the next Object in the List)
					cObjectListIterator = mcObjectList.erase(cObjectListIterator);
				}
				else
				{
					// Scale the Ring based on how much time has passed
					float fScaleFactor = 1.05f + mcFrameEvent.timeSinceLastFrame;
					cObjectListIterator->mcpSceneNode->scale(fScaleFactor, fScaleFactor, fScaleFactor);

					// Move to the next Object in the list
					cObjectListIterator++;
				}
			}
			// Else this Object is not a Ring
			else
			{
				// If this Object has been dead for long enough
				if (cObjectListIterator->mcAIHelper.mcDegradationTimer.XTimeHasPassedUsingSavedTime(3000))
				{
					// All Neighbor references to this Object should have been cleared out by now
					// so Delete the Object from the Object List (and move to the next Object in the list)
					cObjectListIterator = mcObjectList.erase(cObjectListIterator);
				}
			}

			// Move to the next Object
			continue;
		}

		// Determine the Sharks attack range based on if the Object is the Player or a Fish
		// If the Object is the Player
		if (cObjectListIterator->GetGeneralObjectType() == GiOBJECT_GENERAL_TYPE_PLAYER)
		{
			fSharkCollisionDistance = mcpShark->mcAIHelper.GetAttackRange();
		}
		// Else it is a Fish
		else
		{
			fSharkCollisionDistance = mcpShark->mcAIHelper.GetFeedRange();
		}

		// Get the Players Capture Range based on if this is an Item or not
		if (cObjectListIterator->GetGeneralObjectType() == GiOBJECT_GENERAL_TYPE_ITEM)
		{
			// Get the Players Item Capture range
			fPlayerCollisionDistance = mcpPlayer->mcAIHelper.GetFeedRange();

			// Record that this Object is an Item
			bObjectIsAnItem = true;
		}
		else
		{
			// Get the Players Fish Capture range
			fPlayerCollisionDistance = mcpPlayer->mcAIHelper.GetAttackRange();

			// Record that this Object is an Item
			bObjectIsAnItem = false;
		}

		// Calculate the Radius of the Object
		fRadius = cObjectListIterator->mcpEntity->getBoundingRadius();

		// If the Player is close enough to the Object for a collision AND
		//	this Object is not the Player or the Shark
		if (mcpPlayer->mcpSceneNode->getWorldPosition().distance(cObjectListIterator->mcpSceneNode->getWorldPosition()) < (fRadius + fPlayerRadius + fPlayerCollisionDistance) &&
			mcpPlayer->GetID() != cObjectListIterator->GetID() && mcpShark->GetID() != cObjectListIterator->GetID())
		{
			// If the Goal of the Level is to Collect Objects
			if (msLevelGoal.iGoal == GiLEVEL_GOAL_COLLECT)
			{
				// If this is the type of Object the Player is Collecting
				if (cObjectListIterator->GetSpecificObjectType() == msLevelGoal.iSpecificTypeToCollect)
				{
					// Increment the number of Objects the Player has Collected
					msLevelGoal.iNumberCollected++;
				}
			}

			// Get how many Points the Object was worth
			int iPointsEarned = cObjectListIterator->mcAttributes.ReturnNumberOfPoints();

			// If the Player earned enough points for an extra Health point
			if ((int)(mcpPlayer->mcAttributes.ReturnNumberOfPoints() / GiPOINTS_NEEDED_FOR_EXTRA_HEALTH) !=
				(int)((mcpPlayer->mcAttributes.ReturnNumberOfPoints() + iPointsEarned) / GiPOINTS_NEEDED_FOR_EXTRA_HEALTH))
			{
				// Increase the Players health by one point
				mcpPlayer->mcAttributes.SetHealthRelative(1);
			}

			// Add the Objects Points to the Players Points
			mcpPlayer->mcAttributes.SetNumberOfPointsRelative(iPointsEarned);

			// If we have a handle to the Player Got Points TextBox
			if (mcpPlayerGotPointsTextBox != NULL)
			{
				// Display a message based on how many Points were earned
				if (iPointsEarned < 200)
				{
					mcpPlayerGotPointsTextBox->SetAll("  Nice\n" + StringConverter::toString(iPointsEarned) + " Points", 0.35, 0.4);
				}
				else if (iPointsEarned < 400)
				{
					mcpPlayerGotPointsTextBox->SetAll("  Sweet\n" + StringConverter::toString(iPointsEarned) + " Points", 0.35, 0.4);
				}
				else if (iPointsEarned < 600)
				{
					mcpPlayerGotPointsTextBox->SetAll("  Great\n" + StringConverter::toString(iPointsEarned) + " Points", 0.35, 0.4);
				}
				else if (iPointsEarned < 800)
				{
					mcpPlayerGotPointsTextBox->SetAll("  Super\n" + StringConverter::toString(iPointsEarned) + " Points", 0.35, 0.4);
				}
				else
				{
					mcpPlayerGotPointsTextBox->SetAll("Holy Mackerel\n  " + StringConverter::toString(iPointsEarned) + " Points", 0.28, 0.4);
				}

				// Randomly pick and set the color to display the message in
				mcpPlayerGotPointsTextBox->SetColour(ColourValue(Math::UnitRandom(), Math::UnitRandom(), Math::UnitRandom()));
				
				// Display the message
				mcpPlayerGotPointsTextBox->Show(true);

				// Reset the Player Got Points Timer
				mcPlayerGotPointsTimer.RestartTimerUsingSavedTime();
			}

			// If this Object is a Fish
			if (cObjectListIterator->GetGeneralObjectType() == GiOBJECT_GENERAL_TYPE_ENEMY)
			{
				// If this is a Flocking Fish
				if (cObjectListIterator->mcAIHelper.GetIsAFlockingFish())
				{
					// Decrement the number of Flocking Fish left
					miFlockingFishCount--;
				}
				// Else it's not a Flocking Fish
				else
				{
					// Decrement the number of Non-Flocking Fish left
					miNonFlockingFishCount--;
				}

				// Hide the Object
				cObjectListIterator->mcpSceneNode->setVisible(false);

				// Delete all of the Objects children (other "world" Enemies)
				cObjectListIterator->mcpSceneNode->removeAndDestroyAllChildren();

				// Delete the Objects Entity and Scene Node
				cObjectListIterator->mcpSceneNode->detachObject(cObjectListIterator->mcpEntity);
				mcpSceneManager->destroyEntity(cObjectListIterator->mcpEntity);
				mcpSceneManager->destroySceneNode(cObjectListIterator->mcpSceneNode->getName());

				// Mark the Object to be deleted
				cObjectListIterator->SetState(GiOBJECT_STATE_DEAD);
				cObjectListIterator->mcAIHelper.mcDegradationTimer.RestartTimerUsingSavedTime();

				// Move to the next Object in the list
				cObjectListIterator++;
			}
			// Else if the Object the Player collided with is an Item
			else if (cObjectListIterator->GetGeneralObjectType() == GiOBJECT_GENERAL_TYPE_ITEM)
			{
				// Check which type of item the Player picked up
				switch (cObjectListIterator->GetSpecificObjectType())
				{
					default:
					case GiOBJECT_SPECIFIC_TYPE_RING:
						// Add 5 seconds to the time remaining
						mfLevelTime += 5.0;

						// Display that time was added to the level time remaining
						mcpPlayerGotPointsTextBox->SetAll("+5 seconds", 0.28, 0.4);

						// Randomly pick and set the color to display the message in
						mcpPlayerGotPointsTextBox->SetColour(ColourValue(Math::UnitRandom(), Math::UnitRandom(), Math::UnitRandom()));
					break;

					case GiOBJECT_SPECIFIC_TYPE_DIAMOND:
						// Increase the Players speed a bit
						mcpPlayer->SetMaxVelocitySpeed(mcpPlayer->GetMaxVelocitySpeed() + (1.0 * GiMETER));
					break;
				}

				// If this Item is a Ring
				if (cObjectListIterator->GetSpecificObjectType() == GiOBJECT_SPECIFIC_TYPE_RING)
				{
					// Mark the Ring as being Dead (since we want it to expand before dissapearing)
					cObjectListIterator->SetState(GiOBJECT_STATE_DEAD);
					cObjectListIterator->mcAIHelper.mcDegradationTimer.RestartTimerUsingSavedTime();

					// Move to the next Object in the list
					cObjectListIterator++;
				}
				else
				{
					// Hide the Object
					cObjectListIterator->mcpSceneNode->setVisible(false);

					// Delete all of the Objects children (other "world" Enemies)
					cObjectListIterator->mcpSceneNode->removeAndDestroyAllChildren();

					// Delete the Objects Entity and Scene Node
					cObjectListIterator->mcpSceneNode->detachObject(cObjectListIterator->mcpEntity);
					mcpSceneManager->destroyEntity(cObjectListIterator->mcpEntity);
					mcpSceneManager->destroySceneNode(cObjectListIterator->mcpSceneNode->getName());

					// Erase this Object from the list and move to the next Object in the list
					cObjectListIterator = mcObjectList.erase(cObjectListIterator);
				}
			}
			else
			{
				// Move to the next Object in the list
				cObjectListIterator++;
			}
		}
		// Else if the Shark is close enough to the Object to attack/feed, and the Object is not the Shark, and the Object isn't an Item
		else if (!bObjectIsAnItem && mcpShark->mcpSceneNode->getWorldPosition().distance(cObjectListIterator->mcpSceneNode->getWorldPosition()) < (fRadius + fSharkRadius + fSharkCollisionDistance) &&
				 mcpShark->GetID() != cObjectListIterator->GetID())
		{
			// If this Object is the Player AND the Shark hasn't just attacked the Player
			if (cObjectListIterator->GetGeneralObjectType() == GiOBJECT_GENERAL_TYPE_PLAYER &&
				mcpShark->mcAIHelper.mcHitDelayTimer.XTimeHasPassedUsingSavedTime(mcpShark->mcAIHelper.GetHitDelay()))
			{
				// Push Player in direction away from Shark at the speed the Shark was travelling
				Vector3 sAwayDirection = mcpPlayer->mcpSceneNode->getWorldPosition() - mcpShark->mcpSceneNode->getWorldPosition();
				sAwayDirection.normalise();
				mcpPlayer->SetVelocity(sAwayDirection, mcpShark->GetVelocitySpeed());

				// Reduce the Players Health
				mcpPlayer->mcAttributes.SetHealthRelative(-1);

				// Make the Player flash (an even amount of times) for as long as they are invincible to the Shark
				int iNumOfTimesToFlash = (mcpShark->mcAIHelper.GetHitDelay() / 400);
				mcpScheduler->ScheduleNewEvent(MakePlayerFlash, 0, this, iNumOfTimesToFlash * 2, 200);
				
				// Reset the Sharks Hit Delay Timer
				mcpShark->mcAIHelper.mcHitDelayTimer.RestartTimerUsingSavedTime();
			}
			// Else if this Object is a Fish
			else if (cObjectListIterator->GetGeneralObjectType() == GiOBJECT_GENERAL_TYPE_ENEMY)
			{
				// If this is a Flocking Fish
				if (cObjectListIterator->mcAIHelper.GetIsAFlockingFish())
				{
					// Decrement the number of Flocking Fish left
					miFlockingFishCount--;
				}
				// Else it's not a Flocking Fish
				else
				{
					// Decrement the number of Non-Flocking Fish left
					miNonFlockingFishCount--;
				}

				// Hide the Object
				cObjectListIterator->mcpSceneNode->setVisible(false);

				// Add the Objects Health to the Sharks Health
				mcpShark->mcAttributes.SetHealthRelative(cObjectListIterator->mcAttributes.ReturnHealth());

				// Remove this Object from the Sharks Neighbor list if it's in there
				mcpShark->mcAIHelper.mcNeighborList.remove(&*cObjectListIterator);

				// Delete all of the Objects children (other "world" Enemies)
				cObjectListIterator->mcpSceneNode->removeAndDestroyAllChildren();

				// Delete the Object Entity and Scene Node
				cObjectListIterator->mcpSceneNode->detachObject(cObjectListIterator->mcpEntity);
				mcpSceneManager->destroyEntity(cObjectListIterator->mcpEntity);
				mcpSceneManager->destroySceneNode(cObjectListIterator->mcpSceneNode->getName());

				// Mark the Object to be deleted
				cObjectListIterator->SetState(GiOBJECT_STATE_DEAD);
				cObjectListIterator->mcAIHelper.mcDegradationTimer.RestartTimerUsingSavedTime();

				// Move to the next Object in the list
				cObjectListIterator++;
			}
			else
			{
				// Move to the next Object in the list
				cObjectListIterator++;
			}
		}
		// Else the Player and Shark are not colliding with this Object
		else
		{
			// Move to the next Object in the list
			cObjectListIterator++;
		}
	}
}

// Update the Cameras Position and Orientation based on the mode being used
void CGame::UpdateCamera()
{
	// Temp variable used to tell how much time has passed since switching Camera Modes
	unsigned long ulTimeSinceRestart = 0;

	// If we just switched Camera Modes
	if (miCameraMode != miLastFramesCameraMode)
	{
		// Perform appropriate actions depending on which Camera Mode is now being used
		switch(miCameraMode)
		{
			// First Person Mode
			case GiCAMERA_MODE_FIRST_PERSON:
				mcCameraTightnessTimer.RestartTimerUsingSavedTime();
			break;

			// Third Person Fixed Mode
			case GiCAMERA_MODE_THIRD_PERSON_FIXED:
				// Make sure we are using the correct Tightness
				mcCameraSystem.SetCameraTightness(0.1);
				mcCameraSystem.SetTargetTightness(0.1);

				// Make sure we can see the Player
				mcpPlayer->mcpSceneNode->setVisible(true);
			break;

			// Third Person Chase Mode
			default:
			case GiCAMERA_MODE_THIRD_PERSON_CHASE:
				// Make sure we are using the correct Tightness
				mcCameraSystem.SetCameraTightness(mfCameraTightness);
				mcCameraSystem.SetTargetTightness(mfCameraTargetTightness);

				// Make sure we can see the Player
				mcpPlayer->mcpSceneNode->setVisible(true);
			break;
		};
	}

	// Save the Camera Mode used this frame
	miLastFramesCameraMode = miCameraMode;

	// Perform current Camera Modes actions
	switch(miCameraMode)
	{
		// First Person Mode
		case GiCAMERA_MODE_FIRST_PERSON:
			// Get how much time has passed since switching to First Person Camera Mode
			ulTimeSinceRestart = mcCameraTightnessTimer.ReturnTimePassedSinceLastRestartUsingSavedTime();

			// Have the Camera move in over the course of one second
			if (ulTimeSinceRestart < 1000)
			{
				// Set the Camera and Target Tightness based on how much time has passed
				mcCameraSystem.SetCameraTightness(UnitCubed(ulTimeSinceRestart / 1000.0));
				mcCameraSystem.SetTargetTightness(UnitCubed(ulTimeSinceRestart / 1000.0));
			}
			else
			{
				// Set the Camera and Target Tightness to full final values
				mcCameraSystem.SetCameraTightness(1.0);
				mcCameraSystem.SetTargetTightness(1.0);

				// Make the Player invisible so we can see through him in First Person Camera Mode
				mcpPlayer->mcpSceneNode->setVisible(false);
			}

			// Update the Camera and Target Positions
			mcCameraSystem.Update(mcpPlayer->mcpSceneNode->getWorldPosition(), mcpDesiredCameraTarget->getWorldPosition(), mcFrameEvent.timeSinceLastFrame);
		break;

		// Third Person Fixed Mode
		case GiCAMERA_MODE_THIRD_PERSON_FIXED:
			// Update the Camera and Target Positions
			mcCameraSystem.Update(mcCameraSystem.GetCameraPosition(), mcpDesiredCameraTarget->getWorldPosition(), mcFrameEvent.timeSinceLastFrame);
		break;

		// Third Person Chase Mode
		default:
		case GiCAMERA_MODE_THIRD_PERSON_CHASE:
			// If the Player is looking around
			if (mbLookingAround)
			{
				mcCameraSystem.SetCameraTightness(1.0);
				mcCameraSystem.SetTargetTightness(1.0);

				// Make the Camera look at the Player, since we want to rotate around them
				mcCameraSystem.Update(mcpDesiredCameraPosition->getWorldPosition(), mcpPlayer->mcpSceneNode->getWorldPosition(), mcFrameEvent.timeSinceLastFrame);
			}
			else
			{
				// Update the Camera and Target Positions
				mcCameraSystem.Update(mcpDesiredCameraPosition->getWorldPosition(), mcpDesiredCameraTarget->getWorldPosition(), mcFrameEvent.timeSinceLastFrame);
			}
		break;
	}

	// Update the Sharks Camera as well
	mcSharkCameraSystem.Update(mcpDesiredSharkCameraPosition->getWorldPosition(), mcpDesiredSharkCameraTarget->getWorldPosition(), mcFrameEvent.timeSinceLastFrame);


	// If the Player is no longer flashing from being attacked
	if (mcpScheduler->IsEmpty())
	{
		// If the camera is in 1st person view mode
		if (miCameraMode == GiCAMERA_MODE_FIRST_PERSON)
		{
			// If the Camera is not zooming in to where it should be
			if (mcCameraTightnessTimer.ReturnTimePassedSinceLastRestartUsingSavedTime() > 1000)
			{
				// Make sure the Player is not visible
				mcpPlayer->mcpSceneNode->setVisible(false);
			}
		}
		// Else the camera is in 3rd person view mode
		else
		{
			// Make sure the Player is visible
			mcpPlayer->mcpSceneNode->setVisible(true);
		}
	}
}

// Updates the time remaining to complete the level HUD
void CGame::UpdateHUDs(float _fTimeRemaining)
{
	// Create and display string showing the Players Health
	String sText = "Health " + StringConverter::toString(mcpPlayer->mcAttributes.ReturnHealth());
	mcpPlayersHealthTextBox->SetText(sText);

	// Create and display string showing how much time is remaining
	sText = "Time " + StringConverter::toString(Math::Ceil(_fTimeRemaining));
	mcpLevelTimeRemainingTextBox->SetText(sText);

	// Display how many Points the Player has
	sText = "Score " + StringConverter::toString(mcpPlayer->mcAttributes.ReturnNumberOfPoints());
	mcpPlayersPointsTextBox->SetText(sText);

	// If enough time has passed since the Player caught a fish
	int iDisplayMessageLength = 2000;	// Display message for 2 seconds
	if (mcPlayerGotPointsTimer.XTimeHasPassedUsingSavedTime((unsigned long)iDisplayMessageLength))
	{
		mcpPlayerGotPointsTextBox->Show(false);
	}
	// Else the Player recently caught a fish
	else
	{
		// Get how long it's been since the Player caught the fish
		int iTimeSinceCapture = (int)mcPlayerGotPointsTimer.ReturnTimePassedSinceLastRestartUsingSavedTime();

		// Get the Color of the text
		ColourValue sColor = mcpPlayerGotPointsTextBox->GetColour();

		// Make the message fade out based on time since catching the fish
		sColor.a = abs((float)(iTimeSinceCapture - iDisplayMessageLength) / (float)iDisplayMessageLength);

		// Display faded out text
		mcpPlayerGotPointsTextBox->SetColour(sColor);
	}

	// If the Level Objective should be shown
	if (mbShowObjective)
	{
		// String used to display how many Objects the Player has Collected so far
		String sCollectedSoFar = "";

		// If the Goal of the Level is to collect a number of items
		if (msLevelGoal.iGoal == GiLEVEL_GOAL_COLLECT)
		{
			// Build string showing how many Objects the Player has Collected so far
			sCollectedSoFar = "\n Collected " + StringConverter::toString(msLevelGoal.iNumberCollected) + " of " + StringConverter::toString(msLevelGoal.iRequiredValue);
		}

		// Display the Levels Objective
		mcpObjectiveTextBox->SetText(msLevelGoal.sInGameGoalDescription + sCollectedSoFar);
	}
	// Else show some Debug info instead
	else
	{
		// Update the Players shown Position if possible
		if (mcpObjectiveTextBox != NULL && mcpPlayer != NULL && mcpPlayer->mcpSceneNode != NULL && mcpShark != NULL)
		{
			// Store the Players current Position
			Vector3 sPlayerPosition = mcpPlayer->mcpSceneNode->getWorldPosition();

			// Store the Players Position
			String sText = "X:" + StringConverter::toString(sPlayerPosition.x) + " Y:" + StringConverter::toString(sPlayerPosition.y) + " Z:" + StringConverter::toString(sPlayerPosition.z);

			// Store the Shark info
			sText += "\nShark is ";
			switch (mcpShark->GetState())
			{
				default:
				case GiOBJECT_STATE_TRACKING_PLAYER:
					sText += "Tracking Player";
				break;

				case GiOBJECT_STATE_APPROACHING_PLAYER:
					sText += "Approaching Player";
				break;

				case GiOBJECT_STATE_EXPLORING:
					sText += "Exploring";
				break;

				case GiOBJECT_STATE_TRACKING_FISH:
					sText += "Tracking Fish";
				break;

				case GiOBJECT_STATE_AVOID_PLAYER:
					sText += "Avoiding Player";
				break;
			}
			sText += "  Health " + StringConverter::toString(mcpShark->mcAttributes.ReturnHealth());

			// Display number of Flocking and Non-Flocking Fish
			sText += "\nFlocking Fish " + StringConverter::toString(miFlockingFishCount) + "  Non-Flocking Fish " + StringConverter::toString(miNonFlockingFishCount);

			// Display the Text
			mcpObjectiveTextBox->SetText(sText);
		}
	}
}

// Shows or Hides the Debug Overlay (which displays the Frames Per Second)
bool CGame::ShowDebugHUD(bool _bShowDebugHUD)
{
	// If we have a handle to the Debug Overlay
	if (mcpDebugOverlay)
	{
		// If the FPS should be shown
		if (_bShowDebugHUD)
		{
			// Show the FPS
			UpdateDebugHUD();
			mcpDebugOverlay->show();
		}
		// Else the FPS should be hidden
		else
		{
			// Hide the FPS
			mcpDebugOverlay->hide();
		}
	}
	else
	{
		// Else return that the FPS are not being shown
		return false;
	}

	// Return if the FPS are now being shown or not
	return _bShowDebugHUD;
}

// Updates the info being shown on the Debug HUD
void CGame::UpdateDebugHUD()
{
	static String sCurrentFPS = "Current FPS: ";
	static String sAverageFPS = "Average FPS: ";
	static String sBestFPS = "Best FPS: ";
	static String sWorstFPS = "Worst FPS: ";
	static String sTriangleCount = "Triangle Count: ";

	// Update the Stats when necessary
	try 
	{
		// Get handles to each of the Overlay Elements
		OverlayElement* cpGUIAverageFPS = OverlayManager::getSingleton().getOverlayElement("Core/AverageFps");
		OverlayElement* cpGUICurrentFPS = OverlayManager::getSingleton().getOverlayElement("Core/CurrFps");
		OverlayElement* cpGUIBestFPS = OverlayManager::getSingleton().getOverlayElement("Core/BestFps");
		OverlayElement* cpGUIWorstFPS = OverlayManager::getSingleton().getOverlayElement("Core/WorstFps");
		OverlayElement* cpGUITriangleCount = OverlayManager::getSingleton().getOverlayElement("Core/NumTris");
		OverlayElement* cpGUIDebug = OverlayManager::getSingleton().getOverlayElement("Core/DebugText");
		OverlayElement* cpGUINumberOfBatches = OverlayManager::getSingleton().getOverlayElement("Core/NumBatches");
		OverlayElement* cpGUIOgreLogo = OverlayManager::getSingleton().getOverlayElement("Core/LogoPanel");

		// Get the Stats
		const RenderTarget::FrameStats& cStats = mcpRenderWindow->getStatistics();

		// Display the Stats in the appropriate Overlay Elements
		cpGUIAverageFPS->setCaption(sAverageFPS + StringConverter::toString(cStats.avgFPS));
		cpGUICurrentFPS->setCaption(sCurrentFPS + StringConverter::toString(cStats.lastFPS));
		cpGUIBestFPS->setCaption(sBestFPS + StringConverter::toString(cStats.bestFPS) + " " + StringConverter::toString(cStats.bestFrameTime) + "ms");
		cpGUIWorstFPS->setCaption(sWorstFPS + StringConverter::toString(cStats.worstFPS) + " " + StringConverter::toString(cStats.worstFrameTime) + "ms");
		cpGUITriangleCount->setCaption(sTriangleCount + StringConverter::toString(cStats.triangleCount));

		// Don't display the number of Batches or the Ogre Logo
		cpGUINumberOfBatches->hide();
		cpGUIOgreLogo->hide();
	}
	catch(...)
	{
		// Ignore any exceptions thrown
	}
}

// Toggles between the Players Camera, the Sharks Camera, and showing both Cameras
void CGame::UseCamera(int _iCamera)
{
	// Remove all Viewports from the Render Window
	mcpRenderWindow->removeAllViewports();
	mcpViewport2 = NULL;

	switch (_iCamera)
	{
		default:
		case 1:
			// Display the Players Camera
			mcpViewport = mcpRenderWindow->addViewport(mcpCamera);
			mcpViewport->setBackgroundColour(msBackgroundColor);
			mcpCamera->setAspectRatio(Real(mcpViewport->getActualWidth()) / Real(mcpViewport->getActualHeight()));
		break;

		case 2:
			// Display the Sharks Camera
			mcpViewport = mcpRenderWindow->addViewport(mcpSharkCamera);
			mcpViewport->setBackgroundColour(msBackgroundColor);
			mcpSharkCamera->setAspectRatio(Real(mcpViewport->getActualWidth()) / Real(mcpViewport->getActualHeight()));
		break;

		// Display both the Players and Sharks Cameras
		case 0:
			// Display Players Camera on left side of screen
			mcpViewport = mcpRenderWindow->addViewport(mcpCamera, 0, 0, 0, 1.0, 0.5);
			mcpViewport->setBackgroundColour(msBackgroundColor);
			mcpCamera->setAspectRatio(Real(mcpViewport->getActualWidth()) / Real(mcpViewport->getActualHeight()));

			// Display Sharks Camera on right side of screen
			mcpViewport2 = mcpRenderWindow->addViewport(mcpSharkCamera, 1, 0, 0.5, 1.0, 0.5);
			mcpViewport2->setBackgroundColour(msBackgroundColor);
			mcpSharkCamera->setAspectRatio(Real(mcpViewport->getActualWidth()) / Real(mcpViewport->getActualHeight()));
		break;
	}
}

// Reads in a configuration file and sets the appropriate variables
bool CGame::ReadInConfigurationFile(string _sFileName, bool _bOnlyGetGoalVariables)
{
	ifstream fInFile(_sFileName.c_str());
	string sBuffer;
	const int iJUNK_BUFFER_SIZE = 500;
	char caJunkBuffer[iJUNK_BUFFER_SIZE];
	int iLineNumber = 0;

	// If the File couldn't be opened
	if (!fInFile.is_open())
	{
		WriteToLogFile("ERROR: Could not open " + _sFileName);
		return false;
	}

	// If we do not have handles to the Player and Shark Objects
	if ((mcpPlayer == NULL || mcpShark == NULL) && !_bOnlyGetGoalVariables)
	{
		WriteToLogFile("ERROR: No handles to Player or Shark when reading configuration file " + _sFileName);
		return false;
	}

	// Loop until the entire file has been read
	while (!fInFile.eof())
	{
		// Read in the first word of the line
		fInFile >> sBuffer;
		
		// Increment the Line Number we are on
		iLineNumber++;

		// If this is not a comment line
		if (sBuffer.substr(0, 2) != "//")
		{
			// If we are only interested in the Goal and Time Limit, and we aren't currently reading them in
			if (_bOnlyGetGoalVariables && sBuffer != "LevelGoal:" && sBuffer != "LevelGoalDescription:" && 
				sBuffer != "LevelStory:")
			{
				// Do nothing
			}
			// If we are setting the Levels Goal
			else if (sBuffer == "LevelGoal:")
			{
				fInFile >> sBuffer;

				// If the Levels goal is to achieve a certain Score
				if (sBuffer == "score")
				{
					msLevelGoal.iGoal = GiLEVEL_GOAL_SCORE;

					fInFile >> sBuffer;
					msLevelGoal.iRequiredValue = atoi(sBuffer.c_str());
				}
				// If the Levels goal is to Collect a certain amount of Objects
				else if (sBuffer == "collect")
				{
					msLevelGoal.iGoal = GiLEVEL_GOAL_COLLECT;

					fInFile >> sBuffer;
					msLevelGoal.iRequiredValue = atoi(sBuffer.c_str());

					fInFile >> sBuffer;
					msLevelGoal.iSpecificTypeToCollect = atoi(sBuffer.c_str());
				}
				// If the Levels goal is to survive for a duration of time
				else if (sBuffer == "survive")
				{
					msLevelGoal.iGoal = GiLEVEL_GOAL_SURVIVE;

					fInFile >> sBuffer;
					msLevelGoal.iRequiredValue = (int)atof(sBuffer.c_str());
				}
				// Else there is no goal
				else
				{
					msLevelGoal.iGoal = GiLEVEL_GOAL_NONE;
				}
			}
			else if (sBuffer == "LevelGoalDescription:")
			{
				// Temp buffer for reading in line of file
				char caBuffer[500];

				// Read in the rest of the line to save it as the Goal Description
				fInFile.getline(caBuffer, 500, '\n');

				// Save the Goals Description
				msLevelGoal.sGoalDescription = caBuffer;

				// Find and replace all "\n"s in the string with '\n'
				int iIndex = 0;
				while ((iIndex = (int)msLevelGoal.sGoalDescription.find("\\n", iIndex)) != String::npos)
				{
					msLevelGoal.sGoalDescription.replace(iIndex, 2, 1, '\n');
					iIndex++;
				}
			}
			else if (sBuffer == "LevelGoalInGameDescription:")
			{
				// Temp buffer for reading in line of file
				char caBuffer[500];

				// Read in the rest of the line to save it as the Goal Description
				fInFile.getline(caBuffer, 500, '\n');

				// Save the Goals Description
				msLevelGoal.sInGameGoalDescription = caBuffer;

				// Find and replace all "\n"s in the string with '\n'
				int iIndex = 0;
				while ((iIndex = (int)msLevelGoal.sInGameGoalDescription.find("\\n", iIndex)) != String::npos)
				{
					msLevelGoal.sInGameGoalDescription.replace(iIndex, 2, 1, '\n');
					iIndex++;
				}
			}
			else if (sBuffer == "LevelStory:")
			{
				// Temp buffer for reading in line of file
				char caBuffer[500];

				// Read in the rest of the line to save it as the Goal Description
				fInFile.getline(caBuffer, 500, '\n');

				// Save the Goals Description
				msLevelGoal.sLevelStory = caBuffer;

				// Find and replace all "\n"s in the string with '\n'
				int iIndex = 0;
				while ((iIndex = (int)msLevelGoal.sLevelStory.find("\\n", iIndex)) != String::npos)
				{
					msLevelGoal.sLevelStory.replace(iIndex, 2, 1, '\n');
					iIndex++;
				}
			}
			else if (sBuffer == "LevelGoalAccomplishedMessage:")
			{
				// Temp buffer for reading in line of file
				char caBuffer[500];

				// Read in the rest of the line to save it as the Goal Description
				fInFile.getline(caBuffer, 500, '\n');

				// Save the Goals Description
				msLevelGoal.sGoalAccomplishedMessage = caBuffer;

				// Find and replace all "\n"s in the string with '\n'
				int iIndex = 0;
				while ((iIndex = (int)msLevelGoal.sGoalAccomplishedMessage.find("\\n", iIndex)) != String::npos)
				{
					msLevelGoal.sGoalAccomplishedMessage.replace(iIndex, 2, 1, '\n');
					iIndex++;
				}
			}
			// If we are setting the Time Limit
			else if (sBuffer == "TimeLimit:")
			{
				fInFile >> sBuffer;
				mfLevelTime = atof(sBuffer.c_str());
				mbLevelHasTimeLimit = (mfLevelTime > 0.0) ? true : false;
			}
			// If we are creating new Objects
			else if (sBuffer == "CreateMultipleObjects:")
			{
				int iNumToCreate = 0;			// Holds the number of Objects to create
				int iSpecificObjectType = 0;	// Holds the specific type of Object to create
				int iIndex = 0;					// Counting variable

				fInFile >> sBuffer;
				iNumToCreate = atoi(sBuffer.c_str());

				fInFile >> sBuffer;
				iSpecificObjectType = atoi(sBuffer.c_str());

				// Loop through and create the specified number of Objects
				for (iIndex = 0; iIndex < iNumToCreate; iIndex++)
				{
					// If we should be creating a Fish
					if (iSpecificObjectType >= GiOBJECT_SPECIFIC_TYPE_FISH_INDEX_START &&
						iSpecificObjectType <= GiOBJECT_SPECIFIC_TYPE_FISH_INDEX_END)
					{
						// Create the Fish
						CreateFish(false, iSpecificObjectType);
					}
					// Else if we should be creating an Item
					else
					{
						// Create the Item
						CreateItem(iSpecificObjectType);
					}
				}
			}
			// If we are creating a new Object
			else if (sBuffer == "CreateObject:")
			{
				CObject* cpObject = NULL;			// Pointer to the created Object
				int iSpecificObjectType = 0;		// Holds the specific type of Object to create
				Vector3 sPosition = Vector3::ZERO;	// Holds where to position the Object
				Vector3 sRotation = Vector3::ZERO;	// Holds how to orient the Object
				string sBuffer2, sBuffer3;			// String buffers to read in more data at once

				fInFile >> sBuffer;
				iSpecificObjectType = atoi(sBuffer.c_str());

				fInFile >> sBuffer >> sBuffer2 >> sBuffer3;
				sPosition = Vector3(atof(sBuffer.c_str()) * GiMETER, atof(sBuffer2.c_str()) * GiMETER, atof(sBuffer3.c_str()) * GiMETER);

				fInFile >> sBuffer >> sBuffer2 >> sBuffer3;
				sRotation = Vector3(atof(sBuffer.c_str()), atof(sBuffer2.c_str()), atof(sBuffer3.c_str()));

				// If we should be creating a Fish
				if (iSpecificObjectType >= GiOBJECT_SPECIFIC_TYPE_FISH_INDEX_START &&
					iSpecificObjectType <= GiOBJECT_SPECIFIC_TYPE_FISH_INDEX_END)
				{
					// Create the Fish
					cpObject = CreateFish(false, iSpecificObjectType);
				}
				// Else if we should be creating an Item
				else
				{
					// Create the Item
					cpObject = CreateItem(iSpecificObjectType);
				}

				// Position the Object
				cpObject->mcpSceneNode->setPosition(sPosition);

				// Orient the Object
				cpObject->mcpSceneNode->pitch(Radian(Degree(sRotation.x)));
				cpObject->mcpSceneNode->yaw(Radian(Degree(sRotation.y)));
				cpObject->mcpSceneNode->roll(Radian(Degree(sRotation.z)));
			}
			// If we are setting the Cameras Tightness
			else if (sBuffer == "CameraTightness:")
			{
				fInFile >> sBuffer;
				mfCameraTightness = atof(sBuffer.c_str());
			}
			// If we are setting the Camera Targets Tightness
			else if (sBuffer == "CameraTargetTightness:")
			{
				fInFile >> sBuffer;
				mfCameraTargetTightness = atof(sBuffer.c_str());
			}
			// If we are setting the Camera Targets Max Offset Angle allowed
			else if (sBuffer == "CameraTargetMaxOffsetAngle:")
			{
				fInFile >> sBuffer;
				mfCameraTargetMaxOffsetAngle = atof(sBuffer.c_str());
				mcCameraSystem.SetMaxTargetOffsetAngle(mfCameraTargetMaxOffsetAngle);
			}
			// If we are setting the type of Lighting to use
			else if (sBuffer == "LightingMode:")
			{
				int iMode = 0;

				fInFile >> sBuffer;
				iMode = atoi(sBuffer.c_str());

				switch (iMode)
				{
					default:
					case 1:
						// Use the fast but lower quality lighting mode
						mcpSceneManager->setShadowTechnique(SHADOWTYPE_TEXTURE_MODULATIVE);
					break;

					case 2:
						// Use the medium speed, medium quality lighting mode
						mcpSceneManager->setShadowTechnique(SHADOWTYPE_STENCIL_MODULATIVE);
					break;

					case 3:
						// Use the slow but good quality lighting mode
						mcpSceneManager->setShadowTechnique(SHADOWTYPE_STENCIL_ADDITIVE);
					break;
				}
			}
			// If we are setting the Players initial amount of Points
			else if (sBuffer == "PlayerInitialPoints:")
			{
				fInFile >> sBuffer;
				mcpPlayer->mcAttributes.SetHealth(atoi(sBuffer.c_str()));
			}
			// If we are setting how close the Player must be to a fish to catch it
			else if (sBuffer == "PlayerCaptureFishRange:")
			{
				fInFile >> sBuffer;
				mcpPlayer->mcAIHelper.SetAttackRange(atof(sBuffer.c_str()) * GiMETER);
			}
			// If we are setting how close the Player must be to an Item to get it
			else if (sBuffer == "PlayerGetItemRange:")
			{
				fInFile >> sBuffer;
				mcpPlayer->mcAIHelper.SetFeedRange(atof(sBuffer.c_str()) * GiMETER);
			}
			// If we are settings the Players max speed
			else if (sBuffer == "PlayerMaxSpeed:")
			{
				fInFile >> sBuffer;
				mcpPlayer->SetMaxVelocitySpeed(((mcpPlayer->GetFriction() / GiMETER) + atof(sBuffer.c_str())) * GiMETER);
			}
			// If we are setting the number of Flocking Fish to use
			else if (sBuffer == "FlockingFishInitialCount:")
			{
				fInFile >> sBuffer;
				miInitialFlockingFishCount = atoi(sBuffer.c_str());
			}
			// If we are setting the number of Non-Flocking Fish to use
			else if (sBuffer == "NonFlockingFishInitialCount:")
			{
				fInFile >> sBuffer;
				miInitialNonFlockingFishCount = atoi(sBuffer.c_str());
			}
			// If we are specifying whether to allow Flocking between different types of fish
			else if (sBuffer == "AllowMixedFlocking:")
			{
				fInFile >> sBuffer;
				mbAllowMixedFlocking = (sBuffer == "true") ? true : false;
			}
			// If we are setting how old a fish must be to spawn
			else if (sBuffer == "FishSpawnAge:")
			{
				fInFile >> sBuffer;
				mcDefaultFlockingFish.mcAIHelper.SetSpawnAge(atof(sBuffer.c_str()));
			}
			// If we are setting how far away a fish can detect the Shark from
			else if (sBuffer == "FishSenseSharkRange:")
			{
				fInFile >> sBuffer;
				mcDefaultFlockingFish.mcAIHelper.SetSenseSharkRange(atof(sBuffer.c_str()) * GiMETER);
			}
			// If we are setting how far away a fish can detect the Player from
			else if (sBuffer == "FishSenseSubmarineRange:")
			{
				fInFile >> sBuffer;
				mcDefaultFlockingFish.mcAIHelper.SetSenseSubmarineRange(atof(sBuffer.c_str()) * GiMETER);
			}
			// If we are setting how far away a fish can detect another fish from
			else if (sBuffer == "FishSenseFishRange:")
			{
				fInFile >> sBuffer;
				mcDefaultFlockingFish.mcAIHelper.SetSenseFishRange(atof(sBuffer.c_str()) * GiMETER);
			}
			// If we are setting the Max number of neighbors a fish can have
			else if (sBuffer == "FishMaxNeighborCount:")
			{
				fInFile >> sBuffer;
				mcDefaultFlockingFish.mcAIHelper.SetMaxNumberOfNeighbors(atoi(sBuffer.c_str()));
				mcpShark->mcAIHelper.SetMaxNumberOfNeighbors(mcDefaultFlockingFish.mcAIHelper.GetMaxNumberOfNeighbors());
			}
			// If we are setting the distance between fish for them to be considered neighbors
			else if (sBuffer == "FishSenseNeighborRange:")
			{
				fInFile >> sBuffer;
				mcDefaultFlockingFish.mcAIHelper.SetSenseNeighborRange(atof(sBuffer.c_str()) * GiMETER);
			}
			else if (sBuffer == "FishSeparationStrength:")
			{
				fInFile >> sBuffer;
				mcDefaultFlockingFish.mcAIHelper.SetSeparationCoefficient(atof(sBuffer.c_str()));
			}
			else if (sBuffer == "FishAlignmentStrength:")
			{
				fInFile >> sBuffer;
				mcDefaultFlockingFish.mcAIHelper.SetAlignmentCoefficient(atof(sBuffer.c_str()));
			}
			else if (sBuffer == "FishCohesionStrength:")
			{
				fInFile >> sBuffer;
				mcDefaultFlockingFish.mcAIHelper.SetCohesionCoefficient(atof(sBuffer.c_str()));
			}
			else if (sBuffer == "FishPreditorAvoidanceStrength:")
			{
				fInFile >> sBuffer;
				mcDefaultFlockingFish.mcAIHelper.SetPreditorAvoidanceCoefficient(atof(sBuffer.c_str()));
			}
			else if (sBuffer == "SharkInitialPoints:")
			{
				fInFile >> sBuffer;
				mcpShark->mcAttributes.SetHealth(atoi(sBuffer.c_str()));
			}
			else if (sBuffer == "SharkMinimumPoints:")
			{
				fInFile >> sBuffer;
				mcpShark->mcAttributes.SetMinHealth(atoi(sBuffer.c_str()));
			}
			else if (sBuffer == "SharkFeedPoints:")
			{
				fInFile >> sBuffer;
				mcDefaultFlockingFish.mcAttributes.SetHealth(atoi(sBuffer.c_str()));
			}
			else if (sBuffer == "SharkNourishedPoints:")
			{
				fInFile >> sBuffer;
				mcpShark->mcAIHelper.SetNourishedHealth(atoi(sBuffer.c_str()));
			}
			else if (sBuffer == "SharkFullBellyPoints:")
			{
				fInFile >> sBuffer;
				mcpShark->mcAIHelper.SetFullHealth(atoi(sBuffer.c_str()));
			}
			else if (sBuffer == "SharkHealthDegradeDelay:")
			{
				fInFile >> sBuffer;
				mcpShark->mcAIHelper.SetHealthDegradeDelay((unsigned long)(atof(sBuffer.c_str())* 1000.0f));
			} 
			else if (sBuffer == "SharkSenseFishRange:")
			{
				fInFile >> sBuffer;
				mcpShark->mcAIHelper.SetSenseFishRange(atof(sBuffer.c_str()) * GiMETER);
			}
			else if (sBuffer == "SharkSenseSubmarineRange:")
			{
				fInFile >> sBuffer;
				mcpShark->mcAIHelper.SetSenseSubmarineRange(atof(sBuffer.c_str()) * GiMETER);
			}
			else if (sBuffer == "SharkFeedRange:")
			{
				fInFile >> sBuffer;
				mcpShark->mcAIHelper.SetFeedRange(atof(sBuffer.c_str()) * GiMETER);
			}
			else if (sBuffer == "SharkAttackRange:")
			{
				fInFile >> sBuffer;
				mcpShark->mcAIHelper.SetAttackRange(atof(sBuffer.c_str()) * GiMETER);
			}
			else if (sBuffer == "SharkVelocityFraction:")
			{
				fInFile >> sBuffer;
				mcpShark->mcAIHelper.SetVelocityCoefficient(atof(sBuffer.c_str()));
			}
			else if (sBuffer == "SharkHitDelay:")
			{
				fInFile >> sBuffer;
				mcpShark->mcAIHelper.SetHitDelay((unsigned long)(atof(sBuffer.c_str()) * 1000.0f));
			}
			// Else this is an unrecognized command
			else
			{
				WriteToLogFile("ERROR: Unrecognized command (" + sBuffer + ") in " + _sFileName + " at line number " + StringConverter::toString(iLineNumber));
			}
		}

		// Read in and ignore the rest of the line
		fInFile.getline(caJunkBuffer, iJUNK_BUFFER_SIZE, '\n');
	}
	
	// Close the File now that we're done reading it
	fInFile.close();

	// Return success
	return true;
}

// Creates an Object of the specified type
void CGame::CreateObject(CObject& _cObject)
{
	Vector3 sEnemyPosition = Vector3::ZERO;			// Used to hold the Enemy's initial position
	float fEnemyLengthInMeters = 0.0f;				// How big the Enemy's Length should be in Meters
	Vector3 sEnemySize = Vector3::ZERO;				// Holds the Enemy Meshes size
	float fEnemySizeToMeterRatio = 0.0f;			// Holds the Enemy Mesh to Meter size ratio 
	Vector3 sEnemyDimensionsScale = Vector3::ZERO;	// How much to Scale the Enemies Dimensions
	ColourValue sEnemyColor;						// Holds the Enemies colour
	bool bUseRandomColoredMaterial = false;			// Holds if the custom material should be applied to the Enemy or not
	String sEnemyMesh;								// Holds the name of the Mesh the Object should use
	float fEnemySpeed = 0.0f;						// Holds how fast the Enemy should move
	float fEnemyRotationSpeed = 0.0f;				// Holds how fast the Enemy should rotate
	bool bRandomlyOrientObject = true;				// Holds whether to randomly orient the Object or not
	bool bUseAutoTracking = true;					// Holds whether to use Auto Tracking or not

	// Create the Object Scene Node
	_cObject.mcpSceneNode = mcpSceneManager->getRootSceneNode()->createChildSceneNode(_cObject.GetName() + "SceneNode");

	// Create an Object of the specified type
	bUseRandomColoredMaterial = false;
	switch(_cObject.GetSpecificObjectType())
	{
		case GiOBJECT_SPECIFIC_TYPE_SHARK:
			// Specify the Enemy Mesh to use
			sEnemyMesh = "Shark.mesh";

			// Specify Enemy's Size (Length) in Meters
			fEnemyLengthInMeters = 6.0;

			// Randomly pick an amount to scale the Enemies Dimensions by
			sEnemyDimensionsScale.x = 1.0;
			sEnemyDimensionsScale.y = 1.0;
			sEnemyDimensionsScale.z = 1.0;
			
			// Specify Enemies Speed (Sharks speed is manipulated by how much Health it has)
			fEnemySpeed = ((mfLevelFriction / GiMETER) + 10.0) * GiMETER;
			_cObject.SetDefaultAccelerationRate(fEnemySpeed);
			_cObject.SetMaxVelocitySpeed(fEnemySpeed);
			_cObject.SetMinVelocitySpeed(0.0);

			// Specify Enemies Rotation Speed
			fEnemyRotationSpeed = Math::RangeRandom(70.0, 130.0);
			_cObject.SetDefaultRotationVelocitySpeed(fEnemyRotationSpeed);
			_cObject.SetMaxRotationVelocitySpeed(fEnemyRotationSpeed);
			_cObject.SetMinRotationVelocitySpeed(0.0);
			_cObject.SetRotationVelocity(Vector3::UNIT_X, fEnemyRotationSpeed);

			// Specify which directions the Enemy mesh faces by default
			_cObject.SetMeshesDefaultForwardDirection(Vector3::UNIT_Z);
			_cObject.SetMeshesDefaultUpDirection(Vector3::UNIT_Y);
			_cObject.SetMeshesDefaultRightDirection(Vector3::UNIT_X);

			// Specify how many Points this Enemy is worth
			_cObject.mcAttributes.SetNumberOfPoints(0);
		break;

		default:
		case GiOBJECT_SPECIFIC_TYPE_DANS_FISH:
			// Specify the Enemy Mesh to use
			sEnemyMesh = "DansFish.mesh";
			bUseRandomColoredMaterial = true;

			// Specify Enemy's Size (Length) in Meters
			fEnemyLengthInMeters = 1.5;

			// Randomly pick an amount to scale the Enemies Dimensions by
			sEnemyDimensionsScale.x = Math::RangeRandom(0.75, 1.25);
			sEnemyDimensionsScale.y = Math::RangeRandom(0.75, 1.25);
			sEnemyDimensionsScale.z = Math::RangeRandom(0.75, 1.25);

			// Specify Enemies Speed
			fEnemySpeed = Math::RangeRandom((mfLevelFriction / GiMETER) + 0.5, (mfLevelFriction / GiMETER) + 2.0) * GiMETER;
			_cObject.SetDefaultAccelerationRate(fEnemySpeed);
			_cObject.SetMaxVelocitySpeed(fEnemySpeed);
			_cObject.SetMinVelocitySpeed(0.0);

			// Specify Enemies Rotation Speed
			fEnemyRotationSpeed = Math::RangeRandom(30.0, 180.0);
			_cObject.SetDefaultRotationVelocitySpeed(fEnemyRotationSpeed);
			_cObject.SetMaxRotationVelocitySpeed(fEnemyRotationSpeed);
			_cObject.SetMinRotationVelocitySpeed(0.0);
			_cObject.SetRotationVelocity(Vector3::UNIT_X, fEnemyRotationSpeed);

			// Specify which directions the Enemy mesh faces by default
			_cObject.SetMeshesDefaultForwardDirection(Vector3::UNIT_Z);
			_cObject.SetMeshesDefaultUpDirection(Vector3::UNIT_Y);
			_cObject.SetMeshesDefaultRightDirection(Vector3::UNIT_X);

			// Specify how many Points this Enemy is worth
			_cObject.mcAttributes.SetNumberOfPoints(200);
		break;

		case GiOBJECT_SPECIFIC_TYPE_DANS_OCTOPUS:
			// Specify the Enemy Mesh to use
			sEnemyMesh = "DansOctopus.mesh";
			bUseRandomColoredMaterial = true;

			// Specify Enemy's Size (Length) in Meters
			fEnemyLengthInMeters = 2.5;

			// Randomly pick an amount to scale the Enemies Dimensions by
			sEnemyDimensionsScale.x = Math::RangeRandom(0.75, 1.25);
			sEnemyDimensionsScale.y = Math::RangeRandom(0.75, 1.25);
			sEnemyDimensionsScale.z = Math::RangeRandom(0.75, 1.25);

			// Specify Enemies Speed
			fEnemySpeed = Math::RangeRandom((mfLevelFriction / GiMETER) + 4.0, (mfLevelFriction / GiMETER) + 6.0) * GiMETER;
			_cObject.SetDefaultAccelerationRate(fEnemySpeed);
			_cObject.SetMaxVelocitySpeed(fEnemySpeed);
			_cObject.SetMinVelocitySpeed(0.0);

			// Specify Enemies Rotation Speed
			fEnemyRotationSpeed = Math::RangeRandom(30.0, 180.0);
			_cObject.SetDefaultRotationVelocitySpeed(fEnemyRotationSpeed);
			_cObject.SetMaxRotationVelocitySpeed(fEnemyRotationSpeed);
			_cObject.SetMinRotationVelocitySpeed(0.0);
			_cObject.SetRotationVelocity(Vector3::UNIT_X, fEnemyRotationSpeed);

			// Specify which directions the Enemy mesh faces by default
			_cObject.SetMeshesDefaultForwardDirection(Vector3::UNIT_Z);
			_cObject.SetMeshesDefaultUpDirection(Vector3::UNIT_Y);
			_cObject.SetMeshesDefaultRightDirection(Vector3::UNIT_X);

			// Specify how many Points this Enemy is worth
			_cObject.mcAttributes.SetNumberOfPoints(450);
		break;

		case GiOBJECT_SPECIFIC_TYPE_BLUE_TANGO_FISH:
			// Specify the Enemy Mesh to use
			sEnemyMesh = "BlueTangoFish.mesh";
			bUseRandomColoredMaterial = true;

			// Specify Enemy's Size (Length) in Meters
			fEnemyLengthInMeters = 1.7;

			// Randomly pick an amount to scale the Enemies Dimensions by
			sEnemyDimensionsScale.x = Math::RangeRandom(0.75, 1.25);
			sEnemyDimensionsScale.y = Math::RangeRandom(0.75, 1.25);
			sEnemyDimensionsScale.z = Math::RangeRandom(0.75, 1.25);

			// Specify Enemies Speed
			fEnemySpeed = Math::RangeRandom((mfLevelFriction / GiMETER) + 2.5, (mfLevelFriction / GiMETER) + 3.0) * GiMETER;
			_cObject.SetDefaultAccelerationRate(fEnemySpeed);
			_cObject.SetMaxVelocitySpeed(fEnemySpeed);
			_cObject.SetMinVelocitySpeed(0.0);

			// Specify Enemies Rotation Speed
			fEnemyRotationSpeed = Math::RangeRandom(30.0, 100.0);
			_cObject.SetDefaultRotationVelocitySpeed(fEnemyRotationSpeed);
			_cObject.SetMaxRotationVelocitySpeed(fEnemyRotationSpeed);
			_cObject.SetMinRotationVelocitySpeed(0.0);
			_cObject.SetRotationVelocity(Vector3::UNIT_X, fEnemyRotationSpeed);

			// Specify which direction the Enemy mesh faces by default
			_cObject.SetMeshesDefaultForwardDirection(Vector3::NEGATIVE_UNIT_X);
			_cObject.SetMeshesDefaultUpDirection(Vector3::UNIT_Y);
			_cObject.SetMeshesDefaultRightDirection(Vector3::NEGATIVE_UNIT_Z);

			// Specify how many Points this Enemy is worth
			_cObject.mcAttributes.SetNumberOfPoints(350);
		break;

		case GiOBJECT_SPECIFIC_TYPE_TURTLE:
			// Specify the Enemy Mesh to use
			sEnemyMesh = "Turtle.mesh";

			// Specify Enemy's Size (Length) in Meters
			fEnemyLengthInMeters = 3.0;

			// Randomly pick an amount to scale the Enemies Dimensions by
			sEnemyDimensionsScale.x = Math::RangeRandom(0.75, 1.25);
			sEnemyDimensionsScale.y = Math::RangeRandom(0.75, 1.25);
			sEnemyDimensionsScale.z = Math::RangeRandom(0.75, 1.25);
			
			// Specify Enemies Speed
			fEnemySpeed = Math::RangeRandom((mfLevelFriction / GiMETER) + 0.25, (mfLevelFriction / GiMETER) + 1.0) * GiMETER;
			_cObject.SetDefaultAccelerationRate(fEnemySpeed);
			_cObject.SetMaxVelocitySpeed(fEnemySpeed);
			_cObject.SetMinVelocitySpeed(0.0);

			// Specify Enemies Rotation Speed
			fEnemyRotationSpeed = Math::RangeRandom(30.0, 60.0);
			_cObject.SetDefaultRotationVelocitySpeed(fEnemyRotationSpeed);
			_cObject.SetMaxRotationVelocitySpeed(fEnemyRotationSpeed);
			_cObject.SetMinRotationVelocitySpeed(0.0);
			_cObject.SetRotationVelocity(Vector3::UNIT_X, fEnemyRotationSpeed);

			// Specify which directions the Enemy mesh faces by default
			_cObject.SetMeshesDefaultForwardDirection(Vector3::UNIT_Z);
			_cObject.SetMeshesDefaultUpDirection(Vector3::UNIT_Y);
			_cObject.SetMeshesDefaultRightDirection(Vector3::UNIT_X);

			// Specify how many Points this Enemy is worth
			_cObject.mcAttributes.SetNumberOfPoints(125);
		break;

		case GiOBJECT_SPECIFIC_TYPE_BLUE_FISH:
			// Specify the Enemy Mesh to use
			sEnemyMesh = "BlueFish.mesh";

			// Specify Enemy's Size (Length) in Meters
			fEnemyLengthInMeters = 1.2;

			// Randomly pick an amount to scale the Enemies Dimensions by
			sEnemyDimensionsScale.x = Math::RangeRandom(0.75, 1.25);
			sEnemyDimensionsScale.y = Math::RangeRandom(0.75, 1.25);
			sEnemyDimensionsScale.z = Math::RangeRandom(0.75, 1.25);
			
			// Specify Enemies Speed
			fEnemySpeed = Math::RangeRandom((mfLevelFriction / GiMETER) + 4.0, (mfLevelFriction / GiMETER) + 5.0) * GiMETER;
			_cObject.SetDefaultAccelerationRate(fEnemySpeed);
			_cObject.SetMaxVelocitySpeed(fEnemySpeed);
			_cObject.SetMinVelocitySpeed(0.0);

			// Specify Enemies Rotation Speed
			fEnemyRotationSpeed = Math::RangeRandom(80.0, 180.0);
			_cObject.SetDefaultRotationVelocitySpeed(fEnemyRotationSpeed);
			_cObject.SetMaxRotationVelocitySpeed(fEnemyRotationSpeed);
			_cObject.SetMinRotationVelocitySpeed(0.0);
			_cObject.SetRotationVelocity(Vector3::UNIT_X, fEnemyRotationSpeed);

			// Specify which direction the Enemy mesh faces by default
			_cObject.SetMeshesDefaultForwardDirection(Vector3::NEGATIVE_UNIT_X);
			_cObject.SetMeshesDefaultUpDirection(Vector3::UNIT_Y);
			_cObject.SetMeshesDefaultRightDirection(Vector3::NEGATIVE_UNIT_Z);

			// Specify how many Points this Enemy is worth
			_cObject.mcAttributes.SetNumberOfPoints(500);
		break;

		case GiOBJECT_SPECIFIC_TYPE_KILLER_WHALE:
			// Specify the Enemy Mesh to use
			sEnemyMesh = "KillerWhale.mesh";

			// Orient the Enemy properly
			_cObject.mcpSceneNode->pitch(Degree(-90));

			// Specify Enemy's Size (Length) in Meters
			fEnemyLengthInMeters = 7.0;

			// Randomly pick an amount to scale the Enemies Dimensions by
			sEnemyDimensionsScale.x = Math::RangeRandom(0.75, 1.25);
			sEnemyDimensionsScale.y = Math::RangeRandom(0.75, 1.25);
			sEnemyDimensionsScale.z = Math::RangeRandom(0.75, 1.25);
		
			// Specify Enemies Speed
			fEnemySpeed = Math::RangeRandom((mfLevelFriction / GiMETER) + 5.0, (mfLevelFriction / GiMETER) + 8.0) * GiMETER;
			_cObject.SetDefaultAccelerationRate(fEnemySpeed);
			_cObject.SetMaxVelocitySpeed(fEnemySpeed);
			_cObject.SetMinVelocitySpeed(0.0);

			// Specify Enemies Rotation Speed
			fEnemyRotationSpeed = Math::RangeRandom(70.0, 110.0);
			_cObject.SetDefaultRotationVelocitySpeed(fEnemyRotationSpeed);
			_cObject.SetMaxRotationVelocitySpeed(fEnemyRotationSpeed);
			_cObject.SetMinRotationVelocitySpeed(0.0);
			_cObject.SetRotationVelocity(Vector3::UNIT_X, fEnemyRotationSpeed);

			// Specify which direction the Enemy mesh faces by default
			_cObject.SetMeshesDefaultForwardDirection(Vector3::NEGATIVE_UNIT_X);
			_cObject.SetMeshesDefaultUpDirection(Vector3::UNIT_Y);
			_cObject.SetMeshesDefaultRightDirection(Vector3::NEGATIVE_UNIT_Z);

			// Specify how many Points this Enemy is worth
			_cObject.mcAttributes.SetNumberOfPoints(700);
		break;

		case GiOBJECT_SPECIFIC_TYPE_GOLDEN_FISH:
			// Specify the Enemy Mesh to use
			sEnemyMesh = "GoldenFish.mesh";

			// Specify Enemy's Size (Length) in Meters
			fEnemyLengthInMeters = 1.7;

			// Randomly pick an amount to scale the Enemies Dimensions by
			sEnemyDimensionsScale.x = Math::RangeRandom(1.0, 3.0);
			sEnemyDimensionsScale.y = Math::RangeRandom(0.75, 1.25);
			sEnemyDimensionsScale.z = Math::RangeRandom(0.75, 1.25);
			
			// Specify Enemies Speed
			fEnemySpeed = Math::RangeRandom((mfLevelFriction / GiMETER) + 7.0, (mfLevelFriction / GiMETER) + 9.0) * GiMETER;
			_cObject.SetDefaultAccelerationRate(fEnemySpeed);
			_cObject.SetMaxVelocitySpeed(fEnemySpeed);
			_cObject.SetMinVelocitySpeed(0.0);

			// Specify Enemies Rotation Speed
			fEnemyRotationSpeed = Math::RangeRandom(90.0, 120.0);
			_cObject.SetDefaultRotationVelocitySpeed(fEnemyRotationSpeed);
			_cObject.SetMaxRotationVelocitySpeed(fEnemyRotationSpeed);
			_cObject.SetMinRotationVelocitySpeed(0.0);
			_cObject.SetRotationVelocity(Vector3::UNIT_X, fEnemyRotationSpeed);

			// Specify which directions the Enemy mesh faces by default
			_cObject.SetMeshesDefaultForwardDirection(Vector3::UNIT_Z);
			_cObject.SetMeshesDefaultUpDirection(Vector3::UNIT_Y);
			_cObject.SetMeshesDefaultRightDirection(Vector3::UNIT_X);

			// Specify how many Points this Enemy is worth
			_cObject.mcAttributes.SetNumberOfPoints(1000);
		break;

		case GiOBJECT_SPECIFIC_TYPE_PAREXO_FISH:
			// Specify the Enemy Mesh to use
			sEnemyMesh = "ParexoFish.mesh";

			// Specify Enemy's Size (Length) in Meters
			fEnemyLengthInMeters = 2.0;

			// Randomly pick an amount to scale the Enemies Dimensions by
			sEnemyDimensionsScale.x = Math::RangeRandom(0.75, 1.25);
			sEnemyDimensionsScale.y = Math::RangeRandom(0.75, 1.25);
			sEnemyDimensionsScale.z = Math::RangeRandom(0.75, 1.25);
			
			// Specify Enemies Speed
			fEnemySpeed = Math::RangeRandom((mfLevelFriction / GiMETER) + 2.0, (mfLevelFriction / GiMETER) + 4.0) * GiMETER;
			_cObject.SetDefaultAccelerationRate(fEnemySpeed);
			_cObject.SetMaxVelocitySpeed(fEnemySpeed);
			_cObject.SetMinVelocitySpeed(0.0);

			// Specify Enemies Rotation Speed
			fEnemyRotationSpeed = Math::RangeRandom(150.0, 270.0);
			_cObject.SetDefaultRotationVelocitySpeed(fEnemyRotationSpeed);
			_cObject.SetMaxRotationVelocitySpeed(fEnemyRotationSpeed);
			_cObject.SetMinRotationVelocitySpeed(0.0);
			_cObject.SetRotationVelocity(Vector3::UNIT_X, fEnemyRotationSpeed);

			// Specify which direction the Enemy mesh faces by default
			_cObject.SetMeshesDefaultForwardDirection(Vector3::NEGATIVE_UNIT_X);
			_cObject.SetMeshesDefaultUpDirection(Vector3::UNIT_Y);
			_cObject.SetMeshesDefaultRightDirection(Vector3::NEGATIVE_UNIT_Z);

			// Specify how many Points this Enemy is worth
			_cObject.mcAttributes.SetNumberOfPoints(375);
		break;

		case GiOBJECT_SPECIFIC_TYPE_RING:
			// Specify the Enemy Mesh to use
			sEnemyMesh = "Ring.mesh";
			bUseRandomColoredMaterial = true;

			// Specify Objects Size (Length) in Meters
			fEnemyLengthInMeters = 6.0;

			// Randomly pick an amount to scale the Enemies Dimensions by
			sEnemyDimensionsScale.x = 1.0;
			sEnemyDimensionsScale.y = 1.0;
			sEnemyDimensionsScale.z = 1.0;

			// Specify Enemies Speed
			_cObject.SetDefaultAccelerationRate(0.0);
			_cObject.SetMaxVelocitySpeed(0.0);
			_cObject.SetMinVelocitySpeed(0.0);

			// Specify Enemies Rotation Speed
			_cObject.SetDefaultRotationVelocitySpeed(0.0);
			_cObject.SetMaxRotationVelocitySpeed(0.0);
			_cObject.SetMinRotationVelocitySpeed(0.0);

			// Do not randomly orient this Object, and do not use Auto Tracking
			bRandomlyOrientObject = false;
			bUseAutoTracking = false;

			// Specify which directions the Enemy mesh faces by default
			_cObject.SetMeshesDefaultForwardDirection(Vector3::UNIT_Z);
			_cObject.SetMeshesDefaultUpDirection(Vector3::UNIT_Y);
			_cObject.SetMeshesDefaultRightDirection(Vector3::UNIT_X);

			// Specify how many Points this Enemy is worth
			_cObject.mcAttributes.SetNumberOfPoints(100);
		break;

		case GiOBJECT_SPECIFIC_TYPE_DIAMOND:
			// Specify the Enemy Mesh to use
			sEnemyMesh = "Diamond.mesh";
			bUseRandomColoredMaterial = true;

			// Specify Enemy's Size (Length) in Meters
			fEnemyLengthInMeters = 3.0;

			// Randomly pick an amount to scale the Enemies Dimensions by
			sEnemyDimensionsScale.x = 1.0;
			sEnemyDimensionsScale.y = 1.0;
			sEnemyDimensionsScale.z = 1.0;
			
			// Specify Enemies Speed
			_cObject.SetDefaultAccelerationRate(0.0);
			_cObject.SetMaxVelocitySpeed(0.0);
			_cObject.SetMinVelocitySpeed(0.0);

			// Specify Enemies Rotation Speed
			fEnemyRotationSpeed = Math::RangeRandom(45.0, 90.0);
			_cObject.SetDefaultRotationVelocitySpeed(fEnemyRotationSpeed);
			_cObject.SetMaxRotationVelocitySpeed(fEnemyRotationSpeed);
			_cObject.SetMinRotationVelocitySpeed(0.0);
			_cObject.SetRotationVelocity(Vector3::UNIT_Y, fEnemyRotationSpeed);

			// Do not randomly orient this Object, and do not use Auto Tracking
			bRandomlyOrientObject = false;
			bUseAutoTracking = false;

			// Specify which directions the Enemy mesh faces by default
			_cObject.SetMeshesDefaultForwardDirection(Vector3::UNIT_Z);
			_cObject.SetMeshesDefaultUpDirection(Vector3::UNIT_Y);
			_cObject.SetMeshesDefaultRightDirection(Vector3::UNIT_X);

			// Specify how many Points this Enemy is worth
			_cObject.mcAttributes.SetNumberOfPoints(50);
		break;
	}

	// Create the Object Entity and attach it to the Scene Node
	_cObject.mcpEntity = mcpSceneManager->createEntity(_cObject.GetName() + "Entity", sEnemyMesh);
	_cObject.mcpSceneNode->attachObject(_cObject.mcpEntity);
	_cObject.mcpEntity->setCastShadows(true);

	// Start moving the Object
	_cObject.SetAcceleration(_cObject.GetMeshesDefaultForwardDirection() * _cObject.GetDefaultAccelerationRate());
	_cObject.SetVelocitySpeed(_cObject.GetMaxVelocitySpeed());

	// If this Object should use the Custom Material
	if (bUseRandomColoredMaterial)
	{
		// Randomly calculate the Objects colour
		sEnemyColor = ColourValue(Math::RangeRandom(0, 1), Math::RangeRandom(0, 1), 0);

		// Create the custom Material to use for this Object
		MaterialPtr cpMaterial = MaterialManager::getSingleton().create(_cObject.GetName() + "Material", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, true);
		cpMaterial->setAmbient(sEnemyColor);	// Set the Material colour
		cpMaterial->setDiffuse(sEnemyColor);
		cpMaterial->setSpecular(sEnemyColor);

		// Apply the custom created material
		_cObject.mcpEntity->setMaterialName(_cObject.GetName() + "Material");
	}

	// Randomly calculate where to position the Object, making sure they are positioned above (not in)
	//	the sea floor and below the sea ceiling
	sEnemyPosition.x = Math::RangeRandom(GiLEVEL_LEFT + GiMETER, GiLEVEL_RIGHT - GiMETER);
	sEnemyPosition.y = Math::RangeRandom(GiLEVEL_BOTTOM + GiMETER, GiLEVEL_TOP - GiMETER);
	sEnemyPosition.z = Math::RangeRandom(GiLEVEL_BACK + GiMETER, GiLEVEL_FRONT - GiMETER);

	// Position the Enemy
	_cObject.mcpSceneNode->translate(sEnemyPosition);

	// If the Object should be randomly oriented
	if (bRandomlyOrientObject)
	{
		// Orient the Enemy randomly (in case not using Auto-tracking)
		_cObject.mcpSceneNode->yaw(Degree(Math::RangeRandom(0, 360)), Node::TS_WORLD);
	}

	// Scale the Enemies to the specified size, then apply Dimension scaling to make them look a little different
	fEnemySizeToMeterRatio = GetObjectLengthToMeterRatio(&_cObject);
	fEnemyLengthInMeters = fEnemyLengthInMeters * fEnemySizeToMeterRatio;
	_cObject.mcpSceneNode->scale(fEnemyLengthInMeters, fEnemyLengthInMeters, fEnemyLengthInMeters);
	_cObject.mcpSceneNode->scale(sEnemyDimensionsScale);

	// Apply Level Friction to Object
	_cObject.SetFriction(mfLevelFriction);

	// Give Objects a Scene Node to use for Automatic Tracking
	_cObject.SetExclusiveSceneNodeToUseForAutomaticTracking(mcpSceneManager->getRootSceneNode()->createChildSceneNode(_cObject.GetName() + "TrackingSceneNode"));
	
	// If this Object should use Auto Tracking
	if (bUseAutoTracking)
	{
		// Set this Object to use Auto Tracking
		_cObject.SetIfObjectShouldMoveToDestinationAutomatically(true, true);
	}
}

// Returns how big the Length of the Object is compared to a Meter
float CGame::GetObjectLengthToMeterRatio(CObject* _cpObject)
{
	// Get the size of the Objects mesh
	Vector3 sObjectSize = _cpObject->mcpEntity->getBoundingBox().getSize();

	// Get the Length of the Object, depending on their default facing direction
	float fObjectLength = 0.0f;
	if (_cpObject->GetMeshesDefaultForwardDirection() == Vector3::UNIT_X ||
		_cpObject->GetMeshesDefaultForwardDirection() == Vector3::NEGATIVE_UNIT_X)
	{
		fObjectLength = sObjectSize.x;
	}
	else if (_cpObject->GetMeshesDefaultForwardDirection() == Vector3::UNIT_Y ||
			 _cpObject->GetMeshesDefaultForwardDirection() == Vector3::NEGATIVE_UNIT_Y)
	{
		fObjectLength = sObjectSize.y;
	}
	else if (_cpObject->GetMeshesDefaultForwardDirection() == Vector3::UNIT_Z ||
			 _cpObject->GetMeshesDefaultForwardDirection() == Vector3::NEGATIVE_UNIT_Z)
	{
		fObjectLength = sObjectSize.z;
	}

	// Make sure no Divide by Zero
	if (fObjectLength == 0.0)
	{
		fObjectLength = -1.0;
	}

	// Return the Length to Meter ratio
	return (GiMETER / fObjectLength);
}

// Function creates a new Flocking or Non-Flocking Fish and returns a pointer to it
// NOTE: Using -1 for ObjectSpecificType will randomly pick which specific type of Fish to create
CObject* CGame::CreateFish(bool _bIsFlockingFish, int _iObjectSpecificType)
{
	CObject cTempObject;						// Temp Object used to create the Fish
	int iSpecificType = _iObjectSpecificType;	// The Specific Type of Fish to create
	
	// If this should be a Flocking Fish
	if (_bIsFlockingFish)
	{
		// Copy Flocking Fish parameters to this Fish
		cTempObject = mcDefaultFlockingFish;

		// Set how often this Fish's Neighbor list should be updated
		cTempObject.mcAIHelper.SetUpdateTimeInterval((int)Math::RangeRandom(500, 1000));
	}

	// Set the Objects ID, Name, and General Type
	cTempObject.SetID(miTotalNumberOfObjectsCreated++);
	cTempObject.SetName("Enemy" + StringConverter::toString(cTempObject.GetID()));
	cTempObject.SetGeneralObjectType(GiOBJECT_GENERAL_TYPE_ENEMY);

	// If we should randomly pick which specific type of Fish to create
	if (iSpecificType == -1)
	{
		// If we should create a random Flocking Fish
		if (_bIsFlockingFish)
		{
			// Randomly pick the type of Flocking Fish to create
			iSpecificType = (int)Math::RangeRandom(GiOBJECT_SPECIFIC_TYPE_FLOCKING_FISH_INDEX_START, GiOBJECT_SPECIFIC_TYPE_FLOCKING_FISH_INDEX_END + 1);
		}
		// Else we should create a random Non-Flocking Fish
		else
		{
			// Randomly pick the type of Non-Flocking Fish to create
			iSpecificType = (int)Math::RangeRandom(GiOBJECT_SPECIFIC_TYPE_NON_FLOCKING_FISH_INDEX_START, GiOBJECT_SPECIFIC_TYPE_NON_FLOCKING_FISH_INDEX_END + 1);
		}
	}

	// Set the type of Fish to create
	cTempObject.SetSpecificObjectType(iSpecificType);

	// Setup the Fish
	CreateObject(cTempObject);

	// Insert the Object into the Object List
	mcObjectList.push_front(cTempObject);

	// If we just created a Flocking Fish
	if (_bIsFlockingFish)
	{
		// Increment the Number of Flocking Fish
		miFlockingFishCount++;
	}
	else
	{
		// Increment the Number of Non-Flocking Fish
		miNonFlockingFishCount++;	
	}

	// Return a pointer to the newly created Object
	return &mcObjectList.front();
}

// Function creates and returns a pointer to an Item
CObject* CGame::CreateItem(int _iObjectSpecificType)
{
	CObject cTempObject;		// Temp Object used to create the Item

	// Set the Objects ID, Name, and Type
	cTempObject.SetID(miTotalNumberOfObjectsCreated++);
	cTempObject.SetName("Item" + StringConverter::toString(cTempObject.GetID()));
	cTempObject.SetGeneralObjectType(GiOBJECT_GENERAL_TYPE_ITEM);
	cTempObject.SetSpecificObjectType(_iObjectSpecificType);

	// Create the Item
	CreateObject(cTempObject);

	// Insert the Object into the Object List
	mcObjectList.push_front(cTempObject);

	// Return a pointer to the newly created Item
	return &mcObjectList.front();
}



/////////////////////////////////////////////////////////////////
// Input Handler Functions

// Key Pressed Listener - This function is called everytime a keyboard key is pressed
bool CGame::keyPressed(const OIS::KeyEvent& _cArg)
{
	// If the Escape key was pressed
	if (_cArg.key == OIS::KC_ESCAPE)
	{
		// If we are in the Main Menu
		if (mcGameStateManager.ReturnCurrentStateFunction() == (void*)GameStateMainMenu)
		{
			// Exit the Game by shutting down all Game States
			mcGameStateManager.ShutdownAllStates(this);
		}
		// Else return to the Main Menu
		else
		{
			mcGameStateManager.ShutdownCurrentStateAndSetNextState(GameStateMainMenu, this);
		}
		return true;
	}

	// If the Show/HideDebug HUD key was pressed
	if (_cArg.key == OIS::KC_TAB)
	{
		// Toggle showing the FPS
		mbShowDebugHUD = ShowDebugHUD(!mbShowDebugHUD);
	}

	// If we are in the Play Game State
	if (mcGameStateManager.ReturnCurrentStateFunction() == (void*)GameStatePlay)
	{
		// Set factor used to reverse the Y-axis controls
//		int iReverseYAxis = (mbReverseYAxis == true) ? -1 : 1;

		// Temp vectors used for calculations
		Vector3 cVector = Vector3::ZERO;
		Vector3 cVector2 = Vector3::ZERO;

		// Check which Keyboard Key was pressed
		switch (_cArg.key)
		{
			case OIS::KC_UP:
				// Get the Players default Forward Direction and their current Acceleration
				cVector = mcpPlayer->GetMeshesDefaultForwardDirection();
				cVector2 = mcpPlayer->GetAcceleration();

				// Calculate new Acceleration vector
				cVector.x = (cVector.x == 1.0) ? mcpPlayer->GetDefaultAccelerationRate() : cVector2.x;
				cVector.y = (cVector.y == 1.0) ? mcpPlayer->GetDefaultAccelerationRate() : cVector2.y;
				cVector.z = (cVector.z == 1.0) ? mcpPlayer->GetDefaultAccelerationRate() : cVector2.z;

				// Move the Player forward
				mcpPlayer->SetAcceleration(cVector);
			break;

			case OIS::KC_DOWN:
				// Get the Players default Forward Direction and their current Acceleration
				cVector = mcpPlayer->GetMeshesDefaultForwardDirection();
				cVector2 = mcpPlayer->GetAcceleration();

				// Calculate new Acceleration vector
				cVector.x = (cVector.x == 1.0) ? -mcpPlayer->GetDefaultAccelerationRate() : cVector2.x;
				cVector.y = (cVector.y == 1.0) ? -mcpPlayer->GetDefaultAccelerationRate() : cVector2.y;
				cVector.z = (cVector.z == 1.0) ? -mcpPlayer->GetDefaultAccelerationRate() : cVector2.z;

				// Move the Player backwards
				mcpPlayer->SetAcceleration(cVector);
			break;

			case OIS::KC_LEFT:
				// Rotate the Player left
				mcpPlayer->SetRotationAcceleration(Vector3(0, mcpPlayer->GetDefaultRotationAccelerationRate(), 0));
			break;

			case OIS::KC_RIGHT:
				// Rotate the Player right
				mcpPlayer->SetRotationAcceleration(Vector3(0, -mcpPlayer->GetDefaultRotationAccelerationRate(), 0));
			break;

			case OIS::KC_SPACE:
				// Get the Players default Up Direction and their current Acceleration
				cVector = mcpPlayer->GetMeshesDefaultUpDirection();
				cVector2 = mcpPlayer->GetAcceleration();

				// Calculate new Acceleration vector
				cVector.x = (cVector.x == 1.0) ? mcpPlayer->GetDefaultAccelerationRate() : cVector2.x;
				cVector.y = (cVector.y == 1.0) ? mcpPlayer->GetDefaultAccelerationRate() : cVector2.y;
				cVector.z = (cVector.z == 1.0) ? mcpPlayer->GetDefaultAccelerationRate() : cVector2.z;

				// Move the Player upwards
				mcpPlayer->SetAcceleration(cVector);
			break;

			case OIS::KC_Y:
				// Reverse the y-axis controls
				mbReverseYAxis = !mbReverseYAxis;
			break;

			case OIS::KC_RETURN:
				// Increment to the next Camera Mode
				// If we have went through all of the Camera Modes
				if (++miCameraToUse > 2)
				{
					// Wrap back around to the first Mode
					miCameraToUse = 0;
				}

				// Specity the Camera Mode to use
				UseCamera(miCameraToUse);
			break;

			case OIS::KC_1:
				// Use First Person Camera Mode
				miCameraMode = GiCAMERA_MODE_FIRST_PERSON;
			break;

			case OIS::KC_2:
				// Use Third Person Fixed Camera Mode
				miCameraMode = GiCAMERA_MODE_THIRD_PERSON_FIXED;
			break;

			case OIS::KC_3:
				// Use Third Person Chase Camera Mode
				miCameraMode = GiCAMERA_MODE_THIRD_PERSON_CHASE;
			break;

			case OIS::KC_RSHIFT:
				// Toggle showing the Objective
				mbShowObjective = !mbShowObjective;
			break;

			case OIS::KC_X:
				// Get the Desired position of the Camera
				cVector = mcpDesiredCameraPosition->getPosition();
				
				// If the Desired Camera position isn't too far away from the Player
				if (cVector.z > -30 * GiMETER)
				{
					// Zoom the Camera out away from the Player
					mcpDesiredCameraPosition->translate(Vector3(0, 0, -2 * GiMETER), Node::TS_LOCAL);
				}
			break;

			case OIS::KC_Z:
				// Get the Desired position of the Camera
				cVector = mcpDesiredCameraPosition->getPosition();
				
				// If the Desired Camera position isn't too close to the Player
				if (cVector.z < -5 * GiMETER)
				{
					// Zoom the Camera in towards the Player
					mcpDesiredCameraPosition->translate(Vector3(0, 0, 2 * GiMETER), Node::TS_LOCAL);
				}
			break;

			// TODO - Remove this developer cheat to instantly beat a level
			case OIS::KC_W:
				mcGameStateManager.ShutdownCurrentStateAndSetNextState(GameStatePlayerBeatLevel, this);
			break;

			// TODO - Remove this developer cheat to instantly die
			case OIS::KC_D:
				mcGameStateManager.ShutdownCurrentStateAndSetNextState(GameStateGameOver, this);
			break;

			// TODO - Remove this developer cheat to instantly get more time
			case OIS::KC_T:
				mfLevelTime += 10.0f;
			break;

			// TODO - Remove this developer cheat to instantly gain more health
			case OIS::KC_H:
				mcpPlayer->mcAttributes.SetHealthRelative(1);
			break;

			// Do nothing if it's an unrecognized key
			default:
			break;
		}
	}
	// Else If we are in the Main Menu Game State
	else if (mcGameStateManager.ReturnCurrentStateFunction() == (void*)GameStateMainMenu)
	{
		// Check which Keyboard Key was pressed
		switch (_cArg.key)
		{
			case OIS::KC_SPACE:
				// Set the Level to play to zero (Free Swim Mode)
				miLevel = 1;

				// Switch to Loading the Level
				mcGameStateManager.ShutdownCurrentStateAndSetNextState(GameStateLoadingLevel, this);
			break;

			case OIS::KC_RETURN:
				// Set the Level to play to zero (Free Swim Mode)
				miLevel = 0;

				// Switch to Loading the Level
				mcGameStateManager.ShutdownCurrentStateAndSetNextState(GameStateLoadingLevel, this);
			break;

			// TODO - Remove these developer cheats to quickly access levels
			case OIS::KC_1:
				// Set the Level to play to 1
				miLevel = 1;

				// Switch to Loading the Level
				mcGameStateManager.ShutdownCurrentStateAndSetNextState(GameStateLoadingLevel, this);
			break;

			case OIS::KC_2:
				// Set the Level to play to 2
				miLevel = 2;

				// Switch to Loading the Level
				mcGameStateManager.ShutdownCurrentStateAndSetNextState(GameStateLoadingLevel, this);
			break;

			case OIS::KC_3:
				// Set the Level to play to 3
				miLevel = 3;

				// Switch to Loading the Level
				mcGameStateManager.ShutdownCurrentStateAndSetNextState(GameStateLoadingLevel, this);
			break;

			case OIS::KC_4:
				// Set the Level to play to 4
				miLevel = 4;

				// Switch to Loading the Level
				mcGameStateManager.ShutdownCurrentStateAndSetNextState(GameStateLoadingLevel, this);
			break;

			case OIS::KC_5:
				// Set the Level to play to 5
				miLevel = 5;

				// Switch to Loading the Level
				mcGameStateManager.ShutdownCurrentStateAndSetNextState(GameStateLoadingLevel, this);
			break;


			// Do nothing if it's an unrecognized key
			default:
			break;
		}
	}
	// Else If we are in the Loading Game State
	else if (mcGameStateManager.ReturnCurrentStateFunction() == (void*)GameStateLoadingLevel)
	{
		// Check which Keyboard Key was pressed
		switch (_cArg.key)
		{
			case OIS::KC_SPACE:
				// Switch to playing the game
				mcGameStateManager.ShutdownCurrentStateAndSetNextState(GameStatePlay, this);
			break;

			// Do nothing if it's an unrecognized key
			default:
			break;
		}
	}
	// Else If we are in the Player Beat Level
	else if (mcGameStateManager.ReturnCurrentStateFunction() == (void*)GameStatePlayerBeatLevel)
	{
		// Check which Keyboard Key was pressed
		switch (_cArg.key)
		{
			case OIS::KC_SPACE:
				// Increase to the next level
				miLevel++;

				// If the Player just beat the last Level
				if (miLevel == GiNUMBER_OF_LEVELS)
				{
					// Switch to the Beat Game State
					mcGameStateManager.ShutdownCurrentStateAndSetNextState(GameStatePlayerBeatGame, this);
				}
				else
				{
					// Switch to loading the next level
					mcGameStateManager.ShutdownCurrentStateAndSetNextState(GameStateLoadingLevel, this);
				}
			break;

			// Do nothing if it's an unrecognized key
			default:
			break;
		}
	}
	// Else If we are in the Player Beat the Game State
	else if (mcGameStateManager.ReturnCurrentStateFunction() == (void*)GameStatePlayerBeatGame)
	{
		// Check which Keyboard Key was pressed
		switch (_cArg.key)
		{
			case OIS::KC_SPACE:
				// Switch back to the Main Menu
				mcGameStateManager.ShutdownCurrentStateAndSetNextState(GameStateMainMenu, this);
			break;

			// Do nothing if it's an unrecognized key
			default:
			break;
		}
	}
	// Else If we are in the Game Over State
	else if (mcGameStateManager.ReturnCurrentStateFunction() == (void*)GameStateGameOver)
	{
		// Check which Keyboard Key was pressed
		switch (_cArg.key)
		{
			case OIS::KC_SPACE:
				// Switch to the Main Menu
				mcGameStateManager.ShutdownCurrentStateAndSetNextState(GameStateLoadingLevel, this);
			break;

			// Do nothing if it's an unrecognized key
			default:
			break;
		}
	}

	return true;
}

// Key Released Listener - This function is called everytime a keyboard key is released
bool CGame::keyReleased(const OIS::KeyEvent& _cArg)
{ 
	// If we are in the Main Menu Game State
	if (mcGameStateManager.ReturnCurrentStateFunction() == (void*)GameStateMainMenu)
	{
	}
	// Else If we are in the Play Game State
	else if (mcGameStateManager.ReturnCurrentStateFunction() == (void*)GameStatePlay)
	{
		// Temp vectors used for calculations
		Vector3 cVector = Vector3::ZERO;
		Vector3 cVector2 = Vector3::ZERO;

		// Check which Keyboard Key was released
		switch (_cArg.key)
		{
			case OIS::KC_UP:
			case OIS::KC_DOWN:
				// Get the Players default Forward Direction and their current Acceleration
				cVector = mcpPlayer->GetMeshesDefaultForwardDirection();
				cVector2 = mcpPlayer->GetAcceleration();

				// Calculate new Acceleration vector
				cVector.x = (cVector.x == 1.0) ? 0.0 : cVector2.x;
				cVector.y = (cVector.y == 1.0) ? 0.0 : cVector2.y;
				cVector.z = (cVector.z == 1.0) ? 0.0 : cVector2.z;

				// Stop moving Player forward/backwards
				mcpPlayer->SetAcceleration(cVector);
			break;

			case OIS::KC_LEFT:
			case OIS::KC_RIGHT:
				// Stop Rotating the Player left/right
				mcpPlayer->SetRotationAcceleration(Vector3::ZERO);
				mcpPlayer->SetRotationVelocity(Vector3::ZERO);
			break;

			case OIS::KC_D:
				// Get the Players default Up Direction and their current Acceleration
				cVector = mcpPlayer->GetMeshesDefaultUpDirection();
				cVector2 = mcpPlayer->GetAcceleration();

				// Calculate new Acceleration vector
				cVector.x = (cVector.x == 1.0) ? 0.0 : cVector2.x;
				cVector.y = (cVector.y == 1.0) ? 0.0 : cVector2.y;
				cVector.z = (cVector.z == 1.0) ? 0.0 : cVector2.z;

				// Stop moving Player upwards
				mcpPlayer->SetAcceleration(cVector);
			break;

			// Do nothing if it's an unrecognized key
			default:
			break;
		}
	}
	// Else If we are in the Player Won Game State
	else if (mcGameStateManager.ReturnCurrentStateFunction() == (void*)GameStatePlayerBeatLevel)
	{
	}
	// Else If we are in the Game Over State
	else if (mcGameStateManager.ReturnCurrentStateFunction() == (void*)GameStateGameOver)
	{
	}

	return true;
}

// Mouse Movement Listener - This function is called everytime the mouse is moved
bool CGame::mouseMoved(const OIS::MouseEvent& _cArg)
{
	// If we are in the Main Menu Game State
	if (mcGameStateManager.ReturnCurrentStateFunction() == (void*)GameStateMainMenu)
	{
	}
	// Else If we are in the Play Game State
	else if (mcGameStateManager.ReturnCurrentStateFunction() == (void*)GameStatePlay)
	{
		// Set factor used to reverse the Y-axis controls
		int iReverseYAxis = (mbReverseYAxis == true) ? 1 : -1;

		// If the Right Mouse Button is being held down
		if (_cArg.state.buttonDown(OIS::MB_Right))
		{
			// Record that the Player is looking around
			mbLookingAround = true;
			
			// Rotate the Camera based on which Camera Mode is being used
			switch (miCameraMode)
			{
				case GiCAMERA_MODE_FIRST_PERSON:
				case GiCAMERA_MODE_THIRD_PERSON_FIXED:
					// Rotate the Camera around it's local axis
					mcpCamera->yaw(Degree(mfMouseSpeed * -_cArg.state.X.rel * mcFrameEvent.timeSinceLastFrame));
					mcpCamera->pitch(Degree(iReverseYAxis * mfMouseSpeed * _cArg.state.Y.rel * mcFrameEvent.timeSinceLastFrame));
				break;

				default:
				case GiCAMERA_MODE_THIRD_PERSON_CHASE:					
					// Rotate the Camera around the Player
					mcpPlayersPositionForCamera->yaw(Degree(mfMouseSpeed * -_cArg.state.X.rel * mcFrameEvent.timeSinceLastFrame), Node::TS_PARENT);
					mcpPlayersPositionForCamera->pitch(Degree(-iReverseYAxis * mfMouseSpeed * _cArg.state.Y.rel * mcFrameEvent.timeSinceLastFrame), Node::TS_PARENT);
				break;
			}			
		}

		// If the Mouse Scroll Wheel was rolled forward
		if (_cArg.state.Z.rel > 0)
		{
			// Get the Desired position of the Camera
			Vector3 cVector = mcpDesiredCameraPosition->getPosition();
			
			// If the Desired Camera position isn't too close to the Player
			if (cVector.z < -5 * GiMETER)
			{
				// Zoom the Camera in towards the Player
				mcpDesiredCameraPosition->translate(Vector3(0, 0, 2 * GiMETER), Node::TS_LOCAL);
			}
		}
		// If the Mouse Scroll Wheel was rolled backward
		else if (_cArg.state.Z.rel < 0)
		{
			// Get the Desired position of the Camera
			Vector3 cVector = mcpDesiredCameraPosition->getPosition();
			
			// If the Desired Camera position isn't too far away from the Player
			if (cVector.z > -30 * GiMETER)
			{
				// Zoom the Camera out away from the Player
				mcpDesiredCameraPosition->translate(Vector3(0, 0, -2 * GiMETER), Node::TS_LOCAL);
			}
		}
	
		// Else no Mouse Buttons are being held down
		else
		{
		}
	}
	// Else If we are in the Player Won Game State
	else if (mcGameStateManager.ReturnCurrentStateFunction() == (void*)GameStatePlayerBeatLevel)
	{
	}
	// Else If we are in the Game Over State
	else if (mcGameStateManager.ReturnCurrentStateFunction() == (void*)GameStateGameOver)
	{
	}

	return true;
}

// Mouse Pressed Listener - This function is called everytime a mouse button is pressed
bool CGame::mousePressed(const OIS::MouseEvent& _cArg, OIS::MouseButtonID _ID)
{ 
	// If we are in the Main Menu Game State
	if (mcGameStateManager.ReturnCurrentStateFunction() == (void*)GameStateMainMenu)
	{
	}
	// Else If we are in the Play Game State
	else if (mcGameStateManager.ReturnCurrentStateFunction() == (void*)GameStatePlay)
	{
		// Temp vectors used for calculations
		Vector3 cVector = Vector3::ZERO;
		Vector3 cVector2 = Vector3::ZERO;

		// Check which Mouse Button was pressed
		switch (_ID)
		{
			case OIS::MB_Left:
				// Get the Players default Up Direction and their current Acceleration
				cVector = mcpPlayer->GetMeshesDefaultUpDirection();
				cVector2 = mcpPlayer->GetAcceleration();

				// Calculate new Acceleration vector
				cVector.x = (cVector.x == 1.0) ? mcpPlayer->GetDefaultAccelerationRate() : cVector2.x;
				cVector.y = (cVector.y == 1.0) ? mcpPlayer->GetDefaultAccelerationRate() : cVector2.y;
				cVector.z = (cVector.z == 1.0) ? mcpPlayer->GetDefaultAccelerationRate() : cVector2.z;

				// Move the Player upwards
				mcpPlayer->SetAcceleration(cVector);
			break;

			default:
			break;
		}
	}
	// Else If we are in the Loading Game State
	else if (mcGameStateManager.ReturnCurrentStateFunction() == (void*)GameStateLoadingLevel)
	{
		// Check which Keyboard Key was pressed
		switch (_ID)
		{
			case OIS::MB_Left:
				// Switch to playing the game
				mcGameStateManager.ShutdownCurrentStateAndSetNextState(GameStatePlay, this);
			break;

			// Do nothing if it's an unrecognized key
			default:
			break;
		}
	}
	// Else If we are in the Player Won Game State
	else if (mcGameStateManager.ReturnCurrentStateFunction() == (void*)GameStatePlayerBeatLevel)
	{
	}
	// Else If we are in the Game Over State
	else if (mcGameStateManager.ReturnCurrentStateFunction() == (void*)GameStateGameOver)
	{
	}

	return true;
}

// Mouse Released Listener - This function is called everytime a mouse button is released
bool CGame::mouseReleased(const OIS::MouseEvent& _cArg, OIS::MouseButtonID _ID)
{ 
	// If we are in the Main Menu Game State
	if (mcGameStateManager.ReturnCurrentStateFunction() == (void*)GameStateMainMenu)
	{
	}
	// Else If we are in the Play Game State
	else if (mcGameStateManager.ReturnCurrentStateFunction() == (void*)GameStatePlay)
	{
		// Temp vectors used for calculations
		Vector3 cVector = Vector3::ZERO;
		Vector3 cVector2 = Vector3::ZERO;

		// Check which Mouse Button was released
		switch (_ID)
		{
			case OIS::MB_Left:
				// Get the Players default Up Direction and their current Acceleration
				cVector = mcpPlayer->GetMeshesDefaultUpDirection();
				cVector2 = mcpPlayer->GetAcceleration();

				// Calculate new Acceleration vector
				cVector.x = (cVector.x == 1.0) ? 0.0 : cVector2.x;
				cVector.y = (cVector.y == 1.0) ? 0.0 : cVector2.y;
				cVector.z = (cVector.z == 1.0) ? 0.0 : cVector2.z;

				// Stop moving Player upwards
				mcpPlayer->SetAcceleration(cVector);
			break;

			case OIS::MB_Right:
				// Record that we are no longer looking around
				mbLookingAround = false;

				// Set that the Last Camera Mode is different than the current one to reset Camera Mode settings
				miLastFramesCameraMode = miCameraMode + 1;

				// Look around
				switch (miCameraMode)
				{
					case GiCAMERA_MODE_FIRST_PERSON:
					case GiCAMERA_MODE_THIRD_PERSON_FIXED:
						// Rotate the Camera back to it's original orientation
						mcpCamera->setOrientation(Quaternion::IDENTITY);
					break;

					default:
					case GiCAMERA_MODE_THIRD_PERSON_CHASE:
						// Rotate the Camera back to it's original orientation
						mcpPlayersPositionForCamera->setOrientation(Quaternion::IDENTITY);
					break;
				}	
			break;

			default:
			break;
		}
	}
	// Else If we are in the Player Won Game State
	else if (mcGameStateManager.ReturnCurrentStateFunction() == (void*)GameStatePlayerBeatLevel)
	{
	}
	// Else If we are in the Game Over State
	else if (mcGameStateManager.ReturnCurrentStateFunction() == (void*)GameStateGameOver)
	{
	}

	return true;
}


// Joystick Pressed Listener - This function is called everytime a joystick button is pressed
bool CGame::buttonPressed(const OIS::JoyStickEvent &_cArg, int _iButton) { return true; }

// Joystick Released Listener - This function is called everytime a joystick button is released
bool CGame::buttonReleased(const OIS::JoyStickEvent &_cArg, int _iButton) { return true; }

// Joystick Moved Listener - This function is called everytime the joystick axis is moved
bool CGame::axisMoved(const OIS::JoyStickEvent &_cArg, int _iAxis) { return true; }


/////////////////////////////////////////////////////////////////
// Error Handling Functions

// Writes the Text to the Log File
void CGame::WriteToLogFile(string _sText)
{
	// Open the Log File in append mode so we can add to the end of it
	ofstream fOutFile("../../Log.txt", ios_base::out | ios_base::app);

	// If the File was opened successfully
	if (fOutFile.is_open())
	{
		// Write the text to the Log File
		fOutFile << _sText << endl;
	}

	// Close the Log File
	fOutFile.close();
}



/////////////////////////////////////////////////////////////////
// Global Functions

// Makes the Player flash for a few seconds
bool MakePlayerFlash(void* _vpOptionalDataToPass)
{
	// Get the Game object pointer
	CGame* cpGame = (CGame*)_vpOptionalDataToPass;

	// If the Player is Dead or we are no longer in the Play Game State
	if (cpGame->mcpPlayer->mcAttributes.ReturnHealth() < 1 || 
		cpGame->mcGameStateManager.ReturnCurrentStateFunction() != cpGame->GameStatePlay)
	{
		// Stop flashing the Player
		return false;
	}

	// Make the Player visible/invisible
	cpGame->mcpPlayer->mcpSceneNode->flipVisibility(false);
	
	// Toggle the Players SpotLight on and off, as toggling the Players visibility seems to toogle the spot light too
	Light* cpPlayerSpotLight = cpGame->mcpSceneManager->getLight("PlayerSpotLight");
	cpPlayerSpotLight->setVisible(!cpPlayerSpotLight->getVisible());

	return true;
}