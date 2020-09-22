#ifndef CAPPLICATION_CPP
#define CAPPLICATION_CPP

#include <Ogre.h>				// Include OGRE
#include <OIS/OIS.h>			// Include the Open Input System
#include <CEGUI/CEGUI.h>		// Include CEGUI
#include <OgreCEGUIRenderer.h>	// Include CEGUI plugin for Ogre

using namespace Ogre;

// This class is used to do all application Initialization, Shutdown, and the game loop
class CApplication
{
public:
	// Constructor
	CApplication()
	{
		// Initialize all private data to null
		mcpRoot = NULL;
		mcpRenderWindow = NULL;
		mcpKeyboard = NULL;
		mcpMouse = NULL;
		mcpJoystick = NULL;
		mcpInputManager = NULL;
		mcpRenderer = NULL;
		mcpSystem = NULL;

		mcpSceneManager = NULL;
		mcpCamera = NULL;
		mcpViewport = NULL;
	}

	// Destructor
    ~CApplication()
    {
		// Destroy Input objects
		if (mcpInputManager != NULL)
		{
			mcpInputManager->destroyInputObject(mcpKeyboard);
			OIS::InputManager::destroyInputSystem(mcpInputManager);
		}

		// Destroy CEGUI objects
		if (mcpRenderer != NULL)
		{	delete mcpRenderer;}
		if (mcpSystem != NULL)
		{	delete mcpSystem;}

		// Destroy remaining Ogre objects
		if (mcpRoot != NULL)
		{	delete mcpRoot;}
    }

	// Function creates the Window, OGRE objects, etc and starts the game loop
	void Go()
    {
        CreateRoot();
        DefineResources();

        // If we could not get the Render System settings
        if (!SetupRenderSystem())
		{	
			// Exit the application
			return;
		}

        CreateRenderWindow();
        InitializeResourceGroups();
        InitializeCoreSceneComponents();
        SetupInputSystem();
        SetupCEGUI();
        SetListeners();
		GameInitialization();
        StartRenderLoop();
    }

protected:
    Root* mcpRoot;							// OGRE root
	RenderWindow* mcpRenderWindow;			// Handle to the Window
    OIS::Keyboard* mcpKeyboard;				// Handle to the Keyboard
	OIS::Mouse* mcpMouse;					// Handle to the Mouse
	OIS::JoyStick* mcpJoystick;				// Handle to the Joystick
    OIS::InputManager* mcpInputManager;		// Handle to the OIS Input Manager
    CEGUI::OgreCEGUIRenderer* mcpRenderer;	// Handle to CEGUI Renderer
    CEGUI::System* mcpSystem;				// Handle to CEGUI System object

	SceneManager* mcpSceneManager;			// Handle to the Scene Manager
	Camera* mcpCamera;						// Handle to the Camera
	Viewport* mcpViewport;					// Handle to the Viewport

	// Create the Ogre root object
    void CreateRoot()
    {
		mcpRoot = new Root();
    }
    
	// Tell Ogres ResourceGroupManager where it can find all of the needed resources
    void DefineResources()
    {
		String secName, typeName, archName;
		ConfigFile cf;

		// Load the "resources.cfg" into the ConfigFile parser
		cf.load("resources.cfg");

		// Start looping through the parsed ConfigFile
		ConfigFile::SectionIterator seci = cf.getSectionIterator();
		while (seci.hasMoreElements())
		{
			secName = seci.peekNextKey();
			ConfigFile::SettingsMultiMap* settings = seci.getNext();
			ConfigFile::SettingsMultiMap::iterator i;

			// Add the resources to the ResourceGroupManager
			for (i = settings->begin(); i != settings->end(); ++i)
			{
				typeName = i->first;
				archName = i->second;
				ResourceGroupManager::getSingleton().addResourceLocation(archName, typeName, secName);
			}
       }
    }
    
	// Specify which Render System should be used (ie DirectX, OpenGL, etc)
    bool SetupRenderSystem()
    {
		try
		{
			// Try loading the settings if they are stored already
			// If the settings are not already stored
			if (!mcpRoot->restoreConfig())
			{
				// Prompt the user with the ConfigDialog box to let them choose their settings
				// If the user did not choose any settings (ie they cancelled the dialog box)
				if (!mcpRoot->showConfigDialog())
				{
					// Exit the Application
					return false;
				}
			}
		} 
		// Catch all exceptions
		catch(...)
		{
			// Prompt the user with the ConfigDialog box to let them choose their settings
			// If the user did not choose any settings (ie they cancelled the dialog box)
			if (!mcpRoot->showConfigDialog())
			{
				// Exit the Application
				return false;
			}
		}
		return true;
    }
    
	// Create the Window to Render in
    void CreateRenderWindow()
    {
		// Save a pointer to the Render Window being created
		mcpRenderWindow = mcpRoot->initialise(true, "Daniel Schroeder's Ocean Explorer");
    }

	// Initialize any resources we need to use right away
    void InitializeResourceGroups()
    {
		// Set the default number of Mipmaps to use
		TextureManager::getSingleton().setDefaultNumMipmaps(5);

		// Initialze all game resources
		ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
    }

	// Initialize Core Scene Objects
    void InitializeCoreSceneComponents()
    {
		// Initialize the SceneManager, Camera, and Viewport
		mcpSceneManager = mcpRoot->createSceneManager(/*ST_GENERIC*/ST_EXTERIOR_FAR, "Default SceneManager");
		mcpCamera = mcpSceneManager->createCamera("Camera");
		mcpViewport = mcpRenderWindow->addViewport(mcpCamera);

		// Set the Camera Aspect Ratio to match the Viewport
		mcpCamera->setAspectRatio(Real(mcpViewport->getActualWidth()) / Real(mcpViewport->getActualHeight()));
    }

	// Initialize OIS (Open Input System)
    void SetupInputSystem()
    {
		// Local variables used to initialize OIS
		size_t windowHnd = 0;
		std::ostringstream windowHndStr;
		OIS::ParamList pl;

		// Initialize the OIS InputManager
		mcpRenderWindow->getCustomAttribute("WINDOW", &windowHnd);
		windowHndStr << windowHnd;
		pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));
		mcpInputManager = OIS::InputManager::createInputSystem(pl);

		try
		{
			// Get handles to the Keyboard, Mouse, and Joystick using Buffered Input
			mcpKeyboard = static_cast<OIS::Keyboard*>(mcpInputManager->createInputObject(OIS::OISKeyboard, true));
			mcpMouse = static_cast<OIS::Mouse*>(mcpInputManager->createInputObject(OIS::OISMouse, true));
			//mcpJoystick = static_cast<OIS::JoyStick*>(mcpInputManager->createInputObject(OIS::OISJoyStick, true));
		}
		catch (const OIS::Exception &e)
		{
			throw new Exception(42, e.eText, "CApplication::setupInputSystem");
		}
    }

	// Initialize CEGUI
    void SetupCEGUI()
    {
		// CEGUI setup
		mcpRenderer = new CEGUI::OgreCEGUIRenderer(mcpRenderWindow, Ogre::RENDER_QUEUE_OVERLAY, false, 3000, mcpSceneManager);
		mcpSystem = new CEGUI::System(mcpRenderer);
    }

	// Attach the Frame Listener and any Input Handlers
	virtual void SetListeners()
	{
		/////////////////////////////////////////////////////////////
		//	Inheriting class should override this function and set the Frame Listener and Input Handlers
		//
		//	Example:
		//		// Set this class as the Keyboard Input Handler
		//		if (mcpKeyboard != NULL)
		//		{	mcpKeyboard->setEventCallback(this);}
		//
		//		// Set this class as the Mouse Input Handler
		//		if (mcpMouse != NULL)
		//		{	mcpMouse->setEventCallback(this);}
		//
		//		// Set this class as the Joystick Input Handler
		//		if (mcpJoystick != NULL)
		//		{	mcpJoystick->setEventCallback(this);}
		//
		//		// Set this class to be the Frame Listener
		//		mcpRoot->addFrameListener(this);
		/////////////////////////////////////////////////////////////
	}

	// Do any extra Game initialization, such as creating the initial scene or setting the game state
	virtual void GameInitialization()
	{}

	// Start the game loop
    void StartRenderLoop()
    {
		// Start rendering the scene
		mcpRoot->startRendering();
    }
};

#endif
