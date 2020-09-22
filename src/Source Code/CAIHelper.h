/////////////////////////////////////////////////////////////////////
// CAIHelper.h (and CAIHelper.cpp)
// Written by Daniel Schroeder
// Created on October 19, 2007
// Last Updated October 19, 2007
//-------------------------------------------------------------------
//   The CAI class is used to control a CObject using Artificial Intelligence
/////////////////////////////////////////////////////////////////////

#ifndef CAI_HELPER_H
#define CAI_HELPER_H

class CObject;			//#include "CObject.h"	// Including CObject causes a recursive definition problem
#include "CTimer.h"
#include <Ogre.h>		// Include OGRE
#include <list>
using namespace std;

class CAIHelper
{
public:

	//---------- Contructors and Copy Functions ----------
		
	CAIHelper();										// Default Constructor
	~CAIHelper();										// Destructor

	CAIHelper(CAIHelper const& cAIHelperToCopyFrom);	// Copy Constructor
	void CopyFrom(CAIHelper const& cAIHelperToCopyFrom);// Copy from another object

	// Erase all information as if AI was just created
	void Purge();


	//---------- Set and Get Functions ----------

	// Fish Functions

	// Set/Get the Separation Coefficient
	// NOTE: The Separation, Alignment, Cohesion, and PreditorAvoidance Coefficients should sum up to one
	void SetSeparationCoefficient(float _fSeparationCoefficient);
	float GetSeparationCoefficient() const;

	// Set/Get the Alignment Coefficient
	// NOTE: The Separation, Alignment, Cohesion, and PreditorAvoidance Coefficients should sum up to one
	void SetAlignmentCoefficient(float _fAlignmentCoefficient);
	float GetAlignmentCoefficient() const;

	// Set/Get the Cohesion Coefficient
	// NOTE: The Separation, Alignment, Cohesion, and PreditorAvoidance Coefficients should sum up to one
	void SetCohesionCoefficient(float _fCohesionCoefficient);
	float GetCohesionCoefficient() const;

	// Set/Get the Preditor Avoidance Coefficient
	// NOTE: The Separation, Alignment, Cohesion, and PreditorAvoidance Coefficients should sum up to one
	void SetPreditorAvoidanceCoefficient(float _fPreditorAvoidanceCoefficient);
	float GetPreditorAvoidanceCoefficient() const;

	// Set/Get the Maximum Distance between 2 Object to consider them Neighbors
	void SetSenseNeighborRange(float _fNeighborSenseRange);
	float GetSenseNeighborRange() const;

	// Set/Get the Maximum Number of Neighbors allowed
	void SetMaxNumberOfNeighbors(int _iMaxNumberOfNeighbors);
	int GetMaxNumberOfNeighbors() const;

	// Set/Get the Age of the fish
	void SetAge(float _fAge);
	float GetAge() const;

	// Set/Get the Age at which the fish will Spawn into two fish
	void SetSpawnAge(float _fSpawnAge);
	float GetSpawnAge() const;

	// Set/Get if this Fish is a Flocking Fish or not
	void SetIsAFlockingFish(bool _bIsAFlockingFish);
	bool GetIsAFlockingFish() const;

	// Set/Get the distance at which the Shark can be detected
	void SetSenseSharkRange(float _fSenseSharkRange);
	float GetSenseSharkRange() const;


	// Shark Functions

	// Set/Get Health at which Shark is considered Nourished
	void SetNourishedHealth(int _iNourishedHealth);
	int GetNourishedHealth() const;

	// Set/Get Health at which Shark is considered Full
	void SetFullHealth(int _iFullHealth);
	int GetFullHealth() const;

	// Set/Get the Range at which the Shark Senses the Submarine
	void SetSenseSubmarineRange(float _fSenseSubmarineRange);
	float GetSenseSubmarineRange() const;

	// Set/Get how close the Shark must be to eat a Fish
	void SetFeedRange(float _fFeedRange);
	float GetFeedRange() const;

	// Set/Get how close Shark must be to Submarine to attack it
	void SetAttackRange(float _fAttackRange);
	float GetAttackRange() const;

	// Set/Get how long the Shark should swim away from the Submarine for after attacking it
	// NOTE: Time should be in milliseconds (1000ms = 1s)
	void SetHitDelay(unsigned long _ulHitDelay);
	unsigned long GetHitDelay() const;

	// Set/Get how often the Sharks Health should degrade by a point
	// NOTE: Time should be in milliseconds (1000ms = 1s)
	void SetHealthDegradeDelay(unsigned long _ulHealthDegradeDelay);
	unsigned long GetHealthDegradeDelay() const;

	// Set/Get the Velocity Coefficient
	void SetVelocityCoefficient(float _fVelocityCoefficient);
	float GetVelocityCoefficient() const;

	
	// General Functions

	// Set/Get the Range at which the Fish senses the Shark, and vica-versa
	void SetSenseFishRange(float _fSenseFishRange);
	float GetSenseFishRange() const;

	// Set/Get interval of how long to wait before updating
	void SetUpdateTimeInterval(unsigned long _ulUpdateIntervalTime);
	unsigned long GetUpdateTimeInterval() const;


	//---------- Public Data ---------

	list<CObject*> mcNeighborList;		// Holds a list of this Objects Neighbors
	list<CObject*> mcPreditorList;		// Holds a list of Preditors which may harm this Object

	CTimer mcUpdateTimer;				// Timer to be used for periodic updates
	CTimer mcHitDelayTimer;				// Timer used to keep track of how long ago Shark attacked Player
	CTimer mcDegradationTimer;			// Timer used to keep track of the last time Shark lost Health from moving, and the last time a Flocking Fish aged

private:

	// Fish Specific Data
	float mfSeparationCoefficient;		// Separation Coefficient
	float mfAlignmentCoefficient;		// Alignment Coefficient
	float mfCohesionCoefficient;		// Cohesion Coefficient
	float mfPreditorAvoidanceCoefficient;	// Preditor Avoidance Coefficient

	float mfSenseNeighborRange;			// Maximum distance between 2 Objects to consider them Neighbors
	int miMaxNumberOfNeighbors;			// Maximum number of Neighbors a fish can have
	
	float mfAge;						// The age of the fish
	float mfSpawnAge;					// The age at which the fish will spawn into two fish

	bool mbIsFlockingFish;				// Tells if this fish is a Flocking Fish or not
	float mfSenseSharkRange;			// The distance at which the Shark can be detected

	// Shark Specific Data
	int miNourishedHealth;				// How much Health the Shark must have to be considered Nourished
	int miFullHealth;					// How much Health the Shark must have to be considered Full
	float mfSenseSubmarineRange;		// The distance at which the Submarine can be detected
	float mfFeedRange;					// How close Shark must be to fish to eat it
	float mfAttackRange;				// How close Shark must be to submarine to attack it
	unsigned long mulHitDelay;			// How long the Shark should continue swimming away from the Submarine after attacking it
	unsigned long mulHealthDegradeDelay;// How often the Shark should lose a Health point
	float mfVelocityCoefficient;		// Used to help determine the Sharks Velocity (Health * VelocityCoefficient)

	// General Data
	float mfSenseFishRange;				// How close a Fish must be before the Object detects them, and vica-versa
	unsigned long mulUpdateTimeInterval;// How often this Objects AI should be updated
};

#endif
