#include "CAttributes.h"

//---------- Contructors and Copy Functions ----------

// Default Constructor
CAttributes::CAttributes()
{
	// Initialize all data
	Purge();
}

// Destructor
CAttributes::~CAttributes()
{
	Purge();
}

// Copy Constructor
CAttributes::CAttributes(CAttributes const& cAttributesToCopyFrom)
{
	// Copy data over from other object
	CopyFrom(cAttributesToCopyFrom);
}

// Copy from another object
void CAttributes::CopyFrom(CAttributes const& cAttributesToCopyFrom)
{
	// Erase all info from this Object
	Purge();

	// Copy Attributes data
	miHealth = cAttributesToCopyFrom.ReturnHealth();
	miMaxHealth = cAttributesToCopyFrom.ReturnMaxHealth();
	miMinHealth = cAttributesToCopyFrom.ReturnMinHealth();
	mfArmor = cAttributesToCopyFrom.ReturnArmor();
	mfMaxArmor = cAttributesToCopyFrom.ReturnMaxArmor();
	mfShieldStrength = cAttributesToCopyFrom.ReturnShieldStrength();
	mfMaxShieldStrength = cAttributesToCopyFrom.ReturnMaxShieldStrength();
	miLives = cAttributesToCopyFrom.ReturnNumberOfLives();
	miMaxLives = cAttributesToCopyFrom.ReturnMaxNumberOfLives();
	miStatus = cAttributesToCopyFrom.ReturnStatus();
	miPoints = cAttributesToCopyFrom.ReturnNumberOfPoints();
}

// Erase all info as if Object was just created
void CAttributes::Purge()
{
	// Reset all private data
	miHealth = miStatus = miLives = miPoints = 0;
	mfArmor = mfShieldStrength = 0.0;
	miMaxHealth = miMinHealth = miMaxLives = -1;
	mfMaxArmor = mfMaxShieldStrength = -1.0;
}


//---------- Functions ----------


// Set Health
void CAttributes::SetHealth(int iHealth)
{
	// Set Health
	miHealth = iHealth;

	// If Health is too high
	if (miHealth > miMaxHealth && miMaxHealth != -1)
	{
		// Set Health to the Max Health it can be
		miHealth = miMaxHealth;
	}

	// If Health is too low
	if (miHealth < miMinHealth && miMinHealth != -1)
	{
		// Set Health to the Min Health it can be
		miHealth = miMinHealth;
	}
}

// Add HealthToAdd to Objects Health
void CAttributes::SetHealthRelative(int iHealthToAdd)
{
	// Set Health
	SetHealth(miHealth + iHealthToAdd);
}

// Return Health
int CAttributes::ReturnHealth() const
{
	return miHealth;
}

// Set Max Health
// Will only limit SetHealth() if MaxHealth is not -1
void CAttributes::SetMaxHealth(int iMaxHealth)
{
	// Set Max Health
	miMaxHealth = iMaxHealth;

	// Make sure Health value is still valid
	SetHealth(miHealth);
}

// Return Max Health
int CAttributes::ReturnMaxHealth() const
{
	return miMaxHealth;
}

// Set Min Health
// Will only limit SetHealth() if MinHealth is not -1
void CAttributes::SetMinHealth(int iMinHealth)
{
	miMinHealth = iMinHealth;
}

// Return Min Health
int CAttributes::ReturnMinHealth() const
{
	return miMinHealth;
}

// Set Armor
void CAttributes::SetArmor(float fArmor)
{
	// Set Armor
	mfArmor = fArmor;

	// If Armor is too high
	if (mfArmor > mfMaxArmor && mfMaxArmor != -1)
	{
		// Set Armor to the Max it can be
		mfArmor = mfMaxArmor;
	}
}

// Return Armor
float CAttributes::ReturnArmor() const
{
	return mfArmor;
}

// Set Max Armor
// Will only limit SetArmor() if MaxArmor is not -1
void CAttributes::SetMaxArmor(float fMaxArmor)
{
	// Set Max Armor
	mfMaxArmor = fMaxArmor;

	// Make sure Armor value is still valid
	SetArmor(mfArmor);
}

// Return Max Armor
float CAttributes::ReturnMaxArmor() const
{
	return mfMaxArmor;
}

// Set Shield Strength
void CAttributes::SetShieldStrength(float fShieldStrength)
{
	// Set the Shield Strength
	mfShieldStrength = fShieldStrength;

	// If ShieldStrength is too high
	if (mfShieldStrength > mfMaxShieldStrength && mfMaxShieldStrength != -1)
	{
		mfShieldStrength = mfMaxShieldStrength;
	}
}

// Return Shield Strength
float CAttributes::ReturnShieldStrength() const
{
	return mfShieldStrength;
}

// Set Max Shield Strength
// Will only limit SetShieldStrength() if MaxShieldStrength is not -1
void CAttributes::SetMaxShieldStrength(float fMaxShieldStrength)
{
	// Set the Max Shield Strength
	mfMaxShieldStrength = fMaxShieldStrength;

	// Make sure Sheild Strength value is still valid
	SetShieldStrength(mfShieldStrength);
}

// Return Max Shield Strength
float CAttributes::ReturnMaxShieldStrength() const
{
	return mfMaxShieldStrength;
}

// Set number of Lives
void CAttributes::SetNumberOfLives(int iLives)
{
	// Set Number of Lives
	miLives = iLives;

	// If Lives is too high
	if (miLives > miMaxLives && miMaxLives != -1)
	{
		// Set Lives to the Max it can be
		miLives = miMaxLives;
	}
}

// Return number of Lives
int CAttributes::ReturnNumberOfLives() const
{
	return miLives;
}

// Set Max Number Of Lives
// Will only limit SetNumberOfLives() if MaxLives is not zero
void CAttributes::SetMaxNumberOfLives(int iMaxLives)
{
	// Set Max Lives
	miMaxLives = iMaxLives;

	// Make sure Lives value is still valid
	SetNumberOfLives(miLives);
}

// Return Max Number Of Lives
int CAttributes::ReturnMaxNumberOfLives() const
{
	return miMaxLives;
}

// Set Objects Status
void CAttributes::SetStatus(int iStatus)
{
	miStatus = iStatus;
}

// Return Objects Status
int CAttributes::ReturnStatus() const
{
	return miStatus;
}

// Set number of Points object is worth
void CAttributes::SetNumberOfPoints(int iPoints)
{
	miPoints = iPoints;
}

// Add PointsToAdd to current number of Points
void CAttributes::SetNumberOfPointsRelative(int iPointsToAdd)
{
	// Add PointsToAdd to current number of Points
	miPoints += iPointsToAdd;
}

// Return number of Points object is worth
int CAttributes::ReturnNumberOfPoints() const
{
	return miPoints;
}
