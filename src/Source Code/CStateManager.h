/*****************************************************************************/
/* CStateManager.h (and CStateManager.cpp)                                   */
/* Written by Daniel Schroeder                                               */
/* Created on July 15 2005                                                   */
/* Last Updated September 5, 2007                                            */
/*---------------------------------------------------------------------------*/
/*   The CStateManager class is used to keep track of Game States, or any    */
/* other States. You can set a function as the current State by calling the  */
/* SetState() function and the function you want to calls name. In order to	 */
/* set a function as a State however, the function must follow a specified	 */
/* format (see below). Then when Process() is called, it calls the set       */
/* States function. If the State is being processed for the first time, it   */
/* calls the function with a Purpose of eInitialization. If the State is     */
/* being shutdown it calls the function with a Purpose of eShutdown. If the  */
/* State was previously initialized, but another State was Set, and now the  */
/* first State is being Set again, the function will be called with a Purpose*/
/* of eReturning. Otherwise, the funciton will be called with a Purpose of   */
/* eNormal.                                                                  */
/*                                                                           */
/* State functions must have the following format:                           */
/*   static void FunctionName(void* vpOptionalData, EPurpose ePurpose);      */
/*                                                                           */
/* Examples of State functions:                                              */
/*   static void GameStateStartMenu(void* vpOptionalDataToPass,              */
/*                                  EPurpose ePurpose);                      */
/*   static void GameStatePlay(void*, EPurpose);                             */
/*                                                                           */
/* Example of Initializing and Setting a State:                              */
/*   CStateManager cStateManager;                                            */
/*   cStateManager.SetState(GameStateStartMenu, vpOptionalDataToPass);       */
/*																			 */
/* Then just call cStateManager.Process() each game loop, and if it returns	 */
/* false then exit the game loop. First the GameStateStartMenu() function    */
/* would be called. Then from within the GameStateStartMenu() you could call */
/* for example, cStateManager.SetState(GameStatePlay); when the player		 */
/* selects to play the game, and on the next game loop the GameStatePlay()	 */
/* function will be called.													 */
/*****************************************************************************/

#ifndef CSTATE_MANAGER_H
#define CSTATE_MANAGER_H


// Types of purposes for calling a function
enum EPurpose
{
	eInitialize,	// Used if GameStateFunction is being called for the first time
	eNormal,		// Used as the Normal GameStateFunction call
	eShutdown,		// Used if GameStateFunction is being called for the last time
	eReturning		// Used if GameStateFunction was running, then we left it, and now coming back to it
};

// Holds States Function and pointer to Next State
struct SGameState
{
	void (*Function)(void* vpFunction, EPurpose ePurpose);
	void* vpOptionalData;
	SGameState* spNext;
};

// Manages the Game States
class CStateManager
{
	public:
		
		CStateManager();		// Constructor
		~CStateManager();		// Destructor

		// Set the State Function to call
		// Returns true if State was set ok, false if not
		bool SetState(void (*Function)(void* vpFunction, EPurpose ePurpose), void* vpOptionalDataToPass = 0);

		// Set the Next State Function to call, but Shutdown the current one first
		// Returns true if State was set ok, false if not
		// NOTE: Do not use to Set the first State Function (since there will not yet be a Current State to Shutdown)
		bool ShutdownCurrentStateAndSetNextState(void (*Function)(void* vpFunction, EPurpose ePurpose), void* vpOptionalDataToPass = 0);

		// Returns to the State which called the Current State
		// If bShutDownCurrentState == true, the Current State is shutdown before returning to the Previous State
		// Returns true if Previous State was returned to ok, false if not
		bool ReturnToPreviousState(bool bShutDownCurrentState, void* vpOptionalDataToPass = 0);

		// Call the State Function
		// Returns false when no State Function to call
		bool Process();

		// Returns the Current State Function the State Manager is processing
		// Returns 0 if no Current State Function is being processed
		void* ReturnCurrentStateFunction() const;

		// Returns the Previous State Function, which would be the State Function that called the Current State Function
		// Returns 0 if only 1 or no State has been Set yet (Since at least 2 States need to be called for this to exist)
		void* ReturnPreviousStateFunction() const;

		// Shutsdown all State Functions. Once done, Process() will return false when called
		void ShutdownAllStates(void* vpOptionalDataToPass = 0);

	private:

		// Returns the State if it is in the list
		// Returns 0 if State not in list
		SGameState* FunctionIsInList(void (*Function)(void* vpFunction, EPurpose ePurpose)) const;

		// Removes a State from the List
		// Returns true if State was in List and Removed, False if not
		bool RemoveStateFromList(SGameState* spStateToRemove);

		// Moves a State to the front of the List
		bool MoveStateToFrontOfList(SGameState* spStateToMove);

		SGameState* mspCurrentStateToProcess;	// Current State Function to call
		SGameState* mspNextStateToProcess;		// Next State Function to call
		SGameState* mspHead;					// Points to Head of State List

		EPurpose	mePurpose;					// Holds what purpose the Current State should have
		EPurpose	meNextStatePurpose;			// Holds what purpose the Next State should have
};

#endif
