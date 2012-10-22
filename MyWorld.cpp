#include "MyWorld.h"

MyWorld::~MyWorld()
{}

GameStatus MyWorld::RunLevel()
{
	// On entry to this function, the NachMan, Monster and
	// Maze objects have already been created. To get a pointer
	// to them, you can use the World class’s GetNachMan(),
	// GetMonster(), and GetMaze() methods.
	NachMan* nmPtr = GetNachMan();
	Maze* mPtr = GetMaze();
	Monster* iPtr = GetMonster(0);
	Monster* sPtr = GetMonster(1);
	Monster* dPtr = GetMonster(2);
	Monster* cPtr = GetMonster(3);
	
	// YOU MUST WRITE CODE TO DO EXACTLY THIS:
	// NachMan initialization:
	// 1. Set NachMan’s state to ALIVE and
	// 2. Set NachMan’s current location to the
	// specified NachMan-start-location in the Maze
	// 3. Reset the NachMan’s direction to none so he
	// doesn’t move until the player hits a key
	// Monster initialization:
	nmPtr->completeReset();
	
	// 1. Set all four monsters’ states to normal
	// 2. Set each monster’s start location in the maze to the monster home square in the Maze
	iPtr->completeReset();
	sPtr->completeReset();
	dPtr->completeReset();
	cPtr->completeReset();

	// Redisplay every cell on the screen before
	// game play begins (true means redraw every grid cell)
	DisplayScreen(true); // use this exact function call
	
	// Define your main game loop
	// Each iteration of the loop is one game tick
	while(nmPtr->isAlive() && mPtr->GetRemainingFood() > 0)
	{
		// Ask the NachMan object to move itself
		nmPtr->DoSomething();
	
		// Check if NachMan has died as a result of
		// moving itself. 
		if(nmPtr->isAlive())
		{
			//STILL NEED TO ADD MONSTER BEHAVIOR!!//
			//If NachMan has not died,
			// then ask each of the four monsters to move itself
			iPtr->DoSomething();
			sPtr->DoSomething();
			dPtr->DoSomething();
			cPtr->DoSomething();
			// Now update the graphical display, only redrawing
			// those sqares on the grid that changed due to
			// movement (That’s what the false means)
		}	
	
		DisplayScreen(false); // use this exact function call
	}
	
	if (mPtr->GetRemainingFood() == 0)
	{
		// you must return finishedlevel if NachMan
		// has eaten all of the pellets on this level
		// and therefore has completed the level
		return FINISHED_LEVEL;
	}
	if(nmPtr->isAlive() == false)// otherwise, NachMan died; you must return playerdied in this case.
		return PLAYER_DIED;
}
