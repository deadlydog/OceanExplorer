// CStateManager.cpp

#include "CStateManager.h"

// Constructor
CStateManager::CStateManager()
{
	// Initialize private data
	mspCurrentStateToProcess	= 0;	
	mspNextStateToProcess		= 0;		
	mspHead						= 0;
	mePurpose					= eInitialize;
	meNextStatePurpose			= eInitialize;
}

// Destructor
CStateManager::~CStateManager()
{
	ShutdownAllStates();
}

// Set the State Function to call
bool CStateManager::SetState(void (*Function)(void* vpFunction, EPurpose ePurpose), void* vpOptionalDataToPass)
{
	SGameState* spState;	// Temp pointer to Game States

	// If Function seems valid
	if (Function != 0)
	{
		// If Function State already exists
		if ((spState = FunctionIsInList(Function)) != 0)
		{
			// Move GameState to front of List
			MoveStateToFrontOfList(spState);

			// Set the States Optional Data To Pass in case it's changed
			spState->vpOptionalData = vpOptionalDataToPass;

			// Set specified State to run with a Purpose of Returning
			mspCurrentStateToProcess = spState;
			mePurpose = eReturning;
		}
		// Else State is not in list so create it
		else
		{
			// Create new State
			spState = new SGameState;

			// Set new States data and put it at front of State list
			spState->Function = Function;
			spState->vpOptionalData = vpOptionalDataToPass;

			// If this is the first State to go in the List
			if (mspHead == 0)
			{
				spState->spNext = 0;
			}
			// Else this is not the only State in the List
			else
			{
				spState->spNext = mspHead;
			}

			// Set to front of List
			mspHead = spState;

			// Set specified State to run with a Purpose of Initialize
			mspCurrentStateToProcess = spState;
			mePurpose = eInitialize;
		}

		// Return that everything went ok
		return true;
	}

	// Return that the function was not set
	return false;
}

// Set the Next State Function to call, but Shutdown the current one first
bool CStateManager::ShutdownCurrentStateAndSetNextState(void (*Function)(void* vpFunction, EPurpose ePurpose), void* vpOptionalDataToPass)
{
	SGameState* spState;	// Temp pointer to Game States

	// If Function seems valid
	if (Function != 0)
	{
		// Set the purpose for the current State to Shutdown
		mePurpose = eShutdown;

		// If Next Functions State is already in list
		if ((spState = FunctionIsInList(Function)) != 0)
		{
			// Move State to the front of the List
			MoveStateToFrontOfList(spState);

			// Set the States Optional Data To Pass in case it's changed
			spState->vpOptionalData = vpOptionalDataToPass;

			// Set specified Next State to run with a Purpose of Returning
			mspNextStateToProcess = spState;
			meNextStatePurpose = eReturning;
		}
		// Else State is not in list
		else
		{
			// Create new State
			spState = new SGameState;

			// Set new States data and put it at front of State list
			spState->Function = Function;
			spState->vpOptionalData = vpOptionalDataToPass;

			// If this is the first State to go in the List (although it shouldn't be - but just in case)
			if (mspHead == 0)
			{
				spState->spNext = 0;
			}
			// Else this is not the only State in the List
			else
			{
				spState->spNext = mspHead;
			}

			// Set to front of List
			mspHead = spState;

			// Set specified Next State to run with a Purpose of Initialize
			mspNextStateToProcess	= spState;
			meNextStatePurpose		= eInitialize;
		}

		// Return that everything went ok
		return true;
	}

	// Return that the function was not set
	return false;
}

// Returns to the State which called the Current State
// If bShutDownCurrentState == true, the Current State is shutdown before returning to the Previous State
bool CStateManager::ReturnToPreviousState(bool bShutDownCurrentState, void* vpOptionalDataToPass)
{
	// Temp pointer to the Previous State
	SGameState* spPreviousState;

	// If there is no Current or Previous State
	if (mspHead == 0 || (spPreviousState = mspHead->spNext) == 0)
	{
		return false;
	}

	// If the Current State should be shutdown
	if (bShutDownCurrentState)
	{
		// Return if everything went ok or not
		return ShutdownCurrentStateAndSetNextState(spPreviousState->Function, vpOptionalDataToPass);
	}
	// Else simply call the Previous State
	else
	{
		// Return if everything went ok or not
		return SetState(spPreviousState->Function, vpOptionalDataToPass);
	}
}

// Call the State Function
bool CStateManager::Process()
{
	// If there is no CurrentStateFunction to process
	if (mspCurrentStateToProcess == 0)
	{
		return false;
	}

	// If we are not Shutting down the Current State Function
	if (mePurpose != eShutdown)
	{
		// Simply call the Current State Function with the appropriate Purpose
		mspCurrentStateToProcess->Function(mspCurrentStateToProcess->vpOptionalData, mePurpose);

		// If Next State To Process is not set (This means the Current State is not Shutting Down)
		if (mspNextStateToProcess == 0)
		{
			// Set State to Normal
			mePurpose = eNormal;
		}
	}
	// Else we are Shutting down the Current State Function, so must prepare Next State Function
	else
	{
		// Call the Current State Function with the Shutdown Purpose
		mspCurrentStateToProcess->Function(mspCurrentStateToProcess->vpOptionalData, eShutdown);

		// Remove Current State Function from list
		RemoveStateFromList(mspCurrentStateToProcess);

		// Move NextState to the front of the List
		MoveStateToFrontOfList(mspNextStateToProcess);

		// Set the Current State Function to the new Next State Function
		mspCurrentStateToProcess = mspNextStateToProcess;
		mePurpose = meNextStatePurpose;

		// Reset the Next State data
		mspNextStateToProcess = 0;
		meNextStatePurpose = eInitialize;
	}

	return true;
}

// Returns the Current State Function the State Manager is processing
// Returns 0 if no Current State Function is being processes
void* CStateManager::ReturnCurrentStateFunction() const
{
	// If there is no State to process
	if (mspCurrentStateToProcess == 0)
	{
		return 0;
	}

	// Return the State Function which is being processed
	return mspCurrentStateToProcess->Function;
}

// Shutsdown all State Functions. Once done, Process() will return false when called
void CStateManager::ShutdownAllStates(void* vpOptionalDataToPass)
{
	SGameState* spState = mspHead;
	SGameState* spTemp	= 0;

	// Loop through and Shutdown every GameStateFunction
	while (spState != 0)
	{
		// Call Function with Shutdown Purpose
		spState->Function(vpOptionalDataToPass, eShutdown);

		// Move to Next GameStateFunction and delete Current one
		spTemp = spState;
		spState = spState->spNext;
		delete spTemp;
	}

	// Reset private data
	mspCurrentStateToProcess	= 0;	
	mspNextStateToProcess		= 0;		
	mspHead						= 0;
	mePurpose					= eInitialize;
	meNextStatePurpose			= eInitialize;
}


//********** Private Functions **********

// Returns the State if it is in the list
// Returns 0 if State not in list
SGameState* CStateManager::FunctionIsInList(void (*Function)(void* vpFunction, EPurpose ePurpose)) const
{
		SGameState* spState = mspHead;	// Temp pointer to Game States

		// Loop through list looking for Function
		while (spState != 0 && spState->Function != Function)
		{
			spState = spState->spNext;
		}

		// If Function State was found in List
		if (spState != 0)
		{
			return spState;
		}

		// Else return that Function was not found in List
		return 0;
}

// Removes a State from the List
// Returns true if State was in List and Removed, False if not
bool CStateManager::RemoveStateFromList(SGameState* spStateToRemove)
{
	SGameState* spState		= mspHead;	// Temp pointer to Game States
	SGameState* spPrevious	= 0;		// Temp pointer to Previous Game State

	// Loop through list looking for GameState
	while (spState != 0 && spState != spStateToRemove)
	{
		spPrevious = spState;
		spState = spState->spNext;
	}

	// If State was found
	if (spState != 0)
	{
		// If State is the first in the List
		if (spState == mspHead)
		{
			mspHead = spState->spNext;
			delete mspHead;
		}
		// Else State is somewhere in the middle or end of the list
		else
		{
			spPrevious->spNext = spState->spNext;
			delete spState;
		}

		// Return that State was Deleted
		return true;
	}

	// Else return that the State was not found
	return false;
}

// Moves a State to the front of the List
bool CStateManager::MoveStateToFrontOfList(SGameState* spStateToMove)
{
	SGameState* spState		= mspHead;	// Temp pointer to Game States
	SGameState* spPrevious	= 0;		// Temp pointer to Previous Game State

	// Loop through list looking for GameState
	while (spState != 0 && spState != spStateToMove)
	{
		spPrevious = spState;
		spState = spState->spNext;
	}

	// If State was found
	if (spState != 0)
	{
		// If State is not already at the front of the List
		if (spState != mspHead)
		{
			// If the State is not second in the list (ie Previous is not pointing to the Head)
			if (spPrevious != mspHead)
			{
				// Move State to the front of the List
				spPrevious->spNext = spState->spNext;
				spState->spNext = mspHead->spNext;
				mspHead = spState;
			}
			else
			{
				// Move State to the front of the List
				spPrevious->spNext = spState->spNext;
				spState->spNext = spPrevious;
				mspHead = spState;
			}			
		}

		// Return that State is now at front of the List
		return true;
	}

	// Else return that the State was not found
	return false;
}

