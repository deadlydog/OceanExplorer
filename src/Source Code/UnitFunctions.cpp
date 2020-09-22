#ifndef UNIT_FUNCTIONS_CPP
#define UNIT_FUNCTIONS_CPP

#include <cmath>
using namespace std;

// These functions are meant to take a value between 0.0 and 1.0 and return a value between zero and one

// Function returns the given value, making sure it is a unit value (between 0.0 and 1.0 inclusive)
static float GetValidUnitValue(float x)
{
	if (x < 0.0)
	{
		x = 0.0;
	}
	else if (x > 1.0)
	{
		x = 1.0;
	}
	return x;
}

// Returns x
static float UnitLinear(float x)
{
	return GetValidUnitValue(x);
}

// Returns x^2 (squared)
static float UnitSquared(float x)
{
	return GetValidUnitValue(x * x);
}

// Returns x^3 (cubed)
static float UnitCubed(float x)
{
	return GetValidUnitValue(x * x * x);
}

// Returns x^4 (quaded?)
static float UnitQuaded(float x)
{
	return GetValidUnitValue(x * x * x * x);
}

// Returns the square root of x
static float UnitSquareRoot(float x)
{
	return GetValidUnitValue(sqrt(x));
}

// Returns 0.0 if x < 0.5, and 1.0 otherwise
static float UnitEqualChanceBinary(float x)
{
	float fValue;

	if (x < 0.5f)
	{
		fValue = 0.0f;
	}
	else
	{
		fValue = 1.0f;
	}
	return GetValidUnitValue(fValue);
}


#endif
