#include "CObject.h"
#include <cmath>		// Needed for floorf() function	
using namespace std;


//---------- Contructors and Copy Functions ----------

// Default Constructor
CObject::CObject()
{
	// Initialize all data
	Purge();
}

// Destructor
CObject::~CObject()
{
	Purge();
}

// Copy Constructor
CObject::CObject(CObject const& cObjectToCopyFrom)
{
	// Copy data over from other object
	CopyFrom(cObjectToCopyFrom);
}

// Overload the = operator
CObject& CObject::operator =(const CObject& cObjectToCopyFrom)
{
	// Copy data over from other object
	CopyFrom(cObjectToCopyFrom);

	return *this;
}

// Copy from another object
void CObject::CopyFrom(CObject const& cObjectToCopyFrom)
{
	// Erase all info from this Object
	Purge();

	// Copy Public data
	mcpEntity = cObjectToCopyFrom.mcpEntity;
	mcpSceneNode = cObjectToCopyFrom.mcpSceneNode;
	mcpAnimationState = cObjectToCopyFrom.mcpAnimationState;
	mcDestinationList = cObjectToCopyFrom.mcDestinationList;
	mcExternalForceList = cObjectToCopyFrom.mcExternalForceList;
	mcAttributes.CopyFrom(cObjectToCopyFrom.mcAttributes);
	mcAIHelper.CopyFrom(cObjectToCopyFrom.mcAIHelper);

	// Copy Private data

	msMeshForwardDirection = cObjectToCopyFrom.GetMeshesDefaultForwardDirection();
	msMeshUpDirection = cObjectToCopyFrom.GetMeshesDefaultUpDirection();
	msMeshRightDirection = cObjectToCopyFrom.GetMeshesDefaultRightDirection();

	// Copy Movement data
	msUnitVelocity = cObjectToCopyFrom.GetVelocityDirection();
	msUnitAcceleration = cObjectToCopyFrom.GetAccelerationDirection();
	mfSpeed = cObjectToCopyFrom.GetVelocitySpeed();
	mfAccelerationRate = cObjectToCopyFrom.GetAccelerationRate();
	mfFriction = cObjectToCopyFrom.GetFriction();
	mfMinVelocity = cObjectToCopyFrom.GetMinVelocitySpeed();
	mfMaxVelocity = cObjectToCopyFrom.GetMaxVelocitySpeed();
	mfDefaultVelocity = cObjectToCopyFrom.GetDefaultVelocitySpeed();
	mfMinAcceleration = cObjectToCopyFrom.GetMinAccelerationRate();
	mfMaxAcceleration = cObjectToCopyFrom.GetMaxAccelerationRate();
	mfDefaultAcceleration = cObjectToCopyFrom.GetDefaultAccelerationRate();
	mbUseMovementAcceleration = cObjectToCopyFrom.GetIfMovementAccelerationIsBeingUsed();
	mbUseMovementFriction = cObjectToCopyFrom.GetIfMovementFrictionIsBeingUsed();

	msUnitRotationVelocity = cObjectToCopyFrom.GetRotationVelocityDirection();
	msUnitRotationAcceleration = cObjectToCopyFrom.GetRotationAccelerationDirection();
	mfRotationSpeed = cObjectToCopyFrom.GetRotationVelocitySpeed();
	mfRotationAccelerationRate = cObjectToCopyFrom.GetRotationAccelerationRate();
	mfRotationFriction = cObjectToCopyFrom.GetRotationFriction();;
	mfMinRotationVelocity = cObjectToCopyFrom.GetMinRotationVelocitySpeed();
	mfMaxRotationVelocity = cObjectToCopyFrom.GetMaxRotationVelocitySpeed();
	mfDefaultRotationVelocity = cObjectToCopyFrom.GetDefaultRotationVelocitySpeed();
	mfMinRotationAcceleration = cObjectToCopyFrom.GetMinRotationAccelerationRate();
	mfMaxRotationAcceleration = cObjectToCopyFrom.GetMaxRotationAccelerationRate();
	mfDefaultRotationAcceleration = cObjectToCopyFrom.GetDefaultRotationAccelerationRate();
	mbUseRotationAcceleration = cObjectToCopyFrom.GetIfRotationAccelerationIsBeingUsed();
	mbUseRotationFriction = cObjectToCopyFrom.GetIfRotationFrictionIsBeingUsed();

	mTranslationRelativeTo = cObjectToCopyFrom.GetWhatTranslationIsRelativeTo();
	mPitchRelativeTo = cObjectToCopyFrom.GetWhatPitchIsRelativeTo();
	mYawRelativeTo = cObjectToCopyFrom.GetWhatYawIsRelativeTo();
	mRollRelativeTo = cObjectToCopyFrom.GetWhatRollIsRelativeTo();

	// Copy Automatic Tracking data	
	mcpAutoTrackingSceneNode = cObjectToCopyFrom.GetExclusiveSceneNodeUsedForAutomaticTracking();
	mcpDestination = GetCurrentDestination();
	mbMoveToDestinationAutomatically = cObjectToCopyFrom.GetIfObjectShouldMoveToDestinationAutomatically();
	mbFaceDestinationAutomatically = cObjectToCopyFrom.GetIfObjectShouldFaceDestinationAutomatically();

	// Copy Properties data
	msName = cObjectToCopyFrom.GetName();
	miID = cObjectToCopyFrom.GetID();
	miGeneralObjectType = cObjectToCopyFrom.GetGeneralObjectType();
	miSpecificObjectType = cObjectToCopyFrom.GetSpecificObjectType();
	miState = cObjectToCopyFrom.GetState();
	mbCollision = cObjectToCopyFrom.GetIfObjectIsColliding();
}

// Erase all info as if Object was just created
void CObject::Purge()
{
	// Reset public data
	mcpEntity = NULL;
	mcpSceneNode = NULL;
	mcpAnimationState = NULL;
	mcDestinationList.clear();
	mcExternalForceList.clear();
	mcAttributes.Purge();
	mcAIHelper.Purge();
	
	// Reset private data

	msMeshForwardDirection = msMeshUpDirection = msMeshRightDirection = Ogre::Vector3::ZERO;

	msUnitVelocity = msUnitAcceleration = Ogre::Vector3::ZERO;
	mfSpeed = mfAccelerationRate = mfFriction = 0.0f;
	mfMinVelocity = mfMaxVelocity = mfDefaultVelocity = 0.0f;
	mfMinAcceleration = mfMaxAcceleration = mfDefaultAcceleration = 0.0f;
	mbUseMovementAcceleration = mbUseMovementFriction = true;

	msUnitRotationVelocity = msUnitRotationAcceleration = Ogre::Vector3::ZERO;
	mfRotationSpeed = mfRotationAccelerationRate = mfRotationFriction = 0.0f;
	mfMinRotationVelocity = mfMaxRotationVelocity = mfDefaultRotationVelocity = 0.0f;
	mfMinRotationAcceleration = mfMaxRotationAcceleration = mfDefaultRotationAcceleration = 0.0f;
	mbUseRotationAcceleration = mbUseRotationFriction = true;

	mTranslationRelativeTo = Ogre::Node::TS_LOCAL;
	mPitchRelativeTo = Ogre::Node::TS_LOCAL;
	mYawRelativeTo = Ogre::Node::TS_LOCAL;
	mRollRelativeTo = Ogre::Node::TS_LOCAL;

	mcpAutoTrackingSceneNode = NULL;

	mcpDestination = NULL;
	mfDistanceToDestination = 0.0f;
	msDirectionToDestination = Ogre::Vector3::ZERO;

	mbMoveToDestinationAutomatically = mbFaceDestinationAutomatically = false;
	mbDestinationReached = true; // Set to true initially so we know we're not moving towards a Destination yet

	msName = "";
	miID = miGeneralObjectType = miSpecificObjectType = miState = 0;
	mbCollision = false;

	mfDurationSinceLastUpdateInSeconds = 0.0f;
}



// Ogre Specific functions

// Set what this Objects Translation (relative movement) should be Relative To
void CObject::SetWhatTranslationShouldBeRelativeTo(Ogre::Node::TransformSpace RelativeTo)
{
	mTranslationRelativeTo = RelativeTo;
}

// Return what this Objects Translation (relative movement) should be Relative To
Ogre::Node::TransformSpace CObject::GetWhatTranslationIsRelativeTo() const
{
	return mTranslationRelativeTo;
}

// Set what this Objects Pitch (X-axis rotation) should be Relative To
void CObject::SetWhatPitchShouldBeRelativeTo(Ogre::Node::TransformSpace RelativeTo)
{
	mPitchRelativeTo = RelativeTo;
}

// Return what this Objects Pitch (X-axis rotation) should be Relative To
Ogre::Node::TransformSpace CObject::GetWhatPitchIsRelativeTo() const
{
	return mPitchRelativeTo;
}

// Set what this Objects Yaw (Y-axis rotation) should be Relative To
void CObject::SetWhatYawShouldBeRelativeTo(Ogre::Node::TransformSpace RelativeTo)
{
	mYawRelativeTo = RelativeTo;
}

// Return what this Objects Yaw (Y-axis rotation) should be Relative To
Ogre::Node::TransformSpace CObject::GetWhatYawIsRelativeTo() const
{
	return mYawRelativeTo;
}

// Set what this Objects Roll (Z-axis rotation) should be Relative To
void CObject::SetWhatRollShouldBeRelativeTo(Ogre::Node::TransformSpace RelativeTo)
{
	mRollRelativeTo = RelativeTo;
}

// Return what this Objects Roll (Z-axis rotation) should be Relative To
Ogre::Node::TransformSpace CObject::GetWhatRollIsRelativeTo() const
{
	return mRollRelativeTo;
}

// Set what all of this Objects movements should be Relative To
void CObject::SetWhatAllMovementsShouldBeRelativeTo(Ogre::Node::TransformSpace TranslationRelativeTo,
													Ogre::Node::TransformSpace PitchRelativeTo,
													Ogre::Node::TransformSpace YawRelativeTo,
													Ogre::Node::TransformSpace RollRelativeTo)
{
	mTranslationRelativeTo = TranslationRelativeTo;
	mPitchRelativeTo = PitchRelativeTo;
	mYawRelativeTo = YawRelativeTo;
	mRollRelativeTo = RollRelativeTo;
}


//---------- Default 3D Mesh Orientation Functions ----------

// NOTE: These should always be set before doing anything

// Set which way the Object model/mesh faces by default, so we know which way is forward
void CObject::SetMeshesDefaultForwardDirection(Ogre::Vector3 _sForwardDirection)
{
	_sForwardDirection.normalise();
	msMeshForwardDirection = _sForwardDirection;
}

// Get which way the Object model/mesh faces by default, so we know which way is forward
Ogre::Vector3 CObject::GetMeshesDefaultForwardDirection() const
{
	return msMeshForwardDirection;
}

// Set which way is Up on the model/mesh
void CObject::SetMeshesDefaultUpDirection(Ogre::Vector3 _sUpDirection)
{
	_sUpDirection.normalise();
	msMeshUpDirection = _sUpDirection;
}

// Get which way is Up on the model/mesh
Ogre::Vector3 CObject::GetMeshesDefaultUpDirection() const
{
	return msMeshUpDirection;
}

// Set which way is Right on the model/mesh
void CObject::SetMeshesDefaultRightDirection(Ogre::Vector3 _sRightDirection)
{
	_sRightDirection.normalise();
	msMeshRightDirection = _sRightDirection;
}

// Get which way is Right on the model/mesh
Ogre::Vector3 CObject::GetMeshesDefaultRightDirection() const
{
	return msMeshRightDirection;
}


//---------- Movement Functions ----------

// Set Velocity (Magnitude of vector is used as Speed)
void CObject::SetVelocity(Ogre::Vector3 sVelocity)
{
	mfSpeed = sVelocity.normalise();
	msUnitVelocity = sVelocity;
}

// Set Velocity
void CObject::SetVelocity(Ogre::Vector3 sDirection, float fSpeed)
{
	sDirection.normalise();
	msUnitVelocity = sDirection;
	mfSpeed = fSpeed;
}

// Get Velocity Direction
Ogre::Vector3 CObject::GetVelocityDirection() const
{
	return msUnitVelocity;
}

// Get Velocity with magnitude
Ogre::Vector3 CObject::GetVelocity() const
{
	return msUnitVelocity * mfSpeed;
}

// Set the Objects Velocity to the sum of its current Velocity and the given Velocity
void CObject::SetVelocityRelative(Ogre::Vector3 sVelocityToAdd)
{
	Ogre::Vector3 sVelocity = msUnitVelocity * mfSpeed;
	sVelocity += sVelocityToAdd;
	
	mfSpeed = sVelocity.normalise();
	msUnitVelocity = sVelocity;
}

// Set Speed
void CObject::SetVelocitySpeed(float fSpeed)
{
	mfSpeed = fSpeed;
}

// Get Speed
float CObject::GetVelocitySpeed() const
{
	return mfSpeed;
}

// Set Min Speed
void CObject::SetMinVelocitySpeed(float fMinSpeed)
{
	mfMinVelocity = fMinSpeed;
}

// Get Min Speed
float CObject::GetMinVelocitySpeed() const
{
	return mfMinVelocity;
}

// Set Max Speed
void CObject::SetMaxVelocitySpeed(float fMaxSpeed)
{
	mfMaxVelocity = fMaxSpeed;
}

// Get Max Speed
float CObject::GetMaxVelocitySpeed() const
{
	return mfMaxVelocity;
}

// Set Default Speed
void CObject::SetDefaultVelocitySpeed(float fDefaultSpeed)
{
	mfDefaultVelocity = fDefaultSpeed;
}

// Get Default Speed
float CObject::GetDefaultVelocitySpeed() const
{
	return mfDefaultVelocity;
}


// Set Acceleration (Magnitude of vector is used as AccelerationRate)
// Acceleration is used to increase/decrease the velocity every time Update() is called
// If you do not want the Object to speed up/slow down, set Accelerations to zero
// NOTE: Requires Update() to be called each frame to actually move the object
// NOTE: When Update() is called Velocity is automatically increased/decreased by current Acceleration
void CObject::SetAcceleration(Ogre::Vector3 sAcceleration)
{
	mfAccelerationRate = sAcceleration.normalise();
	msUnitAcceleration = sAcceleration;
}

// Set Acceleration
void CObject::SetAcceleration(Ogre::Vector3 sDirection, float fAccelerationRate)
{
	sDirection.normalise();
	msUnitAcceleration = sDirection;
	mfAccelerationRate = fAccelerationRate;
}

// Get Acceleration Direction
Ogre::Vector3 CObject::GetAccelerationDirection() const
{
	return msUnitAcceleration;
}

// Get Acceleration with magnitude
Ogre::Vector3 CObject::GetAcceleration() const
{
	return msUnitAcceleration * mfAccelerationRate;
}

// Set the Objects Acceleration to the sum of its current Acceleration and the given Acceleration
void CObject::SetAccelerationRelative(Ogre::Vector3 sAccelerationToAdd)
{
	Ogre::Vector3 sAcceleration = msUnitAcceleration * mfAccelerationRate;
	sAcceleration += sAccelerationToAdd;
	
	mfAccelerationRate = sAcceleration.normalise();
	msUnitVelocity = sAcceleration;
}

// Set the Acceleration Rate
void CObject::SetAccelerationRate(float fRate)
{
	mfAccelerationRate = fRate;
}

// Get the Acceleration Rate
float CObject::GetAccelerationRate() const
{
	return mfAccelerationRate;
}

// Set Min Accelertion Rate
void CObject::SetMinAccelerationRate(float fMinRate)
{
	mfMinAcceleration = fMinRate;
}

// Get Min Accelertion Rate
float CObject::GetMinAccelerationRate() const
{
	return mfMinAcceleration;
}

// Set Max Acceleration Rate
void CObject::SetMaxAccelerationRate(float fMaxRate)
{
	mfMaxAcceleration = fMaxRate;
}

// Get Max Acceleration Rate
float CObject::GetMaxAccelerationRate() const
{
	return mfMaxAcceleration;
}

// Set Default Acceleration Rate
void CObject::SetDefaultAccelerationRate(float fDefaultRate)
{
	mfDefaultAcceleration = fDefaultRate;
}

// Get Default Acceleration Rate
float CObject::GetDefaultAccelerationRate() const
{
	return mfDefaultAcceleration;
}

// Set if Movement Accelerations should be used or not (true by default)
void CObject::SetIfMovementAccelerationShouldBeUsed(bool _bUseMovementAcceleration)
{
	mbUseMovementAcceleration = _bUseMovementAcceleration;
}

// Get if Movement Accelerations should be used or not (true by default)
bool CObject::GetIfMovementAccelerationIsBeingUsed() const
{
	return mbUseMovementAcceleration;
}


// Set Friction
// Frictions will slow the object down no matter what direction it is going
// Zero friction means the object will not slow down on its own
// NOTE: Requires Update() to be called each frame to have an effect
// NOTE: Specified friction X will slow object down by X pixels per second
void CObject::SetFriction(float fFrictionPerSecond)
{
	mfFriction = fFrictionPerSecond;
}

// Get Friction
float CObject::GetFriction() const
{
	return mfFriction;
}

// Set if Movement Friction should be used or not (true by default)
void CObject::SetIfMovementFrictionShouldBeUsed(bool bUseMovementFriction)
{
	mbUseMovementFriction = bUseMovementFriction;
}

// Get if Movement Friction should be used or not (true by default)
bool CObject::GetIfMovementFrictionIsBeingUsed() const
{
	return mbUseMovementFriction;
}


//---------- Manual Rotation Functions ----------

// NOTE: Vectors X direction corresponds to Pitch, Y to Yaw, and Z to Roll
// NOTE: Rotation angles are in terms of Degrees per Second

// Set Rotation Velocity (Magnitude of vector is used as number of Degrees to Rotate Per Second)
void CObject::SetRotationVelocity(Ogre::Vector3 sRotationVelocity)
{
	mfRotationSpeed = sRotationVelocity.normalise();
	msUnitRotationVelocity = sRotationVelocity;
}

// Set Rotation Velocity
void CObject::SetRotationVelocity(Ogre::Vector3 sRotation, float fRotationSpeedInDegreesPerSecond)
{
	sRotation.normalise();
	msUnitRotationVelocity = sRotation;
	mfRotationSpeed = fRotationSpeedInDegreesPerSecond;
}

// Get Rotation Velocity
Ogre::Vector3 CObject::GetRotationVelocityDirection() const
{
	return msUnitRotationVelocity;
}

// Get Rotation Velocity (Magnitude of vector is used as number of Degrees to Rotate Per Second)
Ogre::Vector3 CObject::GetRotationVelocity() const
{
	return msUnitRotationVelocity * mfRotationSpeed;
}

// Set the Objects Rotation Velocity to the sum of its current Rotation Velocity and the given Velocity
void CObject::SetRotationVelocityRelative(Ogre::Vector3 sVelocityToAdd)
{
	Ogre::Vector3 sRotationVelocity = msUnitRotationVelocity * mfRotationSpeed;
	sRotationVelocity += sVelocityToAdd;
	
	mfRotationSpeed = sRotationVelocity.normalise();
	msUnitRotationVelocity = sRotationVelocity;
}

// Set Rotation Speed
void CObject::SetRotationVelocitySpeed(float fSpeedInDegreesPerSecond)
{
	mfRotationSpeed = fSpeedInDegreesPerSecond;
}

// Get Rotation Speed
float CObject::GetRotationVelocitySpeed() const
{
	return mfRotationSpeed;
}

// Set the Min Rotation Angle to Rotate per Second
void CObject::SetMinRotationVelocitySpeed(float fSpeedInDegreesPerSecond)
{
	mfMinRotationVelocity = fSpeedInDegreesPerSecond;
}

// Get the Min Rotation Angle to Rotate per Second
float CObject::GetMinRotationVelocitySpeed() const
{
	return mfMinRotationVelocity;
}

// Set the Max Rotation Angle to Rotate per Second
void CObject::SetMaxRotationVelocitySpeed(float fSpeedInDegreesPerSecond)
{
	mfMaxRotationVelocity = fSpeedInDegreesPerSecond;
}

// Get the Max Rotation Angle to Rotate per Second
float CObject::GetMaxRotationVelocitySpeed() const
{
	return mfMaxRotationVelocity;
}

// Set the Default Rotation Angle to Rotate per Second
void CObject::SetDefaultRotationVelocitySpeed(float fSpeedInDegreesPerSecond)
{
	mfDefaultRotationVelocity = fSpeedInDegreesPerSecond;
}

// Get the Default Rotation Angle to Rotate per Second
float CObject::GetDefaultRotationVelocitySpeed() const
{
	return mfDefaultRotationVelocity;
}


// Set Rotation Acceleration (Magnitude of vector is used as number of Degrees to Accelerate Per Second)
// Acceleration is used to increase/decrease the velocity every time Update() is called
// If you do not want the Object to speed up/slow down, set Accelerations to zero
// NOTE: Requires Update() to be called each frame to actually move the object
// NOTE: When Update() is called Velocity is automatically increased/decreased by current Acceleration
void CObject::SetRotationAcceleration(Ogre::Vector3 sRotationAcceleration)
{
	mfRotationAccelerationRate = sRotationAcceleration.normalise();
	msUnitRotationAcceleration = sRotationAcceleration;
}

// Set Rotation Acceleration
void CObject::SetRotationAcceleration(Ogre::Vector3 sRotation, float fRotationAccelerationInDegreesPerSecond)
{
	sRotation.normalise();
	msUnitRotationAcceleration = sRotation;
	mfRotationAccelerationRate = fRotationAccelerationInDegreesPerSecond;
}

// Get Rotation Acceleration
Ogre::Vector3 CObject::GetRotationAccelerationDirection() const
{
	return msUnitRotationAcceleration;
}

// Get Rotation Acceleration (Magnitude of vector is rotational acceleration in Degrees Per Second)
Ogre::Vector3 CObject::GetRotationAcceleration() const
{
	return msUnitRotationAcceleration * mfRotationAccelerationRate;
}

// Set the Objects Rotation Acceleration to the sum of its current Rotation Acceleration and the given Acceleration
void CObject::SetRotationAccelerationRelative(Ogre::Vector3 sAccelerationToAdd)
{
	Ogre::Vector3 sRotationAcceleration = msUnitRotationAcceleration * mfRotationAccelerationRate;
	sRotationAcceleration += sAccelerationToAdd;
	
	mfRotationAccelerationRate = sRotationAcceleration.normalise();
	msUnitRotationVelocity = sRotationAcceleration;
}

// Set Rotation Acceleration Rate
void CObject::SetRotationAccelerationRate(float fAccelerationInDegreesPerSecond)
{
	mfRotationAccelerationRate = fAccelerationInDegreesPerSecond;
}

// Get Rotation Acceleration Rate
float CObject::GetRotationAccelerationRate() const
{
	return mfRotationAccelerationRate;
}

// Set the Min Rotation Angle to Accelerate by per Second
void CObject::SetMinRotationAccelerationRate(float fAccelerationInDegreesPerSecond)
{
	mfMinRotationAcceleration = fAccelerationInDegreesPerSecond;
}

// Get the Min Rotation Angle to Accelerate by per Second
float CObject::GetMinRotationAccelerationRate() const
{
	return mfMinRotationAcceleration;
}

// Set the Max Rotation Angle to Accelerate by per Second
void CObject::SetMaxRotationAccelerationRate(float fAccelerationInDegreesPerSecond)
{
	mfMaxRotationAcceleration = fAccelerationInDegreesPerSecond;
}

// Get the Max Rotation Angle to Accelerate by per Second
float CObject::GetMaxRotationAccelerationRate() const
{
	return mfMaxRotationAcceleration;
}

// Set the Default Rotation Angle to Accelerate by per Second
void CObject::SetDefaultRotationAccelerationRate(float fAccelerationInDegreesPerSecond)
{
	mfDefaultRotationAcceleration = fAccelerationInDegreesPerSecond;
}

// Get the Default Rotation Angle to Accelerate by per Second
float CObject::GetDefaultRotationAccelerationRate() const
{
	return mfDefaultRotationAcceleration;
}

// Set if Rotation Accelerations should be used or not (true by default)
void CObject::SetIfRotationAccelerationShouldBeUsed(bool bUseRotationAcceleration)
{
	mbUseRotationAcceleration = bUseRotationAcceleration;
}

// Get if Rotation Accelerations should be used or not (true by default)
bool CObject::GetIfRotationAccelerationIsBeingUsed() const
{
	return mbUseRotationAcceleration;
}


// Set Rotation Friction
// Rotation Frictions will slow the objects rotation
// Zero friction means the object will not slow down on its own
// NOTE: Requires Update() to be called each frame to have an effect
// NOTE: Specified friction Pitch will slow object down by Pitch pixels per second
void CObject::SetRotationFriction(float fRotationFriction)
{
	mfRotationFriction = fRotationFriction;
}

// Get Rotation Friction
float CObject::GetRotationFriction() const
{
	return mfRotationFriction;
}

// Set if Rotation Friction should be used or not (true by default)
void CObject::SetIfRotationFrictionShouldBeUsed(bool bUseRotationFriction)
{
	mbUseRotationFriction = bUseRotationFriction;
}

// Get if Rotation Friction should be used or not (true by default)
bool CObject::GetIfRotationFrictionIsBeingUsed() const
{
	return mbUseRotationFriction;
}



// Updates Objects Position
// Moves Object by specified Velocity amounts, which is calculated using the Acceleration values
// NOTE: This should be called every frame
// NOTE: ulAmountOfTimeSinceLastUpdate should be in milliseconds (1000ms = 1s)
void CObject::Update(unsigned long ulAmountOfTimeSinceLastUpdate)
{
	// Put TimeSinceLastUpdate in terms of seconds instead of milliseconds
	float fAmountOfTimeSinceLastUpdateInSeconds = ulAmountOfTimeSinceLastUpdate / 1000.0;

	// Update the Object
	Update(fAmountOfTimeSinceLastUpdateInSeconds);
}

// Updates Objects Position
// Moves Object by specified Velocity amounts, which is calculated using the Acceleration values
// NOTE: This should be called every frame
void CObject::Update(float fAmountOfTimeSinceLastUpdateInSeconds)
{
	// Temp local variables
	Ogre::Vector3 sVelocity = msUnitVelocity * mfSpeed;
	Ogre::Vector3 sRotationVelocity = msUnitRotationVelocity * mfRotationSpeed;
	Ogre::Vector3 sAcceleration = msUnitAcceleration * mfAccelerationRate;
	Ogre::Vector3 sRotationAcceleration = msUnitRotationAcceleration * mfRotationAccelerationRate;

	Ogre::Vector3 sNewVelocity = sVelocity;
	Ogre::Vector3 sNewRotationVelocity = sRotationVelocity;
	float fFrictionImpact = 0.0f;
	float fRotationFrictionImpact = 0.0f;
	float fLength = 0.0f;

	// Record how much time passed between the last two Update() calls
	mfDurationSinceLastUpdateInSeconds = fAmountOfTimeSinceLastUpdateInSeconds;


	//---------- Calculate how much to move ----------

	// If Movement Acceleration is being used
	if (mbUseMovementAcceleration)
	{
		// Calculate new Velocities from Accelerations
		// Acceleration = (Vfinal - Vinitial) / Time   =>  Vfinal = (A * T) + Vinitial 
		sNewVelocity.x = (sAcceleration.x * fAmountOfTimeSinceLastUpdateInSeconds) + sVelocity.x;
		sNewVelocity.y = (sAcceleration.y * fAmountOfTimeSinceLastUpdateInSeconds) + sVelocity.y;
		sNewVelocity.z = (sAcceleration.z * fAmountOfTimeSinceLastUpdateInSeconds) + sVelocity.z;
	}

	// If Rotation Friction is being used
	if (mbUseMovementFriction)
	{
		// Calculate how much impact Friction should have
		fFrictionImpact = mfFriction * fAmountOfTimeSinceLastUpdateInSeconds;


		// Process Friction impact (Do nothing if Velocity is zero)
		
		// Slow down X Velocity by Friction amount
		if (sNewVelocity.x > 0)
		{
			sNewVelocity.x -= fFrictionImpact;

			// If we changed Velocity to opposite direction
			if (sNewVelocity.x < 0)
			{
				sNewVelocity.x = 0.0f;
			}
		}else if (sNewVelocity.x < 0)
		{
			sNewVelocity.x += fFrictionImpact;

			// If we changed Velocity to opposite direction
			if (sNewVelocity.x > 0)
			{
				sNewVelocity.x = 0.0f;
			}
		}

		// Slow down Y Velocity by Friction amount
		if (sNewVelocity.y > 0)
		{
			sNewVelocity.y -= fFrictionImpact;

			// If we changed Velocity to opposite direction
			if (sNewVelocity.y < 0)
			{
				sNewVelocity.y = 0.0f;
			}
		}else if (sNewVelocity.y < 0)
		{
			sNewVelocity.y += fFrictionImpact;

			// If we changed Velocity to opposite direction
			if (sNewVelocity.y > 0)
			{
				sNewVelocity.y = 0.0f;
			}
		}

		// Slow down Z Velocity by Friction amount
		if (sNewVelocity.z > 0)
		{
			sNewVelocity.z -= fFrictionImpact;

			// If we changed Velocity to opposite direction
			if (sNewVelocity.z < 0)
			{
				sNewVelocity.z = 0.0f;
			}
		}else if (sNewVelocity.z < 0)
		{
			sNewVelocity.z += fFrictionImpact;

			// If we changed Velocity to opposite direction
			if (sNewVelocity.z > 0)
			{
				sNewVelocity.z = 0.0f;
			}
		}
	}


	// Make sure Velocities are not out of range

	// Get the New Velocity
	fLength = sNewVelocity.length();
	
	// If the Object is moving too fast
	if (fLength > mfMaxVelocity)
	{
		// Make the Object move at its max speed
		sNewVelocity.normalise();
		sNewVelocity *= mfMaxVelocity;
	}
	// Else if the Object is moving too slow
	else if (fLength < mfMinVelocity)
	{
		// Make the Object move at its min speed
		sNewVelocity.normalise();
		sNewVelocity *= mfMinVelocity;
	}


	//---------- Apply External Forces ----------

	// If there are some External Forces to apply
	if (!mcExternalForceList.empty())
	{
		list<SExternalForce>::iterator cForceIterator;

		// Loop through all External Forces
		for (cForceIterator = mcExternalForceList.begin(); cForceIterator != mcExternalForceList.end(); cForceIterator++)
		{
			// Add the External Force to the Objects velocity
			mcpSceneNode->translate(cForceIterator->sForce * fAmountOfTimeSinceLastUpdateInSeconds, cForceIterator->sTransformSpace);
		}
	}


	// Set New Velocities
	sVelocity = sNewVelocity;

	msUnitVelocity = sVelocity;
	mfSpeed = msUnitVelocity.normalise();


	//---------- Calculate how much to Rotate ----------

	// If Rotation Acceleration is being used
	if (mbUseRotationAcceleration)
	{
		// Calculate new Rotation Velocities from Accelerations
		// Acceleration = (Vfinal - Vinitial) / Time   =>  Vfinal = (A * T) + Vinitial 
		sNewRotationVelocity.x = (sRotationAcceleration.x * fAmountOfTimeSinceLastUpdateInSeconds) + sRotationVelocity.x;
		sNewRotationVelocity.y = (sRotationAcceleration.y * fAmountOfTimeSinceLastUpdateInSeconds) + sRotationVelocity.y;
		sNewRotationVelocity.z = (sRotationAcceleration.z * fAmountOfTimeSinceLastUpdateInSeconds) + sRotationVelocity.z;
	}

	// If Rotation Friction is being used
	if (mbUseRotationFriction)
	{
		// Calculate how much impact Rotation Friction should have
		fFrictionImpact = mfRotationFriction * fAmountOfTimeSinceLastUpdateInSeconds;


		// Process Rotation Friction impact (Do nothing if Velocity is zero)
		
		// Slow down Pitch Velocity by Friction amount
		if (sNewRotationVelocity.x > 0)
		{
			sNewRotationVelocity.x -= fFrictionImpact;

			// If we changed Velocity to opposite direction
			if (sNewRotationVelocity.x < 0)
			{
				sNewRotationVelocity.x = 0.0f;
			}
		}else if (sNewRotationVelocity.x < 0)
		{
			sNewRotationVelocity.x += fFrictionImpact;

			// If we changed Velocity to opposite direction
			if (sNewRotationVelocity.x > 0)
			{
				sNewRotationVelocity.x = 0.0f;
			}
		}

		// Slow down Yaw Velocity by Friction amount
		if (sNewRotationVelocity.y > 0)
		{
			sNewRotationVelocity.y -= fFrictionImpact;

			// If we changed Velocity to opposite direction
			if (sNewRotationVelocity.y < 0)
			{
				sNewRotationVelocity.y = 0.0f;
			}
		}else if (sNewRotationVelocity.y < 0)
		{
			sNewRotationVelocity.y += fFrictionImpact;

			// If we changed Velocity to opposite direction
			if (sNewRotationVelocity.y > 0)
			{
				sNewRotationVelocity.y = 0.0f;
			}
		}

		// Slow down Roll Velocity by Friction amount
		if (sNewRotationVelocity.z > 0)
		{
			sNewRotationVelocity.z -= fFrictionImpact;

			// If we changed Velocity to opposite direction
			if (sNewRotationVelocity.z < 0)
			{
				sNewRotationVelocity.z = 0.0f;
			}
		}else if (sNewRotationVelocity.z < 0)
		{
			sNewRotationVelocity.z += fFrictionImpact;

			// If we changed Velocity to opposite direction
			if (sNewRotationVelocity.z > 0)
			{
				sNewRotationVelocity.z = 0.0f;
			}
		}
	}


	// Make sure Velocities are not out of range

	// Get the New Rotation Velocity
	fLength = sNewRotationVelocity.length();
	
	// If the Object is rotating too fast
	if (fLength > mfMaxRotationVelocity)
	{
		// Make the Object rotate at its max speed
		sNewRotationVelocity.normalise();
		sNewRotationVelocity *= mfMaxRotationVelocity;
	}
	// Else if the Object is rotating too slow
	else if (fLength < mfMinRotationVelocity)
	{
		// Make the Object rotate at its min speed
		sNewRotationVelocity.normalise();
		sNewRotationVelocity *= mfMinRotationVelocity;
	}

	// Set New Rotation Velocities
	sRotationVelocity = sNewRotationVelocity;

	msUnitRotationVelocity = sRotationVelocity;
	mfRotationSpeed = msUnitRotationVelocity.normalise();


	//---------- Update Object according to if it's being automatically controlled or not ----------

	// If the Object should face the Destination
	if (mbFaceDestinationAutomatically)
	{
		// If we have reached the Destination
		if (mbDestinationReached)
		{
			// Get the Next Destination
			GetNextDestination();
		}
		else
		{
			// Make sure the Object is facing the Destination
			PointObjectTowardsDestination();
		}
	}
	// Else the Object is able to control itself
	else
	{
		// Calculate new Relative Rotation from Velocities
		// Velocity = Distance / Time  =>  D = V * T
	//	SetOrientationRelative((mfPitchVelocity * fAmountOfTimeSinceLastUpdateInSeconds), (mfYawVelocity * fAmountOfTimeSinceLastUpdateInSeconds),
	//						   (mfRollVelocity * fAmountOfTimeSinceLastUpdateInSeconds), mTranslationRelativeTo);
		// If the Object should Yaw
		if (sRotationVelocity.y != 0.0)
		{
			mcpSceneNode->yaw(Ogre::Degree(sRotationVelocity.y * fAmountOfTimeSinceLastUpdateInSeconds), mYawRelativeTo);
		}

		// If the Object should Pitch
		if (sRotationVelocity.x != 0.0)
		{
			mcpSceneNode->pitch(Ogre::Degree(sRotationVelocity.x * fAmountOfTimeSinceLastUpdateInSeconds), mPitchRelativeTo);
		}

		// If the Object should Roll
		if (sRotationVelocity.z != 0.0)
		{
			mcpSceneNode->roll(Ogre::Degree(sRotationVelocity.z * fAmountOfTimeSinceLastUpdateInSeconds), mRollRelativeTo);
		}
	}

	// If the Object is being moved automatically
	if (mbMoveToDestinationAutomatically)
	{
		// If we have reached the Destination (or don't have one to move towards)
		if (mbDestinationReached || mcpDestination == NULL)
		{
			// Get the Next Destination
			GetNextDestination();
		}
		// Else we have a Destination to move towards
		else
		{
			// Calculate how far to move this frame
			float fAmountToMove = mfSpeed * fAmountOfTimeSinceLastUpdateInSeconds;

			// Calculate the Direction and Distance to the Destination
			msDirectionToDestination = mcpDestination->getWorldPosition() - mcpSceneNode->getWorldPosition();
			mfDistanceToDestination = msDirectionToDestination.normalise();

			// Subtract the Distance moved this frame from the Distance to the Destination
			mfDistanceToDestination -= fAmountToMove;

			// If we moved past the Destination
			if (mfDistanceToDestination <= 0.0f)
			{
				// Move the Object to the Destination and reset the Destination
				mcpSceneNode->setPosition(mcpDestination->getWorldPosition());

				// Mark that we reached the Destination
				mbDestinationReached = true;
				mcpDestination = NULL;

				// Get the next Destination to go to
				GetNextDestination();
			}
			// Else we haven't reached the Destination yet
			else
			{
				// If the Object should be facing the destination
				if (mbFaceDestinationAutomatically)
				{
					// Move the Object forward
					mcpSceneNode->translate(msMeshForwardDirection * fAmountToMove, Ogre::Node::TS_LOCAL);
				}
				else
				{
					// Move towards the Destination
					mcpSceneNode->translate(msDirectionToDestination * fAmountToMove);
				}
			}
		}
	}
	// Else the Object is able to control itself
	else
	{
		// Calculate new Relative Position from Velocities
		// Velocity = Distance / Time  =>  D = V * T
	//	MoveToRelative((mfXVelocity * fAmountOfTimeSinceLastUpdateInSeconds), (mfYVelocity * fAmountOfTimeSinceLastUpdateInSeconds),
	//				   (mfZVelocity * fAmountOfTimeSinceLastUpdateInSeconds), mTranslationRelativeTo);

		// If the Object should move
		if (sVelocity.x != 0.0 || sVelocity.y != 0.0 || sVelocity.z != 0.0)
		{
			mcpSceneNode->translate((sVelocity.x * fAmountOfTimeSinceLastUpdateInSeconds), (sVelocity.y * fAmountOfTimeSinceLastUpdateInSeconds),
									(sVelocity.z * fAmountOfTimeSinceLastUpdateInSeconds), mTranslationRelativeTo);
		}
	}

	// If this Object has an animation
	if (mcpAnimationState != NULL)
	{
		// If the animation is enabled right now
		if (mcpAnimationState->getEnabled())
		{
			// Update the animation
			mcpAnimationState->addTime(fAmountOfTimeSinceLastUpdateInSeconds);
		}
	}
}



// ---------- Easy Rotation Functions ----------

// Rotates the Object to face the given Location
void CObject::PointObjectTowardsLocation(Ogre::Vector3 _sLocation, bool _bImmediately)
{
	// Find the Direction of the Location relative to this Object
	Ogre::Vector3 sDirection = _sLocation - mcpSceneNode->getWorldPosition();

	// Point the Object in the Direction of the Location
	PointObjectInDirection(sDirection, _bImmediately);
}

// Rotates the Object to face the given Direction
void CObject::PointObjectInDirection(Ogre::Vector3 _sDirection, bool _bImmediately)
{	
	// How much of the rotation should be performed this call (0.0 = no rotation, 1.0 = rotate the whole way)
	float fRatioToRotate = 1.0f;

	// Specify the Rotation Speed to use (if current rotation speed is zero, use the default rotation speed)
	float fRotationSpeed = (mfRotationSpeed != 0.0) ? mfRotationSpeed : mfDefaultRotationVelocity;

	// If we don't need to face the given direction immediately
	if (!_bImmediately && fRotationSpeed != 0.0)
	{
		// Calculate how fast to rotate the Object

		// Make sure Direction to face is a unit vector
		_sDirection.normalise();

		// Get the angle between the direction the Object is currently facing, and the direction we want it to face
		float fAngleToTurn = Ogre::Math::ACos(GetDirectionObjectIsFacing().dotProduct(_sDirection)).valueDegrees();

		// Calculate the amount of time the rotation should take
		float fNumberOfSecondsRotationShouldTake = fAngleToTurn / fRotationSpeed;

		// Calculate how much of the rotation should be performed this frame
		fRatioToRotate = mfDurationSinceLastUpdateInSeconds / fNumberOfSecondsRotationShouldTake;

		// Make sure we do not rotate too far
		if (fRatioToRotate > 1.0 || fRatioToRotate <= 0.0)
		{
			fRatioToRotate = 1.0;
		}
	}

	// Get Objects current orientation
	Ogre::Quaternion sSourceRotation = mcpSceneNode->getOrientation();

	// Get the Rotation that is needed to make the Object face the given direction
	Ogre::Quaternion sDestinationRotation = GetDirectionObjectIsFacing().getRotationTo(_sDirection) * sSourceRotation;

//	mcpSceneNode->setOrientation(sDestinationRotation);
	mcpSceneNode->setOrientation(Ogre::Quaternion::Slerp(fRatioToRotate, sSourceRotation, sDestinationRotation, true));
}

// Returns the unit Direction the Object is currently facing
Ogre::Vector3 CObject::GetDirectionObjectIsFacing() const
{
	Ogre::Vector3 sDirection = mcpSceneNode->getOrientation() * msMeshForwardDirection;
	sDirection.normalise();
	return sDirection;
}



// ---------- Automatic Tracking Functions ----------

// Sets the dummy Scene Node to use to track Vectors
// NOTE: This Scene Node should not be used by anything other than this class and should be a child
//		  of the root Scene Node
void CObject::SetExclusiveSceneNodeToUseForAutomaticTracking(Ogre::SceneNode* _cpExclusiveSceneNode)
{
	mcpAutoTrackingSceneNode = _cpExclusiveSceneNode;
}

// Gets the dummy Scene Node to use to track Vectors
// NOTE: This Scene Node should not be used by anything other than this class and should be a child
//		  of the root Scene Node
Ogre::SceneNode* CObject::GetExclusiveSceneNodeUsedForAutomaticTracking() const
{
	return mcpAutoTrackingSceneNode;
}

// Sets if the Object should move towards it's Destinations automatically (true) or if the Object should
//	have free control to move itself around
// NOTE: If true, any calls to SetAcceleration/Velocity will be ignored until set back to false
// NOTE: Assumes the Object faces down the positive Z direction
// NOTE: Uses the Objects NormalZAccleration as the rate of movement to move the Object
// NOTE: The Set() functions return false if they do not have a Destination to go to (the DestinationList is empty) 
bool CObject::SetIfObjectShouldMoveToDestinationAutomatically(bool _bMoveToDestination, bool _bFaceDestination)
{
	// Record if we should move the Object automatically or not
	mbMoveToDestinationAutomatically = _bMoveToDestination;

	// Set if the Object should face the Destination or not, and 
	//	Return if there is a Destination to move to or not
	return SetIfObjectShouldFaceDestinationAutomatically(_bFaceDestination);
}

// Set if the Object should face towards it's Destination always or not
bool CObject::SetIfObjectShouldFaceDestinationAutomatically(bool _bFaceDestination)
{
	mbFaceDestinationAutomatically = _bFaceDestination;

	// Return if there is a Destination to face to or not
	return !mcDestinationList.empty();
}

// Return if the Object is moving towards it's Destination automatically or not
bool CObject::GetIfObjectShouldMoveToDestinationAutomatically() const
{
	return mbMoveToDestinationAutomatically;
}

// Return if the Object is always facing it's Destination or not
bool CObject::GetIfObjectShouldFaceDestinationAutomatically() const
{
	return mbFaceDestinationAutomatically;
}

// Get the next Destination from the Destination List and store it, as well as make
//	the Object face the new Destination if it's supposed to
bool CObject::GetNextDestination()
{
	// If there is another Destination to go to
	if (mcDestinationList.empty())
	{	
		// Record that we have Reached the Destination (since we don't have one) and exit the function
		mbDestinationReached = true;
		return false;
	}

	// Get and Remove the next Destination from the Destination List
    SDestination* spTemp = mcDestinationList.front();
    mcDestinationList.pop_front();

	// If a Scene Node was given to track
	if (spTemp->cpSceneNode != NULL)
	{
		// Set to track the Scene Node
		mcpDestination = spTemp->cpSceneNode;
	}
	// Else if a Vector was given to track and we have a dummy Scene Node to use
	else if (spTemp->cpVector != NULL && mcpAutoTrackingSceneNode != NULL)
	{
		// Place our dummy Scene Node at the position of the Vector and set it as the Destination		
		mcpAutoTrackingSceneNode->setPosition(*spTemp->cpVector);
		mcpDestination = mcpAutoTrackingSceneNode;
	}
	// Else we cannot use this Destination so try getting the next one
	else
	{
		// Return after getting the Next Destination
		return GetNextDestination();
	}

	// Delete the Destination instance now that we're done with it
	delete spTemp;

	// Mark that we haven't reached the Destination yet
	mbDestinationReached = false;

	// Return that the Object has a Destination
    return true;
}

// Return the current Destination to move to/face
Ogre::SceneNode* CObject::GetCurrentDestination() const
{
	return mcpDestination;
}

// Returns if the Object has reached all of it's Destinations or not
bool CObject::GetIfObjectHasReachedAllDestinations() const
{
	return (mcDestinationList.empty() && mbDestinationReached);
}

// Points the Object towards the current Destination if one exists
bool CObject::PointObjectTowardsDestination()
{
	// If there is no Destination to point towards
	if (mbDestinationReached || mcpDestination == NULL)
	{
		// Exit the function since there is no Destination to point towards
		return false;
	}

	// Make sure the Object is facing its Destination
//	mcpSceneNode->setAutoTracking(true, mcpDestination, msObjectForwardDirection);
	PointObjectTowardsLocation(mcpDestination->getWorldPosition());

/*
		The below code will point the Object towards it's Destination, but the Scene Nodes
	provide a function to do this for us, so we'll just use it as well. Leaving this code here
	in case it is needed in the future for some reason.

	// Calculate the Direction and Distance to the Destination
	msDirectionToDestination = mcpDestination->getWorldPosition() - mcpSceneNode->getWorldPosition();
    mfDistanceToDestination = msDirectionToDestination.normalise();

	// Get the direction the Object is currently facing
	Ogre::Vector3 sCurrentDirection = mcpSceneNode->getOrientation() * msObjectForwardDirection;

	// If we need to do a 180 degree yaw turn (special case as it will cause divide by zero error)
    if ((1.0f + sCurrentDirection.dotProduct(msDirectionToDestination)) < 0.0001f) 
    {
		// Rotate Object 180 degrees
		mcpSceneNode->yaw(Ogre::Degree(180));
    }
    else
    {
		// Find how much we need to rotate to be facing the Destination
        Ogre::Quaternion RotationNeeded = sCurrentDirection.getRotationTo(msDirectionToDestination);

		// Rotate to face the Destination
        mcpSceneNode->rotate(RotationNeeded);
    }
*/
	// Return that Object is now facing the Destination
	return true;
}



//---------- Object Attributes Functions ----------

// Set Objects ID
void CObject::SetID(int _iID)
{
	miID = _iID;
}

// Return Objects ID
int CObject::GetID() const
{
	return miID;
}

// Set the Objects Name
void CObject::SetName(Ogre::String _sName)
{
	msName = _sName;
}

// Return the Objects Name
Ogre::String CObject::GetName() const
{
	return msName;
}

// Set Objects General Type (ie Player or Enemy)
void CObject::SetGeneralObjectType(int iObjectType)
{
	miGeneralObjectType = iObjectType;
}

// Return Objects General Type (ie Player or Enemy)
int CObject::GetGeneralObjectType() const
{
	return miGeneralObjectType;
}

// Set Objects Specific Type (ie Which Player or Enemy)
void CObject::SetSpecificObjectType(int iType)
{
	miSpecificObjectType = iType;
}

// Return Objects Specific Type (ie Which Player or Enemy)
int CObject::GetSpecificObjectType() const
{
	return miSpecificObjectType;
}

// Set the Objects State
void CObject::SetState(int iState)
{
	miState = iState;
}

// Return the Objects State
int CObject::GetState() const
{
	return miState;
}

// Set if Object is colliding with another Object or not
void CObject::SetIfObjectIsColliding(bool bIsCollision)
{
	mbCollision = bIsCollision;
}

// Return if Object is colliding with another Object or not
bool CObject::GetIfObjectIsColliding() const
{
	return mbCollision;
}