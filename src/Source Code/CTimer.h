/*****************************************************************************/
/* CTimer.h (and CTimer.cpp)                                                 */
/* Written by Daniel Schroeder                                               */
/* Created in 2004                                                           */
/* Last Updated October 29, 2007                                             */
/*---------------------------------------------------------------------------*/
/*   The CClock class is a singleton class so only one object of it can be   */
/* created, and this is done auotmatically. It is used to get the current    */
/* system time for the Timers. It can also Save and Return a specific Time   */
/* (I will use this feature to save what Time the current frame is occuring  */
/* at).                                                                      */
/*   Example of instancing the CClock class if needed:                       */
/*     CClock* cpClock = CClock::ReturnSingletonInstance();                  */
/*     cpClock->ReturnCurrentTime();                                         */
/*                                                                           */
/*   The CTimer class is used as a Timer to keep track of how much time in   */
/* milliseconds has passed. The Timers automatically reference the CClock    */
/* instance, so the CClock class can be completely ignored if wanted. Also,  */
/* can use either Current Time or Saved Time when doing calculations.		 */
/* NOTE: Can only use Saved Time functions if Clock is being Reset           */
/*		 periodically, such as every loop of a game							 */
/* Saved Time is recommended for many cases so that all Timers in the game   */
/* loop will calculate the same elapsed time between frames (i.e. so that    */
/* Timers at the start of the game loop will return the same amount of time  */
/* as the ones at the end of the game loop). This is because the Timers will */
/* calculate their elapsed time from their Start Time to the Clocks Saved    */
/* Time (which will be the same for all Timers until the Clock is reset      */
/* again), instead of from their Start Time to the Current Time, which will  */
/* be different for every timer.											 */
/*   NOTE: 1000 milliseconds = 1 second                                      */
/*   -NOTE: Must have winmm.lib included in project to work-                 */
/*****************************************************************************/

#ifndef CTIMER_H
#define CTIMER_H

#include <windows.h>	// Needed for timeGetTime() function
#include <list>			// Used to hold the Schedulers list of Events
// NOTE: Must also include winmm.lib in project for timeGetTime() to work

using namespace std;

// Singleton class (only one object can be created) to get the system time
// All Timers will go by this one single Clock
class CClock
{
public:

	~CClock();		// Destructor

	// Returns a pointer to the class's Instance
	static CClock* ReturnSingletonInstance();

	// Returns the actual amount of Time Elapsed since the last Reset, ignoring the Scale Factor
	unsigned long ReturnActualElapsedTimeAndResetClock();
	unsigned long ReturnActualElapsedTimeWithoutResettingClock();

	// Returns the Saved Time Elapsed * Scale Factor
	unsigned long ReturnElapsedTimeAndResetClock();
	unsigned long ReturnElapsedTimeWithoutResettingClock();

	// Set and Return Scale Factor for Clocks Modified Elapsed Time
	// After ulDuration expires, the Scale Factor will be set to fNextScaleFactor, unless the
	//	Duration == 0 in which case fScaleFactor is used until explicitly changed again later
	// NOTE: ScaleFactor = 0.5 returns half of elapsed time, ScaleFactor = 2 returns 2 times elapsed time
	//		 ScaleFactor = 1.0 returns actual amount of elapsed time, ScaleFactor = 0 pauses Timer
	void SetScaleFactor(float fScaleFactor, unsigned long ulDuration = 0, float fNextScaleFactor = 1.0);
	float ReturnScaleFactor() const;

	// NOTE: Be careful when calling these functions and using them to backup the current time, since
	//		 something like "if (ReturnSavedTime() > (BackedUpTime + Duration)" will not take into account
	//		 any Clock Scaling. A better solution would be: 
	//	if (DurationRemainging -= ReturnElapsedTimeWithoutResettingClock() > 0) (Make sure only called once per frame)
	unsigned long ReturnSavedTime();	// Returns the Saved Time
	unsigned long ReturnCurrentTime();	// Returns the Current Time

private:

	CClock();			// Constructor (Private because this is a Singleton class)
	void ResetClock();	// Resets the Clock

	static CClock* SmcpClockInstance;			// Holds pointer to Class Instance
	static unsigned long SmulSavedTime;			// Holds what Time it was last time Clock was reset

	static float SmfScaleFactor;				// Holds how much to Scale the Clocks returned elapsed time
	static float SmfNextScaleFactor;			// Holds how much to Scale the Clocks returned elapsed time once current Scale Duration expires
	static unsigned long SmulScaleFactorDuration;			// Holds how long to use the Scale Factor
	static unsigned long SmulScaleFactorDurationStartTime;	// Holds what time the Scale Factor started

	void RestoreScaleFactorIfExpired();	// Restores the Scale Factor to 1.0 if it should no longer be used
};


// Class to be used as a Timer to keep track of how much Time has elapsed
class CTimer
{
public:

	CTimer(bool bEffectedByClockScaleFactor = true);	// Constructor

	void RestartTimerUsingCurrentTime();	// Resets the Start Time to the Current Time
	void RestartTimerUsingSavedTime();		// Resets the Start Time to the Clocks Saved Time

	// Returns True if ulTime has passed (Saved/Current Time - Start Time), False if not
	// NOTE: Restarts Timer if True
	bool XTimeHasPassedUsingCurrentTime(unsigned long ulTimeInMilliseconds);
	bool XTimeHasPassedUsingSavedTime(unsigned long ulTimeInMilliseconds);

	// Returns True if ulTime has passed (Saved/Current Time - Start Time), False if not
	// NOTE: Does not restart Timer
	bool XTimeHasPassedUsingCurrentTimeWithoutRestartingTimer(unsigned long ulTimeInMilliseconds);
	bool XTimeHasPassedUsingSavedTimeWithoutRestartingTimer(unsigned long ulTimeInMilliseconds);
	
	// Returns how much time has passed since last check
	unsigned long ReturnTimePassedSinceLastRestartUsingCurrentTime();
	unsigned long ReturnTimePassedSinceLastRestartUsingSavedTime();

	// Returns how much time has passed since last check
	// NOTE: Also restarts the Timer
	unsigned long ReturnTimePassedSinceLastRestartUsingCurrentTimeAndRestartTimer();
	unsigned long ReturnTimePassedSinceLastRestartUsingSavedTimeAndRestartTimer();

	// Set and Return if this Timer is effected by the Clocks Scale Factor or not
	void TimerIsAffectedByClockScaleFactor(bool bTrueOrFalse = true);
	bool ReturnIfTimerIsAffectedByClockScaleFactor() const;

	// Set and Return this Timers individual Scale Factor
	// NOTE: ScaleFactor = 0.5 returns half of elapsed time, ScaleFactor = 2 returns 2 times elapsed time
	//		 ScaleFactor = 1.0 returns actual amount of elapsed time, ScaleFactor = 0 pauses Timer
	// NOTE: ulDuration = 0 means to use the given Scale Factor until explicitly changed again later
	// NOTE: After ulDuration expires, the Scale Factor will be set back to 1.0 automatically
	void SetTimerScaleFactor(float fScaleFactor, unsigned long ulDuration = 0);
	float ReturnTimerScaleFactor() const;

	unsigned long ReturnStartTime() const;	// Returns the time the Timer was started
	unsigned long ReturnSavedTime();		// Returns the Clocks Saved Time
	unsigned long ReturnCurrentTime();		// Returns the Current Time

private:

	CClock* mcpClock;					// Points to the Clock object used to actually get the system time
	unsigned long mulStartTime;			// Holds the time the Timer was started
	
	bool mbEffectedByClockScaleFactor;	// Holds if this Timer is effected by Clock slow downs or speed ups
	float mfTimerScaleFactor;			// Holds this Timers Scale Factor

	unsigned long mulTimerScaleDuration;			// Holds how long to use unusual Scale Factor for
	unsigned long mulTimerScaleDurationStartTime;	// Holds when the Timer Scale Factor was set

	void RestoreTimerScaleFactorIfExpired(); // Restores the Scale Factor to 1.0 if it should no longer be used
};



// Used by the CScheduler class to hold a Scheduled Events information
struct SEvent
{
	int iEventID;							// Unique ID of this Event
	bool (*Function)(void* vpOptionalData);	// Function to call once the Event should fire
	void* vpOptionalDataToPass;				// Optional data to pass into the Function being called

	unsigned long ulDurationRemaining;		// How long to wait before firing the Event

	int iNumberOfTimesToCallFunction;		// Holds how many times this Event should fire
	unsigned long ulDurationBetweenCalls;	// Holds how long to wait before firing the Event again
	bool bKeepFiringEventUntilStopped;		// Holds if this Event should keep firing until explicity stopped or not

	CTimer cEventTimer;						// Holds a Timer used to check when to fire the Event

	// Operator Overload Functions
	// Overload the == operator to compare against the IDs
	bool operator==(const SEvent& _sEvent) const
	{
		// Return if the Event has the specified ID or not
		return (this->iEventID == _sEvent.iEventID);
	}

	// Overload the != operator to compare against the IDs
	bool operator!=(const SEvent& _sEvent) const
	{
		// Return if the Event has the specified ID or not
		return !(this->iEventID == _sEvent.iEventID);
	}
};

// Singleton Class
// Class to be used as a Scheduler to call functions after a specified amount of time has elapsed
class CScheduler
{
public:

	// Destructor
	~CScheduler();

	// Returns a pointer to the class's Instance
	static CScheduler* ReturnSingletonInstance();

	// Schedule a new Event and return its unique ID
	// _vpFunctionToCall is the global or static Function to call when the Event fires
	//	_vpFunctionToCall should be of the form: bool FunctionName(void* vpOptionalData)
	// _ulDurationBeforeCallingFunction is how long to wait before firing the event (calling the Function) for the first time
	// _iNumberOfTimesToCallFunction is the number of times the Event should fire (number of times to call the Function)
	//	If _iNumberOfTimesToCallFunction is set to zero, the Event will be fired repeatedly until it is explicitly stopped or function call returns false
	// _ulDurationInBetweenCalls is how long to wait between firing the Event, when it is set to be fired more than once
	// _bDurationEffectedByClockScale should be set to true if you want Clock Scaling to effect the time before the Event is fired
	//	NOTE: If _vpFunctionToCall returns false, the Event will be deleted and will not be repeated anymore, regardless of if it's supposed to keep repeating or not
	int ScheduleNewEvent(bool (*Function)(void* vpOptionalData), unsigned long _ulDurationBeforeCallingFunction, void* _vpOptionalDataToPass = NULL, int _iNumberOfTimesToCallFunction = 1, unsigned long _ulDurationInBetweenCalls = 0, bool _bDurationEffectedByClockScale = true);

	// Removes a Scheduled Event from the list so it is not fired
	// Returns true if the Event was found, false if not
	bool RemoveScheduledEvent(int _iEventID);

	// Purges all Events from the list
	void RemoveAllScheduledEvents();

	// Updates all Events and Fires the Events which should be fired (should be called once per frame)
	void ProcessAllEvents();

	// Returns there are any Event Scheduled or not
	bool IsEmpty() const;

private:

	CScheduler();								// Constructor (Private because this is a Singleton class)
	static CScheduler* SmcpSchedulerInstance;	// Holds a pointer to the Class Instance

	static list<SEvent> SmcEventList;	// Holds a list of all the Events which are still scheduled to fire
	static int SmiEventIDCounter;		// Holds the next unique Event ID to be used
};

#endif
