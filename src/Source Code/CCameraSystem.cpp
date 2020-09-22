#ifndef CCAMERA_SYSTEM_CPP
#define CCAMERA_SYSTEM_CPP

#include <Ogre.h>
using namespace Ogre;

// Camera Class
class CCameraSystem
{
private:
	String msName;
	SceneManager* mcpSceneManager;
	
	Camera* mcpCamera;			// Holds the Camera
	SceneNode* mcpCameraNode;	// Holds the Cameras Position
	SceneNode* mcpTargetNode;	// Holds the Targets Position (where the Camera should looking)

	float mfCameraTightness;	// How fast the Camera should move to where it should be
	float mfTargetTightness;	// How fast the Target should move to where it should be

	float mfMaxCameraOffsetDistance;	// The max distance allowed between where the Camera is and where it should be
	float mfMaxTargetOffsetAngle;		// The max distance allowed between where the Target is and where it should be

public:
	
	// Default Constructor
	CCameraSystem()
	{
		msName = "";
		mcpSceneManager = NULL;
		mcpCamera = NULL;
		mcpCameraNode = mcpTargetNode = NULL;
		mfCameraTightness = mfTargetTightness = mfMaxCameraOffsetDistance = mfMaxTargetOffsetAngle = 0.0f;
	}

	// Explicit Constructor
	CCameraSystem(String sName, SceneManager* cpSceneManager, Camera* cpCamera)
	{
		SetupCamera(sName, cpSceneManager, cpCamera);
	}

	// Setup the Camera System
	void SetupCamera(String sName, SceneManager* cpSceneManager, Camera* cpCamera)
	{
		// Set the provided variables
		msName = sName;
		mcpSceneManager = cpSceneManager;
		mcpCamera = cpCamera;

		// Create the Camera and Target Scene Nodes
		mcpCameraNode = mcpSceneManager->getRootSceneNode()->createChildSceneNode(msName);
		mcpTargetNode = mcpSceneManager->getRootSceneNode()->createChildSceneNode(msName + "Target");

		// Turn on Camera Auto Tracking to have it always face the Target automatically
		mcpCameraNode->setAutoTracking(true, mcpTargetNode);
		mcpCameraNode->setFixedYawAxis(true);

		// Attach the Camera to the Camera Node
		mcpCameraNode->attachObject(mcpCamera);

		// Set the default Tightness values
		mfCameraTightness = mfTargetTightness = 0.07f;

		// Set the default Max Offsets
		mfMaxCameraOffsetDistance = 1000.0f;
		mfMaxTargetOffsetAngle = 30.0f;
	}

	// Delete all allocated data
	void Purge()
	{
		// If the CameraNode exists
		if (mcpCameraNode != NULL)
		{
			// Detach the Camera from the CameraNode
			mcpCameraNode->detachAllObjects();
			mcpCameraNode = NULL;
		}

		// If we have a handle to the Scene Manager
		if (mcpSceneManager != NULL)
		{
			// Destroy the created Camera and Target Nodes
			mcpSceneManager->destroySceneNode(msName);
			mcpSceneManager->destroySceneNode(msName + "Target");
			mcpSceneManager = NULL;
		}
	}

	// Destructor
	~CCameraSystem()
	{
		Purge();
	}
	
	// Set the Cameras Tightness (how fast it gets to the position it should be in)
	// NOTE: This should be a value between 0.0 and 1.0
	void SetCameraTightness(float fTightness)
	{	
		// Make sure the specified Tightness is a valid value
		if (fTightness < 0.0)
		{
			fTightness = 0.0;
		}
		else if (fTightness > 1.0)
		{
			fTightness = 1.0;
		}
		mfCameraTightness = fTightness;
	}

	// Get the Cameras Tightness
	float GetCameraTightness() const
	{
		return mfCameraTightness;
	}

	// Set the Targets Tightness (how fast the camera should follow the targets movement)
	// NOTE: This should be a value between 0.0 and 1.0
	void SetTargetTightness(float fTightness)
	{
		// Make sure the specified Tightness is a valid value
		if (fTightness < 0.0)
		{
			fTightness = 0.0;
		}
		else if (fTightness > 1.0)
		{
			fTightness = 1.0;
		}
		mfTargetTightness = fTightness;
	}

	// Get the Targets Tightness
	float GetTargetTightness() const
	{
		return mfTargetTightness;
	}

	// Set the Max Camera Offset (max distance it can be away from where it should be)
	void SetMaxCameraOffsetDistance(float fDistance)
	{
		mfMaxCameraOffsetDistance = fDistance;
	}

	// Get the Max Camera Offset
	float GetMaxCameraOffsetDistance() const
	{
		return mfMaxCameraOffsetDistance;
	}

	// Set the Max Target Offset Angle (max angle between the Target and the Desired Target with respect to the Camera)
	// NOTE: The given angle should be between 0 and 180 degrees
	void SetMaxTargetOffsetAngle(float fAngleInDegrees)
	{
		mfMaxTargetOffsetAngle = fAngleInDegrees;
	}

	// Get the Max Target Offset
	float GetMaxTargetOffsetAngle() const
	{
		return mfMaxTargetOffsetAngle;
	}

	// Get the Cameras Position
	Vector3 GetCameraPosition() const
	{
		return mcpCameraNode->getWorldPosition();
	}

	// Set the Cameras position to the position specified
	void SetCameraPosition(Vector3 cCameraPosition)
	{
		mcpCameraNode->setPosition(cCameraPosition);
	}

	// Set the Targets position to the position specified
	void SetTargetPosition(Vector3 cTargetPosition)
	{
		mcpTargetNode->setPosition(cTargetPosition);
	}

	// Update the Camera position and direction it is pointing
	void Update(Vector3 sDesiredCameraPosition, Vector3 sDesiredTargetPosition, float fTimePassedInSeconds)
	{
		Vector3 sDisplacement = Vector3::ZERO;			// Vector to help handle Camera and Target movement
		Vector3 sCameraToTarget = Vector3::ZERO;		// Holds the vector from the Camera to the actual Target
		Vector3 sCameraToDesiredTarget = Vector3::ZERO;	// Holds the vector from the Camera to the Desired Target
		float fAngle = 0.0f;							// Holds the Angle between the Target and the Desired Target, with respect to the Camera

		// Calculate the Camera and Target Tightness to use based on how long it's been since the last update
		// NOTE: Assumes "normal" frame rate is 30 fps
		float fCameraTightness = mfCameraTightness * (fTimePassedInSeconds * 30.0);
		float fTargetTightness = mfTargetTightness * (fTimePassedInSeconds * 30.0);

		// Make sure the Camera and Target Tightness are still within range (between 0.0 and 1.0)
		fCameraTightness = (fCameraTightness > 1.0) ? 1.0 : fCameraTightness;
		fTargetTightness = (fTargetTightness > 1.0) ? 1.0 : fTargetTightness;


		// Update the Cameras position

		// Get the displacement of where the Camera will be with respect to where it should be
		sDisplacement = (sDesiredCameraPosition - mcpCameraNode->getWorldPosition()) * fCameraTightness;

		// If the Camera will be further away from where it should be than allowed
		if (sDisplacement.length() > mfMaxCameraOffsetDistance)
		{
			// Move the Camera to the max distance away from where it should be
			sDisplacement.normalise();
			sDisplacement *= mfMaxCameraOffsetDistance;
		}

		// Move the Camera
		mcpCameraNode->translate(sDisplacement);


		// Update the Targets position

		// Get the displacement of where the Target will be with respect to where it should be
		sDisplacement = (sDesiredTargetPosition - mcpTargetNode->getPosition()) * fTargetTightness;

		// Move the Target
		mcpTargetNode->translate(sDisplacement);

		// Check to make sure the Targets position is within range of the max allowed

		// Get the vectors of the Camera to the Target and Camera to the Desired Target
		sCameraToTarget = mcpTargetNode->getWorldPosition() - mcpCameraNode->getWorldPosition();
		sCameraToDesiredTarget = sDesiredTargetPosition - mcpCameraNode->getWorldPosition();

		// Calculate the angle between the Target being used and the Desired Target with respect to the Camera
		sCameraToTarget.normalise();
		sCameraToDesiredTarget.normalise();
		fAngle = abs((float)Math::ACos(sCameraToTarget.dotProduct(sCameraToDesiredTarget)).valueDegrees());

		// If the new Target position still results in a larger angle between the Target and the Desired Target than allowed
		if (fAngle > mfMaxTargetOffsetAngle)
		{
			// Find the Ratio of how large the Angle is to how large it should be at its max
			// NOTE: Since fAngle > mfMaxTargetOffsetAngle, fRatio will always be between 0.0 and 1.0
			float fRatio = mfMaxTargetOffsetAngle / fAngle;

			// Calculate the Direction and Distance the Target has to move to be within allowed range
			sDisplacement = sDesiredTargetPosition - mcpTargetNode->getPosition();
			sDisplacement *= (1.0 - fRatio);

			// Move the Target to the max angle allowed position
			mcpTargetNode->translate(sDisplacement);
		}
	}
};

#endif
