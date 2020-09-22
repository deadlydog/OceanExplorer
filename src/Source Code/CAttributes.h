/////////////////////////////////////////////////////////////////////
// CAttributes.h (and CAttributes.cpp)
// Written by Daniel Schroeder
// Created on September 28, 2007
// Last Updated September 28, 2007
//-------------------------------------------------------------------
//	The CAttributes class can hold information about an object, such
// as it's health (hit points), armor, shields, lives, points, etc
/////////////////////////////////////////////////////////////////////

#ifndef CATTRIBUTES_H
#define CATTRIBUTES_H

// Status' an object may have
const int GiSTATUS_NONE = 0;
const int GiSTATUS_ACTIVE = 1;
const int GiSTATUS_INACTIVE = 2;

class CAttributes
{
public:

	//---------- Contructors and Copy Functions ----------

	CAttributes();												// Default Constructor
	~CAttributes();												// Destructor

	CAttributes(CAttributes const& cAttributesToCopyFrom);		// Copy Constructor
	void CopyFrom(CAttributes const& cAttributesToCopyFrom);	// Copy from another object

	// Erase all information as if object was just created
	void Purge();


	//---------- Functions ----------

	// Set/Increase or Decrease/Return Health
	void SetHealth(int iHealth);
	void SetHealthRelative(int iHealthToAdd);
	int ReturnHealth() const;

	// Set/Return Max Health
	// Will only limit SetHealth() if MaxHealth is not -1
	void SetMaxHealth(int iMaxHealth);
	int ReturnMaxHealth() const;

	// Set/Return Min Health
	// Will only limit SetHealth() if MinHealth is not -1
	void SetMinHealth(int iMinHealth);
	int ReturnMinHealth() const;

	// Set/Return Armor
	// Armor should reduce the amount of damage taken by the Object
	// NOTE: User will have to take Armor into account themselves when determining damage taken
	void SetArmor(float fArmor);
	float ReturnArmor() const;

	// Set/Return Max Armor
	// Will only limit SetArmor() if MaxArmor is not -1
	void SetMaxArmor(float fMaxArmor);
	float ReturnMaxArmor() const;

	// Set Shield Strength
	// Shields should take damage instead of Object
	// NOTE: User will have to take Shield Strength into account themselves when determining damage taken
	void SetShieldStrength(float fShieldStrength);
	float ReturnShieldStrength() const;

	// Set/Return Max Shield Strength
	// Will only limit SetShieldStrength() if MaxShieldStrength is not -1
	void SetMaxShieldStrength(float fMaxShieldStrength);
	float ReturnMaxShieldStrength() const;

	// Set/Return Number Of Lives
	void SetNumberOfLives(int iLives);
	int ReturnNumberOfLives() const;

	// Set/Return Max Number Of Lives
	// Will only limit SetNumberOfLives() if MaxLives is not -1
	void SetMaxNumberOfLives(int iMaxLives);
	int ReturnMaxNumberOfLives() const;

	// Set/Return Objects Status
	void SetStatus(int iStatus = GiSTATUS_NONE);
	int ReturnStatus() const;

	// Set/Increase or Decrease/Return number of Points object is worth
	// NOTE: Points are allowed to go below zero
	void SetNumberOfPoints(int iPoints);
	void SetNumberOfPointsRelative(int iPointsToAdd);
	int ReturnNumberOfPoints() const;

protected:

	int miHealth, miMaxHealth, miMinHealth;		// Objects Hit Points (HP)
	float mfArmor, mfMaxArmor;					// How much Armor the Object has
	float mfShieldStrength, mfMaxShieldStrength;// Objects Shield Strength
	int miLives, miMaxLives;					// Number of Lives left
	int miStatus;								// Objects current status
	int miPoints;								// Players Points / Points earned from killing this enemy
};

#endif
