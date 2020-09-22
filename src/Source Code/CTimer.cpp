// CTimer.cpp

#include "CTimer.h"

/////////////////////////////////////////////////////////////////////
// CClock Functions

// Initialize the private data
CClock* CClock::SmcpClockInstance						= NULL;
unsigned long CClock::SmulSavedTime						= timeGetTime();
float CClock::SmfScaleFactor							= 1.0f;	// Initialize to Normal speed
float CClock::SmfNextScaleFactor						= 1.0f; // Initialize to Normal speed
unsigned long CClock::SmulScaleFactorDuration			= 0;
unsigned long CClock::SmulScaleFactorDurationStartTime	= 0;


// Constructor - Does nothing
CClock::CClock()
{}

// Destructor - Does nothing
CClock::~CClock()
{}

// Returns a pointer to the class's Instance
CClock* CClock::ReturnSingletonInstance()
{
	// If class has been Instanced already
	if (SmcpClockInstance != NULL)
	{
		// Return pointer to class Instance
		return SmcpClockInstance;
	}
	
	// Else class has not been Instanced yet so Instance it and return it
	SmcpClockInstance = new CClock();
	return SmcpClockInstance;
}

// Returns the actual amount of Time Elapsed since the last Reset, ignoring the Scale Factor, and Resets the Clock
unsigned long CClock::ReturnActualElapsedTimeAndResetClock()
{
	// Get old Saved Time
	unsigned long ulOldSavedTime = ReturnSavedTime();

	// Reset the Clock
	ResetClock();

	// Return how much time had elapsed since last Reset
	return (ReturnCurrentTime() - ulOldSavedTime);
}

// Returns the actual amount of Time Elapsed since the last Reset, ignoring the Scale Factor, without resetting the Clock
unsigned long CClock::ReturnActualElapsedTimeWithoutResettingClock()
{
	// Return how much time had elapsed since last Reset
	return (ReturnCurrentTime() - ReturnSavedTime());
}

// Returns the Time Elapsed * Scale Factor, and Resets the Clock
unsigned long CClock::ReturnElapsedTimeAndResetClock()
{
	// Get old Saved Time
	unsigned long ulOldSavedTime = ReturnSavedTime();

	// Reset the Clock
	ResetClock();

	// Return how much time had elapsed between old Saved Time and new one
	return ((unsigned long)((ReturnCurrentTime() - ulOldSavedTime) * SmfScaleFactor));
}

// Returns the Time Elapsed * Scale Factor, without Resetting the Clock
unsigned long CClock::ReturnElapsedTimeWithoutResettingClock()
{
	// Return how much time had elapsed since last Reset
	return ((unsigned long)((ReturnCurrentTime() - ReturnSavedTime()) * SmfScaleFactor)); 
}


// Set and Return Scale Factor for Clocks Modified Elapsed Time
// After ulDuration expires, the Scale Factor will be set to fNextScaleFactor, unless the
//	Duration == 0 in which case fScaleFactor is used until explicitly changed again later
// NOTE: ScaleFactor = 0.5 returns half of elapsed time, ScaleFactor = 2 returns 2 times elapsed time
//		 ScaleFactor = 1.0 returns actual amount of elapsed time, ScaleFactor = 0 pauses Timer
void CClock::SetScaleFactor(float fScaleFactor, unsigned long ulDuration, float fNextScaleFactor)
{
	// Set the current Scale Factor to use
	SmfScaleFactor = fScaleFactor;

	// If no Duration was specified for this Scale effect
	if (ulDuration == 0)
	{
		// Make sure to continue using the Scale Factor after Duration expires
		fNextScaleFactor = fScaleFactor;
	}

	// Record the Current Time the new Scale Factor took affect
	SmulScaleFactorDurationStartTime = ReturnCurrentTime();
	
	// Record the Duration the new Scale Factor should last for
	SmulScaleFactorDuration = ulDuration;

	// Record what the Scale Factor should change to once the Duratation expires
	SmfNextScaleFactor = fNextScaleFactor;
}

// Return Scale Factor for Clocks Modified Elapsed Time
// NOTE: ScaleFactor = 0.5 returns half of elapsed time, ScaleFactor = 2 returns 2 times elapsed time
//		 ScaleFactor = 1.0 returns actual amount of elapsed time
float CClock::ReturnScaleFactor() const
{
	return SmfScaleFactor;
}

// Returns the Saved Time
unsigned long CClock::ReturnSavedTime()
{
	// Restores the Clocks Scale Factor if it should no longer be used
	RestoreScaleFactorIfExpired();

	return SmulSavedTime;
}

// Returns the Current system Time in milliseconds (1000ms = 1s)
unsigned long CClock::ReturnCurrentTime()
{
	// Restores the Clocks Scale Factor if it should no longer be used
	RestoreScaleFactorIfExpired();

	return timeGetTime();
}

// Resets the Clock
void CClock::ResetClock()
{
	// Get the new Saved Time
	SmulSavedTime = ReturnCurrentTime();
}

// Restores the Scale Factor to 1.0 if it should no longer be used
void CClock::RestoreScaleFactorIfExpired()
{
	// Check to see if a Scale Duration Timer is running
	if (SmulScaleFactorDurationStartTime != 0)
	{
		// If this Timer has expired
		if ((timeGetTime() - SmulScaleFactorDurationStartTime) > SmulScaleFactorDuration)
		{
			// Change to use the new specified Scale Factor
			SmfScaleFactor = SmfNextScaleFactor;

			// Reset Scale Duration Timer Start Time
			SmulScaleFactorDurationStartTime = 0;
		}
	}
}




/////////////////////////////////////////////////////////////////////
// CTimer Functions

// Constructor
CTimer::CTimer(bool bEffectedByClockScaleFactor)
{
	// Initialize private data
	mcpClock = CClock::ReturnSingletonInstance();
	RestartTimerUsingCurrentTime();	// Sets mulStartTime
	mbEffectedByClockScaleFactor = bEffectedByClockScaleFactor;
	mfTimerScaleFactor = 1.0f;
	mulTimerScaleDuration = 0;
	mulTimerScaleDurationStartTime = 0;
}

// Resets the Start Time to the Current Time
void CTimer::RestartTimerUsingCurrentTime()
{
	mulStartTime = ReturnCurrentTime();
}

// Resets the Start Time to the Clocks Saved Time
void CTimer::RestartTimerUsingSavedTime()
{
	mulStartTime = ReturnSavedTime();
}

// Returns True if ulTime has passed (Current Time - Start Time), False if not
// NOTE: Restarts Timer if True
bool CTimer::XTimeHasPassedUsingCurrentTime(unsigned long ulTimeInMilliseconds)
{
	if (ReturnTimePassedSinceLastRestartUsingCurrentTime() < ulTimeInMilliseconds)
	{
		// Return that ulTimeInMilliseconds has not yet elapsed
		return false;
	}

	RestartTimerUsingCurrentTime();	// Restart the Timer

	// Return that ulTimeInMilliseconds has elapsed
	return true;
}

// Returns True if ulTime has passed (Clocks Saved Time - Start Time), False if not
// NOTE: Restarts Timer if True
bool CTimer::XTimeHasPassedUsingSavedTime(unsigned long ulTimeInMilliseconds)
{
unsigned long test = 0;
	if (test = ReturnTimePassedSinceLastRestartUsingSavedTime() < ulTimeInMilliseconds)
	{
		// Return that ulTimeInMilliseconds has not yet elapsed
		return false;
	}

	RestartTimerUsingSavedTime();	// Restart the Timer

	// Return that ulTimeInMilliseconds has elapsed
	return true;
}

// Returns True if ulTime has passed (Current Time - Start Time), False if not
// NOTE: Does not restart Timer
bool CTimer::XTimeHasPassedUsingCurrentTimeWithoutRestartingTimer(unsigned long ulTimeInMilliseconds)
{
	if (ReturnTimePassedSinceLastRestartUsingCurrentTime() < ulTimeInMilliseconds)
	{
		return false;	// Return that ulTimeInMilliseconds has not yet elapsed
	}

	return true;		// Return that ulTimeInMilliseconds has elapsed
}

// Returns True if ulTime has passed (Saved Time - Start Time), False if not
// NOTE: Does not restart Timer
bool CTimer::XTimeHasPassedUsingSavedTimeWithoutRestartingTimer(unsigned long ulTimeInMilliseconds)
{
	if (ReturnTimePassedSinceLastRestartUsingSavedTime() < ulTimeInMilliseconds)
	{
		return false;	// Return that ulTimeInMilliseconds has not yet elapsed
	}

	return true;		// Return that ulTimeInMilliseconds has elapsed
}

// Returns how much time has passed since last check using the Current Time
unsigned long CTimer::ReturnTimePassedSinceLastRestartUsingCurrentTime()
{
	// If this Timer is succeptable to the Clock Scale Factor
	if (mbEffectedByClockScaleFactor == true)
	{
		// Return the Time Passed * the Clock Scale Factor
		return ((unsigned long)(((ReturnCurrentTime() - ReturnStartTime()) * mcpClock->ReturnScaleFactor()) * mfTimerScaleFactor));
	}else
	{
		// Return the Time Passed
		return ((unsigned long)((ReturnCurrentTime() - ReturnStartTime()) * mfTimerScaleFactor));
	}
}

// Returns how much time has passed since last check using the Clocks Saved Time
unsigned long CTimer::ReturnTimePassedSinceLastRestartUsingSavedTime()
{
	// If this Timer is succeptable to the Clock Scale Factor
	if (mbEffectedByClockScaleFactor == true)
	{
		// Return the Time Passed * the Clock Scale Factor
		return ((unsigned long)(((ReturnSavedTime() - ReturnStartTime()) * mcpClock->ReturnScaleFactor()) * mfTimerScaleFactor));
	}else
	{
		// Return the Time Passed
		return ((unsigned long)((ReturnSavedTime() - ReturnStartTime()) * mfTimerScaleFactor));
	}
}

// Returns how much time has passed since last check using the Current Time
// NOTE: Also restarts the Timer
unsigned long CTimer::ReturnTimePassedSinceLastRestartUsingCurrentTimeAndRestartTimer()
{
	unsigned long ulTimePassed = 0;

	// Get the Time Passed
	ulTimePassed = ReturnTimePassedSinceLastRestartUsingCurrentTime();

	// Restart the Timer
	RestartTimerUsingCurrentTime();

	// Return how much Time has Passed since last check
	return ulTimePassed;
}

// Returns how much time has passed since last check using the Clocks Saved Time
// NOTE: Also restarts the Timer
unsigned long CTimer::ReturnTimePassedSinceLastRestartUsingSavedTimeAndRestartTimer()
{
	unsigned long ulTimePassed = 0;

	// Get the Time Passed
	ulTimePassed = ReturnTimePassedSinceLastRestartUsingSavedTime();

	// Restart the Timer
	RestartTimerUsingSavedTime();

	// Return how much Time has Passed since last check
	return ulTimePassed;
}

// Set if this Timer is effected by the Clocks Scale Factor or not
void CTimer::TimerIsAffectedByClockScaleFactor(bool bTrueOrFalse)
{
	mbEffectedByClockScaleFactor = bTrueOrFalse;
}

// Return if this Timer is effected by the Clocks Scale Factor or not
bool CTimer::ReturnIfTimerIsAffectedByClockScaleFactor() const
{
	return mbEffectedByClockScaleFactor;
}

// Set this Timers individual Scale Factor
// NOTE: ScaleFactor = 0.5 returns half of elapsed time, ScaleFactor = 2 returns 2 times elapsed time
//		 ScaleFactor = 1.0 returns actual amount of elapsed time, ScaleFactor = 0 pauses Timer
// NOTE: ulDuration = 0 means to use the given Scale Factor until explicitly changed again later
// NOTE: After ulDuration expires, the Scale Factor will be set back to 1.0 automatically
void CTimer::SetTimerScaleFactor(float fScaleFactor, unsigned long ulDuration)
{
	// Set the new Scale Factor
	mfTimerScaleFactor = fScaleFactor;

	// If a Duration was specified for this Scale effect
	if (ulDuration != 0)
	{
		// Record the Current Time the new Scale Factor took affect
		mulTimerScaleDurationStartTime = ReturnCurrentTime();
		
		// Record the Duration the new Scale Factor should last for
		mulTimerScaleDuration = ulDuration;
	}
}

// Return this Timers individual Scale Factor
float CTimer::ReturnTimerScaleFactor() const
{
	return mfTimerScaleFactor;
}

// Returns the time the Timer was started
unsigned long CTimer::ReturnStartTime() const
{
	return mulStartTime;
}

// Returns the Clocks Saved Time
unsigned long CTimer::ReturnSavedTime()
{
	// Restores the Timers Scale Factor if it should no longer be used
	RestoreTimerScaleFactorIfExpired();

	return mcpClock->ReturnSavedTime();
}

// Returns the Current Time
unsigned long CTimer::ReturnCurrentTime()
{
	// Restores the Timers Scale Factor if it should no longer be used
	RestoreTimerScaleFactorIfExpired();

	return mcpClock->ReturnCurrentTime();
}

// Restores the Scale Factor to 1.0 if it should no longer be used
void CTimer::RestoreTimerScaleFactorIfExpired()
{
	// Check to see if a Scale Duration Timer is running
	if (mulTimerScaleDurationStartTime != 0)
	{
		// If this Timer has expired
		if ((timeGetTime() - mulTimerScaleDurationStartTime) > mulTimerScaleDuration)
		{
			// Restore the original Scale Factor
			mfTimerScaleFactor = 1.0f;

			// Reset Scale Duration Timer Start Time
			mulTimerScaleDurationStartTime = 0;
		}
	}
}



/////////////////////////////////////////////////////////////////////
// CScheduler Functions

// Initialize the private data
CScheduler* CScheduler::SmcpSchedulerInstance	= NULL;
list<SEvent> CScheduler::SmcEventList;
int CScheduler::SmiEventIDCounter				= 0;


// Constructor - Does nothing
CScheduler::CScheduler()
{}

// Destructor - Removes all Events from the list
CScheduler::~CScheduler()
{
	// Remove all Events from the list
	RemoveAllScheduledEvents();
}

// Returns a pointer to the class's Instance
CScheduler* CScheduler::ReturnSingletonInstance()
{
	// If class has been Instanced already
	if (SmcpSchedulerInstance != NULL)
	{
		// Return pointer to class Instance
		return SmcpSchedulerInstance;
	}
	
	// Else class has not been Instanced yet so Instance it and return it
	SmcpSchedulerInstance = new CScheduler();
	return SmcpSchedulerInstance;
}

// Schedule a new Event and return its unique ID
// _vpFunctionToCall is the global or static Function to call when the event fires
// _ulDurationBeforeCallingFunction is how long to wait before firing the event (calling the Function)
// _bRepeatUntilStopped should be set to true if you want the same Function to be called at an interval of _ulDurationBeforeCallingFunction, false if it should be called only once
// _bDurationEffectedByClockScale should be set to true if you want Clock Scaling to effect the time before the Event is fired
//	NOTE: If _vpFunctionToCall returns false and _bRepeatUntilStopped == true, the Event will be deleted and will not be repeated anymore
int CScheduler::ScheduleNewEvent(bool (*Function)(void* vpOptionalData), unsigned long _ulDurationBeforeCallingFunction, void* _vpOptionalDataToPass, int _iNumberOfTimesToCallFunction, unsigned long _ulDurationInBetweenCalls, bool _bDurationEffectedByClockScale)
{
	// Create the Event with the specified attributes
	SEvent sNewEvent;
	sNewEvent.iEventID = SmiEventIDCounter++;
	sNewEvent.Function = Function;
	sNewEvent.vpOptionalDataToPass = _vpOptionalDataToPass;
	sNewEvent.ulDurationRemaining = _ulDurationBeforeCallingFunction;
	sNewEvent.iNumberOfTimesToCallFunction = _iNumberOfTimesToCallFunction;
	sNewEvent.ulDurationBetweenCalls = _ulDurationInBetweenCalls;
	sNewEvent.cEventTimer.TimerIsAffectedByClockScaleFactor(_bDurationEffectedByClockScale);

	// Record if the event should keep firing until explicitly stopped or not
	sNewEvent.bKeepFiringEventUntilStopped = (_iNumberOfTimesToCallFunction == 0) ? true : false;
	
	// Add the new Event to the Event list
	SmcEventList.push_front(sNewEvent);

	// Return the ID of this Event
	return sNewEvent.iEventID;
}

// Removes a Scheduled Event from the list so it is not fired
// Returns true if the Event was found, false if not
bool CScheduler::RemoveScheduledEvent(int _iEventID)
{
	// Backup size of list before attempting remove
	int iOldListSize = (int)SmcEventList.size();

	// Create Dummy Event with the ID we want to remove
	SEvent sDummyEvent;
	sDummyEvent.iEventID = _iEventID;
	
	// Try removing the Event with specified ID
	SmcEventList.remove(sDummyEvent);

	// Return if an Event was removed or not
	return (iOldListSize > (int)SmcEventList.size());
}

// Purges all Events from the list
void CScheduler::RemoveAllScheduledEvents()
{
	// While there are still Events in the list
	while (!SmcEventList.empty())
	{
		// Remove the first Event in the list
		SmcEventList.pop_front();
	}
}

// Updates all Events and Fires the Events which should be fired (should be called once per frame)
void CScheduler::ProcessAllEvents()
{
	// Temp local variables
	int iIndex = 0;
	unsigned long ulTimeElapsed = 0;
	bool bContinueRepeating = false;

	// Store the Number of Events in the list
	int iNumberOfEvents = (int)SmcEventList.size();

	// Loop through all Events and update their Remaining Durations
	list<SEvent>::iterator sIterator = SmcEventList.begin(); // Get a pointer to the first Event in the list
	for (iIndex = 0; iIndex < iNumberOfEvents; iIndex++)
	{
		// Get how much time has elapsed since the last check
		ulTimeElapsed = sIterator->cEventTimer.ReturnTimePassedSinceLastRestartUsingSavedTimeAndRestartTimer();
		
		// Update this Events DurationRemaining
		sIterator->ulDurationRemaining = (ulTimeElapsed > sIterator->ulDurationRemaining) ? 0 : (sIterator->ulDurationRemaining - ulTimeElapsed);
		
		// If this Event should fire
		if (sIterator->ulDurationRemaining == 0)
		{
			// Call the global or static Function with the Optional Data as a parameter
			bContinueRepeating = sIterator->Function(sIterator->vpOptionalDataToPass);

			// Decrement the number of times this function has been called
			sIterator->iNumberOfTimesToCallFunction--;

			// If the Event should continue to repeat
			if ((sIterator->bKeepFiringEventUntilStopped || sIterator->iNumberOfTimesToCallFunction > 0) 
				&& bContinueRepeating)
			{
				// Reset the waiting Duration before firing again
				sIterator->ulDurationRemaining = sIterator->ulDurationBetweenCalls;
			}
		}

		// If this Event should be removed from the list now that it's done firing (not going to repeat)
		if (sIterator->ulDurationRemaining == 0)
		{
			// Delete this Event from the list (and move to the next Event)
			sIterator = SmcEventList.erase(sIterator);
		}
		else
		{
			// Move to the next Event in the list
			sIterator++;
		}
	}
}

// Returns there are any Event Scheduled or not
bool CScheduler::IsEmpty() const
{
	return SmcEventList.empty();
}