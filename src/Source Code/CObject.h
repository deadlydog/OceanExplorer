/////////////////////////////////////////////////////////////////////
// CObject.h (and CObject.cpp)
// Written by Daniel Schroeder
// Created on July 22 2004
// Last Updated September 26, 2007
//-------------------------------------------------------------------
//   The CObject class is used to hold all information about an Object, 
// including the objects position, velocity, acceleration, and type. If 
// you want more information than what's provided in the CObject class,
// you can include other information class instances (such as CAttributes.h
// for example) in the CObject class. After setting all of the objects 
// speeds (Velocity or Acceleration), call the Update() function 
// each frame to update the objects position.
/////////////////////////////////////////////////////////////////////

#ifndef COBJECT_H
#define COBJECT_H

#include "CAIHelper.h"
#include "CAttributes.h"
#include <Ogre.h>			// Include OGRE
#include <deque>			// Double-Ended Queue used to hold list of Destinations
using namespace std;


// Object States
const int GiOBJECT_STATE_DEAD				= -1;
const int GiOBJECT_STATE_NONE				= 0;
const int GiOBJECT_STATE_TRACKING_PLAYER	= 1;
const int GiOBJECT_STATE_APPROACHING_PLAYER	= 2;
const int GiOBJECT_STATE_EXPLORING			= 3;
const int GiOBJECT_STATE_TRACKING_FISH		= 4;
const int GiOBJECT_STATE_AVOID_PLAYER		= 5;
const int GiOBJECT_STATE_AVOID_SHARK		= 6;
const int GiOBJECT_STATE_FLOCKING			= 7;


// General Object Types
const int GiOBJECT_GENERAL_TYPE_PLAYER		= 0;
const int GiOBJECT_GENERAL_TYPE_ENEMY		= 1;
const int GiOBJECT_GENERAL_TYPE_ITEM		= 2;
const int GiOBJECT_GENERAL_TYPE_OTHER		= 3;


// Specific Object Types
const int GiOBJECT_SPECIFIC_TYPE_NONE				= 0;
const int GiOBJECT_SPECIFIC_TYPE_SHARK				= 1;

// Fish that use flocking
const int GiOBJECT_SPECIFIC_TYPE_FISH_INDEX_START	= 2;
const int GiOBJECT_SPECIFIC_TYPE_FLOCKING_FISH_INDEX_START = 2;
const int GiOBJECT_SPECIFIC_TYPE_DANS_FISH			= 2;
const int GiOBJECT_SPECIFIC_TYPE_BLUE_TANGO_FISH	= 3;
const int GiOBJECT_SPECIFIC_TYPE_BLUE_FISH			= 4;
const int GiOBJECT_SPECIFIC_TYPE_GOLDEN_FISH		= 5;
const int GiOBJECT_SPECIFIC_TYPE_PAREXO_FISH		= 6;
const int GiOBJECT_SPECIFIC_TYPE_FLOCKING_FISH_INDEX_END = 6;

// Fish that don't use flocking
const int GiOBJECT_SPECIFIC_TYPE_NON_FLOCKING_FISH_INDEX_START = 7;
const int GiOBJECT_SPECIFIC_TYPE_DANS_OCTOPUS		= 7;
const int GiOBJECT_SPECIFIC_TYPE_TURTLE				= 8;
const int GiOBJECT_SPECIFIC_TYPE_KILLER_WHALE		= 9;
const int GiOBJECT_SPECIFIC_TYPE_NON_FLOCKING_FISH_INDEX_END = 9;
const int GiOBJECT_SPECIFIC_TYPE_FISH_INDEX_END		= 9;

// Specific Item types
const int GiOBJECT_SPECIFIC_TYPE_ITEM_INDEX_START	= 100;
const int GiOBJECT_SPECIFIC_TYPE_RING				= 100;
const int GiOBJECT_SPECIFIC_TYPE_DIAMOND			= 101;
const int GiOBJECT_SPECIFIC_TYPE_ITEM_INDEX_END		= 101;


// Structure to hold a Destination to automatically track
struct SDestination
{
public:
	// Only one of these 2 should ever be set, the other should be NULL
	Ogre::SceneNode* cpSceneNode;	// If we are tracking a Scene Node, it holds the Scene Node to track
	Ogre::Vector3* cpVector;		// If we are tracking a point in space, it holds the point to track

	// Explicit Constructor - set just one of the pointers to track
	SDestination(Ogre::SceneNode* _cpSceneNodeToTrack = NULL, Ogre::Vector3* _cpPointInWorldCoordinatesToTrack = NULL)
	{
		// Record the Scene Node to track
		cpSceneNode = _cpSceneNodeToTrack;

		// If an actual Scene Node was given to track, or no valid Vector was supplied
		if (cpSceneNode != NULL || _cpPointInWorldCoordinatesToTrack == NULL)
		{
			// Make sure Vector is Null
			cpVector = NULL;
		}
		// Else no Scene Node was given to track and Vector supplied is valid
		else
		{
			// So create a copy of the Vector to track
			cpVector = new Ogre::Vector3(*_cpPointInWorldCoordinatesToTrack);
		}
	}

	// Destructor - Destroys the Vector but not the Scene Node (because we don't know what's attached 
	//	to the Scene Node)
	~SDestination()
	{
		// If we initialized a new Vector
		if (cpVector != NULL)
		{
			delete cpVector;
			cpVector = NULL;
		}
	}

	// Overload the = operator
	SDestination& operator =(const SDestination& sDestination)
	{
		cpSceneNode = sDestination.cpSceneNode;

		// If there is no Vector to copy
		if (sDestination.cpVector == NULL)
		{
			cpVector = NULL;
		}
		// Else there is a Vector to copy
		else
		{
			cpVector = new Ogre::Vector3(*sDestination.cpVector);
		}
	}
};

// Structure to hold an external force on the Object
struct SExternalForce
{
public:
	Ogre::Vector3 sForce;					// External Force vector to be applied to object (Force per second)
	Ogre::Node::TransformSpace sTransformSpace;	// Transform Space the Force should be applied in
	int iID;								// ID of this External Force object
	
	// Default constructor
	SExternalForce(Ogre::Vector3 _sForcePerSecond, int _iID, Ogre::Node::TransformSpace _TransformSpace = Ogre::Node::TS_WORLD)
	{
		sForce = _sForcePerSecond;
		sTransformSpace = _TransformSpace;
		iID = _iID;
	}

	// Comparison operators
	bool operator ==(const SExternalForce& sForce)
	{
		return (iID == sForce.iID);
	}
	bool operator !=(const SExternalForce& sForce)
	{
		return (iID != sForce.iID);
	}
};


// Holds all information of a Object
class CObject
{
public:
	
	//---------- Contructors and Copy Functions ----------

	CObject();											// Default Constructor
	~CObject();											// Destructor

	CObject(CObject const& cObjectToCopyFrom);			// Copy Constructor
	CObject& operator =(const CObject& cObjectToCopyFrom);// Overload the = operator 
	void CopyFrom(CObject const& cObjectToCopyFrom);	// Copy from another object

	// Erase all information as if object was just created
	void Purge();



	// Ogre Specific functions

	// Set/Return what this Objects Translation (relative movement) should be Relative To
	void SetWhatTranslationShouldBeRelativeTo(Ogre::Node::TransformSpace RelativeTo = Ogre::Node::TS_PARENT);
	Ogre::Node::TransformSpace GetWhatTranslationIsRelativeTo() const;

	// Set/Return what this Objects Pitch (X-axis rotation) should be Relative To
	void SetWhatPitchShouldBeRelativeTo(Ogre::Node::TransformSpace RelativeTo = Ogre::Node::TS_PARENT);
	Ogre::Node::TransformSpace GetWhatPitchIsRelativeTo() const;

	// Set/Return what this Objects Yaw (Y-axis rotation) should be Relative To
	void SetWhatYawShouldBeRelativeTo(Ogre::Node::TransformSpace RelativeTo = Ogre::Node::TS_PARENT);
	Ogre::Node::TransformSpace GetWhatYawIsRelativeTo() const;

	// Set/Return what this Objects Roll (Z-axis rotation) should be Relative To
	void SetWhatRollShouldBeRelativeTo(Ogre::Node::TransformSpace RelativeTo = Ogre::Node::TS_PARENT);
	Ogre::Node::TransformSpace GetWhatRollIsRelativeTo() const;

	void SetWhatAllMovementsShouldBeRelativeTo(Ogre::Node::TransformSpace TranslationRelativeTo = Ogre::Node::TS_PARENT,
											   Ogre::Node::TransformSpace PitchRelativeTo = Ogre::Node::TS_PARENT,
											   Ogre::Node::TransformSpace YawRelativeTo = Ogre::Node::TS_PARENT,
											   Ogre::Node::TransformSpace RollRelativeTo = Ogre::Node::TS_PARENT);


	//---------- Default 3D Mesh Orientation Functions ----------

	// NOTE: These should always be set before doing anything

	// Set/Get which way the Object model/mesh faces by default, so we know which way is forward
	void SetMeshesDefaultForwardDirection(Ogre::Vector3 _sForwardDirection = Ogre::Vector3::UNIT_Z);
	Ogre::Vector3 GetMeshesDefaultForwardDirection() const;

	// Set/Get which way is Up on the model/mesh
	void SetMeshesDefaultUpDirection(Ogre::Vector3 _sUpDirection = Ogre::Vector3::UNIT_Y);
	Ogre::Vector3 GetMeshesDefaultUpDirection() const;

	// Set/Get which way is Right on the model/mesh
	void SetMeshesDefaultRightDirection(Ogre::Vector3 _sRightDirection = Ogre::Vector3::UNIT_X);
	Ogre::Vector3 GetMeshesDefaultRightDirection() const;


	//---------- Manual Movement Functions ----------

	// NOTE: All movement speeds are in terms of Pixels per Second

	// Set/Get Velocity (Magnitude of vector is used as Speed)
	void SetVelocity(Ogre::Vector3 sVelocity);
	void SetVelocity(Ogre::Vector3 sDirection, float fSpeed);
	Ogre::Vector3 GetVelocityDirection() const;
	Ogre::Vector3 GetVelocity() const;

	// Sets the Objects Velocity to the sum of its current Velocity and the given Velocity
	void SetVelocityRelative(Ogre::Vector3 sVelocityToAdd);

	// Set/Get Speed
	void SetVelocitySpeed(float fSpeed);
	float GetVelocitySpeed() const;

	// Set/Get Min Speed
	void SetMinVelocitySpeed(float fMinSpeed);
	float GetMinVelocitySpeed() const;

	// Set/Get Max Speed
	void SetMaxVelocitySpeed(float fMaxSpeed);
	float GetMaxVelocitySpeed() const;

	// Set/Get Default Speed
	void SetDefaultVelocitySpeed(float fDefaultSpeed);
	float GetDefaultVelocitySpeed() const;


	// Set/Get Acceleration (Magnitude of vector is used as AccelerationRate)
	// Acceleration is used to increase/decrease the velocity every time Update() is called
	// If you do not want the Object to speed up/slow down, set Accelerations to zero
	// NOTE: Requires Update() to be called each frame to actually move the object
	// NOTE: When Update() is called Velocity is automatically increased/decreased by current Acceleration
	void SetAcceleration(Ogre::Vector3 sAcceleration);
	void SetAcceleration(Ogre::Vector3 sDirection, float fAccelerationRate);
	Ogre::Vector3 GetAccelerationDirection() const;
	Ogre::Vector3 GetAcceleration() const;

	// Set the Objects Acceleration to the sum of its current Acceleration and the given Acceleration
	void SetAccelerationRelative(Ogre::Vector3 sAccelerationToAdd);

	// Set/Get the Acceleration Rate
	void SetAccelerationRate(float fRate);
	float GetAccelerationRate() const;

	// Set/Get Min Accelertion Rate
	void SetMinAccelerationRate(float fMinRate);
	float GetMinAccelerationRate() const;

	// Set/Get Max Acceleration Rate
	void SetMaxAccelerationRate(float fMaxRate);
	float GetMaxAccelerationRate() const;

	// Set/Get Default Acceleration Rate
	void SetDefaultAccelerationRate(float fDefaultRate);
	float GetDefaultAccelerationRate() const;

	// Set/Get if Movement Accelerations should be used or not (true by default)
	void SetIfMovementAccelerationShouldBeUsed(bool _bUseMovementAcceleration = true);
	bool GetIfMovementAccelerationIsBeingUsed() const;


	// Set/Get Friction
	// Frictions will slow the object down no matter what direction it is going
	// Zero friction means the object will not slow down on its own
	// NOTE: Requires Update() to be called each frame to have an effect
	// NOTE: Specified friction X will slow object down by X pixels per second
	void SetFriction(float fFrictionPerSecond);
	float GetFriction() const;

	// Set/Get if Movement Friction should be used or not (true by default)
	void SetIfMovementFrictionShouldBeUsed(bool bUseMovementFriction = true);
	bool GetIfMovementFrictionIsBeingUsed() const;


	//---------- Manual Rotation Functions ----------

	// NOTE: Vectors X direction corresponds to Pitch, Y to Yaw, and Z to Roll
	// NOTE: Rotation angles are in terms of Degrees per Second

	// Set/Get Rotation Velocity (Magnitude of vector is used as number of Degrees to Rotate Per Second)
	void SetRotationVelocity(Ogre::Vector3 sRotationVelocity);
	void SetRotationVelocity(Ogre::Vector3 sRotation, float fRotationSpeedInDegreesPerSecond);
	Ogre::Vector3 GetRotationVelocityDirection() const;
	Ogre::Vector3 GetRotationVelocity() const;

	// Set the Objects Rotation Velocity to the sum of its current Rotation Velocity and the given Velocity
	void SetRotationVelocityRelative(Ogre::Vector3 sVelocityToAdd);

	// Set/Get Rotation Speed
	void SetRotationVelocitySpeed(float fSpeedInDegreesPerSecond);
	float GetRotationVelocitySpeed() const;

	// Set/Get the Min Rotation Angle to Rotate per Second
	void SetMinRotationVelocitySpeed(float fSpeedInDegreesPerSecond);
	float GetMinRotationVelocitySpeed() const;

	// Set/Get the Max Rotation Angle to Rotate per Second
	void SetMaxRotationVelocitySpeed(float fSpeedInDegreesPerSecond);
	float GetMaxRotationVelocitySpeed() const;

	// Set/Get the Default Rotation Angle to Rotate per Second
	void SetDefaultRotationVelocitySpeed(float fSpeedInDegreesPerSecond);
	float GetDefaultRotationVelocitySpeed() const;


	// Set/Get Rotation Acceleration (Magnitude of vector is used as number of Degrees to Accelerate Per Second)
	// Acceleration is used to increase/decrease the velocity every time Update() is called
	// If you do not want the Object to speed up/slow down, set Accelerations to zero
	// NOTE: Requires Update() to be called each frame to actually move the object
	// NOTE: When Update() is called Velocity is automatically increased/decreased by current Acceleration
	void SetRotationAcceleration(Ogre::Vector3 sRotationAcceleration);
	void SetRotationAcceleration(Ogre::Vector3 sRotation, float fRotationAccelerationInDegreesPerSecond);
	Ogre::Vector3 GetRotationAccelerationDirection() const;
	Ogre::Vector3 GetRotationAcceleration() const;

	// Set the Objects Rotation Acceleration to the sum of its current Rotation Acceleration and the given Acceleration
	void SetRotationAccelerationRelative(Ogre::Vector3 sAccelerationToAdd);

	// Set/Get Rotation Acceleration Rate
	void SetRotationAccelerationRate(float fAccelerationInDegreesPerSecond);
	float GetRotationAccelerationRate() const;

	// Set/Get the Min Rotation Angle to Accelerate by per Second
	void SetMinRotationAccelerationRate(float fAccelerationInDegreesPerSecond);
	float GetMinRotationAccelerationRate() const;

	// Set/Get the Max Rotation Angle to Accelerate by per Second
	void SetMaxRotationAccelerationRate(float fAccelerationInDegreesPerSecond);
	float GetMaxRotationAccelerationRate() const;

	// Set/Get the Default Rotation Angle to Accelerate by per Second
	void SetDefaultRotationAccelerationRate(float fAccelerationInDegreesPerSecond);
	float GetDefaultRotationAccelerationRate() const;

	// Set/Get if Rotation Accelerations should be used or not (true by default)
	void SetIfRotationAccelerationShouldBeUsed(bool bUseRotationAcceleration = true);
	bool GetIfRotationAccelerationIsBeingUsed() const;


	// Set/Get Rotation Friction
	// Rotation Frictions will slow the objects rotation
	// Zero friction means the object will not slow down on its own
	// NOTE: Requires Update() to be called each frame to have an effect
	// NOTE: Specified friction Pitch will slow object down by Pitch pixels per second
	void SetRotationFriction(float fRotationFriction);
	float GetRotationFriction() const;

	// Set/Get if Rotation Friction should be used or not (true by default)
	void SetIfRotationFrictionShouldBeUsed(bool bUseRotationFriction = true);
	bool GetIfRotationFrictionIsBeingUsed() const;


	// Updates Objects Position and Orientation
	// Moves and Rotates Object by specified Velocity amounts, which is calculated using the Acceleration values, and
	//	also applies friction and external forces
	// NOTE: This should be called every frame
	// NOTE: ulAmountOfTimeSinceLastUpdate should be in milliseconds (1000ms = 1s)
	// NOTE: fAmountOfTimeSinceLastUpdateInSeconds should be in seconds
	void Update(unsigned long ulAmountOfTimeSinceLastUpdate);
	void Update(float fAmountOfTimeSinceLastUpdateInSeconds);


	// ---------- Easy Rotation Functions ----------

	// Rotates the Object to face the given Location
	void PointObjectTowardsLocation(Ogre::Vector3 _sLocation, bool _bImmediately = false);

	// Rotates the Object to face the given Direction
	void PointObjectInDirection(Ogre::Vector3 _sDirection, bool _bImmediately = false);

	// Returns the unit Direction the Object is currently facing
	Ogre::Vector3 GetDirectionObjectIsFacing() const;


	// ---------- Automatic Tracking Functions ----------

	// Set/Get the dummy Scene Node to use to track Vectors (a static position)
	// NOTE: This Scene Node should not be used by anything other than this class and should be a child
	//		  of the root Scene Node
	void SetExclusiveSceneNodeToUseForAutomaticTracking(Ogre::SceneNode* _cpExclusiveSceneNode);
	Ogre::SceneNode* GetExclusiveSceneNodeUsedForAutomaticTracking() const;

	// Sets if the Object should move towards it's Destinations automatically (true) or if the Object should
	//	have free control to move itself around
	// NOTE: If true, any calls to SetAcceleration/Velocity will be ignored until set back to false
	// NOTE: The Set() functions return false if they do not have a Destination to go to (ie the DestinationList is empty) 
	bool SetIfObjectShouldMoveToDestinationAutomatically(bool _bMoveToDestination = true, bool _bFaceDestination = true);
	bool SetIfObjectShouldFaceDestinationAutomatically(bool _bFaceDestination = true);
	bool GetIfObjectShouldMoveToDestinationAutomatically() const;
	bool GetIfObjectShouldFaceDestinationAutomatically() const;

	// Gets the Next Destination from the Destination List
	// Returns true if another Destination was found, false if not
	bool GetNextDestination();

	// Return the current Destination to move to/face
	Ogre::SceneNode* GetCurrentDestination() const;

	// Returns if the Object has reached all of it's Destinations or not
	bool GetIfObjectHasReachedAllDestinations() const;

	// Points the Object towards the current Destination if one exists
	bool PointObjectTowardsDestination();


	//---------- Objects Properties Functions ----------

	// Set/Get Objects ID
	void SetID(int _iID);
	int GetID() const;

	// Set/Get the Objects Name
	void SetName(Ogre::String _sName);
	Ogre::String GetName() const;

	// Set/Get Objects General Type (ie Player or Enemy)
	void SetGeneralObjectType(int iType);
	int GetGeneralObjectType() const;

	// Set/Get Objects Specific Type (ie Which Player or type of Enemy)
	void SetSpecificObjectType(int iType);
	int GetSpecificObjectType() const;

	// Set/Get the Objects State
	void SetState(int iState = GiOBJECT_STATE_NONE);
	int GetState() const;

	// Set/Get if Object is colliding with another Object or not
	void SetIfObjectIsColliding(bool bIsCollision = true);
	bool GetIfObjectIsColliding() const;


	//---------- Other Functions and Objects ----------

	// Ogre Objects that hold entities physical appearance and position
	Ogre::Entity* mcpEntity;		// Holds the Objects Entity (mesh/model)
	Ogre::SceneNode* mcpSceneNode;	// Holds the Objects Scene Node (position/orientation)
	Ogre::AnimationState* mcpAnimationState;	// Controls the animation of the Object

	deque<SDestination*> mcDestinationList;		// Holds a list of Destinations the Object should go to
	list<SExternalForce> mcExternalForceList;	// Holds a list of External Forces to apply to the Object

	CAttributes mcAttributes;		// Holds an Objects Attributes (health, lives, points, etc)
	CAIHelper mcAIHelper;			// Holds the AI used to control this Object

protected:

	//---------- Default Mesh Orientation Data ----------

	Ogre::Vector3 msMeshForwardDirection;	// Holds the meshes default Forward direction
	Ogre::Vector3 msMeshUpDirection;		// Holds the meshes default Up direction
	Ogre::Vector3 msMeshRightDirection;		// Holds the meshes default Right direction


	//---------- Movement Data ----------

	Ogre::Vector3 msUnitVelocity, msUnitAcceleration;
	float mfSpeed, mfAccelerationRate, mfFriction;
	float mfMinVelocity, mfMaxVelocity, mfDefaultVelocity;
	float mfMinAcceleration, mfMaxAcceleration, mfDefaultAcceleration;
	bool mbUseMovementAcceleration, mbUseMovementFriction;		// Tells if Movement Acceleration/Friction should be calculated in Update()

	Ogre::Vector3 msUnitRotationVelocity, msUnitRotationAcceleration;
	float mfRotationSpeed, mfRotationAccelerationRate, mfRotationFriction;
	float mfMinRotationVelocity, mfMaxRotationVelocity, mfDefaultRotationVelocity;
	float mfMinRotationAcceleration, mfMaxRotationAcceleration, mfDefaultRotationAcceleration;
	bool mbUseRotationAcceleration, mbUseRotationFriction;		// Tells if Rotation Acceleration/Friction should be calculated in Update()

	// Ogre specific data - tells what this objects movements should be relative to (TS_LOCAL, TS_PARENT, or TS_WORLD)
	Ogre::Node::TransformSpace mTranslationRelativeTo;	// What the Translation (relative movement) should be Relative To
	Ogre::Node::TransformSpace mPitchRelativeTo;		// What the X-axis rotations should be Relative To
	Ogre::Node::TransformSpace mYawRelativeTo;			// What the Y-axis rotations should be Relative To
	Ogre::Node::TransformSpace mRollRelativeTo;			// What the Z-axis rotations should be Relative To


	// ---------- Automatic Tracking Data ----------

	Ogre::SceneNode* mcpAutoTrackingSceneNode;	// A Scene Node used exclusively by this Object to track a given Vector Destination

	Ogre::SceneNode* mcpDestination;		// The Destination the Object is moving to
	float mfDistanceToDestination;			// The Distance between the Object and it's current Destination
	Ogre::Vector3 msDirectionToDestination;	// The Direction from the Object to the Destination

	bool mbMoveToDestinationAutomatically;	// Tells if Update() should move the Object towards it's Destination, or let it control itself
	bool mbFaceDestinationAutomatically;	// Tells if Update() should make the Object face it's Destination, or let it control itself
	bool mbDestinationReached;				// Tells if the Object has reached it's Destination or not


	//---------- Properties Data ----------

	string msName;					// Name of the Object
	int miID;						// ID of the Object
	int miGeneralObjectType;		// General Type of Object
	int miSpecificObjectType;		// Specific Type of Object
	int miState;					// Holds the current State of the Object
	bool mbCollision;				// Tells if Object is colliding with other Objects

	float mfDurationSinceLastUpdateInSeconds;	// Holds how much time passed between the 2nd last Update() and the last Update() performed
};

#endif
