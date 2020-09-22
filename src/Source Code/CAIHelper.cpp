#include "CAIHelper.h"

// Default Constructor
CAIHelper::CAIHelper()
{
	Purge();
}

// Destructor
CAIHelper::~CAIHelper()
{
	Purge();
}

// Copy Constructor
CAIHelper::CAIHelper(CAIHelper const& _cAIHelperToCopyFrom)
{
	// Copy data over from other AI object
	CopyFrom(_cAIHelperToCopyFrom);
}

// Copy from another object
void CAIHelper::CopyFrom(CAIHelper const& _cAIHelperToCopyFrom)
{
	Purge();

	// Copy the Lists
	mcNeighborList = _cAIHelperToCopyFrom.mcNeighborList;
	mcPreditorList = _cAIHelperToCopyFrom.mcPreditorList;

	mfSeparationCoefficient = _cAIHelperToCopyFrom.GetSeparationCoefficient();
	mfAlignmentCoefficient = _cAIHelperToCopyFrom.GetAlignmentCoefficient();
	mfCohesionCoefficient = _cAIHelperToCopyFrom.GetCohesionCoefficient();
	mfPreditorAvoidanceCoefficient = _cAIHelperToCopyFrom.GetPreditorAvoidanceCoefficient();

	mfSenseNeighborRange = _cAIHelperToCopyFrom.GetSenseNeighborRange();
	miMaxNumberOfNeighbors = _cAIHelperToCopyFrom.GetMaxNumberOfNeighbors();

	mfAge = _cAIHelperToCopyFrom.GetAge();
	mfSpawnAge = _cAIHelperToCopyFrom.GetSpawnAge();
	mbIsFlockingFish = _cAIHelperToCopyFrom.GetIsAFlockingFish();
	mfSenseSharkRange = _cAIHelperToCopyFrom.GetSenseSharkRange();

	miNourishedHealth = _cAIHelperToCopyFrom.GetNourishedHealth();
	miFullHealth = _cAIHelperToCopyFrom.GetFullHealth();
	mfSenseSubmarineRange = _cAIHelperToCopyFrom.GetSenseSubmarineRange();
	mfFeedRange = _cAIHelperToCopyFrom.GetFeedRange();
	mfAttackRange = _cAIHelperToCopyFrom.GetAttackRange();
	mulHitDelay = _cAIHelperToCopyFrom.GetHitDelay();
	mulHealthDegradeDelay = _cAIHelperToCopyFrom.GetHealthDegradeDelay();
	mfVelocityCoefficient = _cAIHelperToCopyFrom.GetVelocityCoefficient();

	mfSenseFishRange = _cAIHelperToCopyFrom.GetSenseFishRange();
	mulUpdateTimeInterval = _cAIHelperToCopyFrom.GetUpdateTimeInterval();
}

// Erase all information as if this instance was just created
void CAIHelper::Purge()
{
	// Empty the Lists
	mcNeighborList.clear();
	mcPreditorList.clear();

	// Reset Coefficients
	mfSeparationCoefficient = mfAlignmentCoefficient = mfCohesionCoefficient = mfPreditorAvoidanceCoefficient = 0.0f;
	mfSenseNeighborRange = -1.0f;
	miMaxNumberOfNeighbors = 0;
	
	mfAge = mfSpawnAge = mfSenseSharkRange = 0.0f;
	mbIsFlockingFish = false;

	miNourishedHealth = miFullHealth = 0;
	mfSenseSubmarineRange = mfFeedRange = mfAttackRange = 0.0f;
	mulHitDelay = mulHealthDegradeDelay = 0;
	mfVelocityCoefficient = 1.0f;

	mfSenseFishRange = 0.0f;
	mulUpdateTimeInterval = 0;
}


//---------- Set and Get Functions ----------


// Set the Separation Coefficient
void CAIHelper::SetSeparationCoefficient(float _fSeparationCoefficient)
{
	mfSeparationCoefficient = _fSeparationCoefficient;
}

// Get the Separation Coefficient
float CAIHelper::GetSeparationCoefficient() const
{
	return mfSeparationCoefficient;
}

// Set the Alignment Coefficient
void CAIHelper::SetAlignmentCoefficient(float _fAlignmentCoefficient)
{
	mfAlignmentCoefficient = _fAlignmentCoefficient;
}

// Get the Alignment Coefficient
float CAIHelper::GetAlignmentCoefficient() const
{
	return mfAlignmentCoefficient;
}

// Set the Cohesion Coefficient
void CAIHelper::SetCohesionCoefficient(float _fCohesionCoefficient)
{
	mfCohesionCoefficient = _fCohesionCoefficient;
}

// Get the Cohesion Coefficient
float CAIHelper::GetCohesionCoefficient() const
{
	return mfCohesionCoefficient;
}

// Set the Preditor Avoidance Coefficient
void CAIHelper::SetPreditorAvoidanceCoefficient(float _fPreditorAvoidanceCoefficient)
{
	mfPreditorAvoidanceCoefficient = _fPreditorAvoidanceCoefficient;
}

// Get the Preditor Avoidance Coefficient
float CAIHelper::GetPreditorAvoidanceCoefficient() const
{
	return mfPreditorAvoidanceCoefficient;
}

// Set the Maximum Distance between 2 Object to consider them Neighbors
void CAIHelper::SetSenseNeighborRange(float _fNeighborSenseRange)
{
	mfSenseNeighborRange = _fNeighborSenseRange;
}

// Get the Maximum Distance between 2 Object to consider them Neighbors
float CAIHelper::GetSenseNeighborRange() const
{
	return mfSenseNeighborRange;
}

// Set the Maximum Number of Neighbors allowed
void CAIHelper::SetMaxNumberOfNeighbors(int _iMaxNumberOfNeighbors)
{
	miMaxNumberOfNeighbors = _iMaxNumberOfNeighbors;
}

// Get the Maximum Number of Neighbors allowed
int CAIHelper::GetMaxNumberOfNeighbors() const
{
	return miMaxNumberOfNeighbors;
}

// Set the Age of the fish
void CAIHelper::SetAge(float _fAge)
{
	mfAge = _fAge;
}

// Get the Age of the fish
float CAIHelper::GetAge() const
{
	return mfAge;
}

// Set the Age at which the fish will Spawn into two fish
void CAIHelper::SetSpawnAge(float _fSpawnAge)
{
	mfSpawnAge = _fSpawnAge;
}

// Get the Age at which the fish will Spawn into two fish
float CAIHelper::GetSpawnAge() const
{
	return mfSpawnAge;
}

// Set if this Fish is a Flocking Fish or not
void CAIHelper::SetIsAFlockingFish(bool _bIsAFlockingFish)
{
	mbIsFlockingFish = _bIsAFlockingFish;
}

// Get if this Fish is a Flocking Fish or not
bool CAIHelper::GetIsAFlockingFish() const
{
	return mbIsFlockingFish;
}

// Set the distance at which the Shark can be detected
void CAIHelper::SetSenseSharkRange(float _fSenseSharkRange)
{
	mfSenseSharkRange = _fSenseSharkRange;
}

// Get the distance at which the Shark can be detected
float CAIHelper::GetSenseSharkRange() const
{
	return mfSenseSharkRange;
}



// Shark Functions

// Set Health at which Shark is considered Nourished
void CAIHelper::SetNourishedHealth(int _iNourishedHealth)
{
	miNourishedHealth = _iNourishedHealth;
}

// Get Health at which Shark is considered Nourished
int CAIHelper::GetNourishedHealth() const
{
	return miNourishedHealth;
}

// Set Health at which Shark is considered Full
void CAIHelper::SetFullHealth(int _iFullHealth)
{
	miFullHealth = _iFullHealth;
}

// Get Health at which Shark is considered Full
int CAIHelper::GetFullHealth() const
{
	return miFullHealth;
}

// Set the Range at which the Shark Senses the Submarine
void CAIHelper::SetSenseSubmarineRange(float _fSenseSubmarineRange)
{
	mfSenseSubmarineRange = _fSenseSubmarineRange;
}

// Get the Range at which the Shark Senses the Submarine
float CAIHelper::GetSenseSubmarineRange() const
{
	return mfSenseSubmarineRange;
}

// Set how close the Shark must be to eat a Fish
void CAIHelper::SetFeedRange(float _fFeedRange)
{
	mfFeedRange = _fFeedRange;
}

// Get how close the Shark must be to eat a Fish
float CAIHelper::GetFeedRange() const
{
	return mfFeedRange;
}

// Set how close Shark must be to Submarine to attack it
void CAIHelper::SetAttackRange(float _fAttackRange)
{
	mfAttackRange = _fAttackRange;
}

// Get how close Shark must be to Submarine to attack it
float CAIHelper::GetAttackRange() const
{
	return mfAttackRange;
}

// Set how long the Shark should swim away from the Submarine for after attacking it
// NOTE: Time should be in milliseconds (1000ms = 1s)
void CAIHelper::SetHitDelay(unsigned long _ulHitDelay)
{
	mulHitDelay = _ulHitDelay;
}

// Get how long the Shark should swim away from the Submarine for after attacking it
// NOTE: Time is in milliseconds (1000ms = 1s)
unsigned long CAIHelper::GetHitDelay() const
{
	return mulHitDelay;
}

// Set how often the Sharks Health should degrade by a point
// NOTE: Time should be in milliseconds (1000ms = 1s)
void CAIHelper::SetHealthDegradeDelay(unsigned long _ulHealthDegradeDelay)
{
	mulHealthDegradeDelay = _ulHealthDegradeDelay;
}

// Get how often the Sharks Health should degrade by a point
unsigned long CAIHelper::GetHealthDegradeDelay() const
{
	return mulHealthDegradeDelay;
}

// Set the Velocity Coefficient
void CAIHelper::SetVelocityCoefficient(float _fVelocityCoefficient)
{
	mfVelocityCoefficient = _fVelocityCoefficient;
}

// Get the Velocity Coefficient
float CAIHelper::GetVelocityCoefficient() const
{
	return mfVelocityCoefficient;
}


// General Functions

// Set the Range at which the Fish senses the Shark, and vica-versa
void CAIHelper::SetSenseFishRange(float _fSenseFishRange)
{
	mfSenseFishRange = _fSenseFishRange;
}

// Get the Range at which the Fish senses the Shark, and vica-versa
float CAIHelper::GetSenseFishRange() const
{
	return mfSenseFishRange;
}

// Set interval of how long to wait before updating AI
void CAIHelper::SetUpdateTimeInterval(unsigned long _ulUpdateIntervalTime)
{
	mulUpdateTimeInterval = _ulUpdateIntervalTime;
}

// Get interval of how long to wait before updating AI
unsigned long CAIHelper::GetUpdateTimeInterval() const
{
	return mulUpdateTimeInterval;
}