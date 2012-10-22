#ifndef _ACTOR_H_
#define _ACTOR_H_

#include "testdefines.h"
#include "BGIgraphics.h"
#include "GraphManager.h"
class World;
class Monster;


enum monsterStatus {NORMAL, VULNERABLE, MONSTERDIE, RETURNTOHOME};

//-----------------------------------------ACTOR-----------------------------------------//
class Actor
{
public:
	//FUNCTIONS
	Actor(int id, int sx, int sy, colors sColor, World* ptr);
	virtual ~Actor();

	//Getter Functions
	int GetX() const;	//Return Actor's X coordinate
	int GetY() const;	//Return Actor's Y coordinate
	colors GetDisplayColor() const;	//Get Actor's color
	int getMyID() const;		//Get Actor's ID #
	World* getWorld() const;	//Return World Pointer

	//Setter Functions
	virtual void DoSomething() = 0;	//Movement function, must be redefined in subclasses
	void SetX(int newXCoord);		//Redefine X coordinate
	void SetY(int newYCoord);		//Redefine Y coordinate
	void setColor(colors newColor);	//Redefine Color

	virtual void completeReset() = 0;

private:
	int m_id;
	int m_x;
	int m_y;
	colors m_color;
	World* m_world;
};


//-----------------------------------------NACHMAN-----------------------------------------//
class NachMan: public Actor
{
public:
	NachMan(World* wPtr, int sx, int sy);
	~NachMan();

	//Getter Functions
	int GetNumLivesLeft() const;	//Get Number of Lives left
	int GetScore() const;			//Get Score
	bool isAlive() const;			//Return true if Nachman is alive

	//Setter Functions
	void DecrementNumLives();		//Decrease number of lives
	void setDead();					//Set Nachman's status to dead
	void setAlive();				//Set Nachman's status to alive
	void setDirection(Direction dir);	//Set Nachman's Direction
	void incrementScore(int aScore);
	virtual void DoSomething();		//Movement function

	virtual void completeReset();
private:
	int m_score;
	int m_lives;
	bool m_alive;
	Direction m_direction;

	bool canMoveInNextDir(Direction dir);	//Return true if NachMan can successufully go to next tile in specific direction, false otherwise.
	void setNextCoord(Direction dir);		//Sets next spot Nachman will move to in specified direction
	void NachManMeetMonster(Monster* mPtr);	//Checks if Monster is on same spot as NachMan and takes appropriate action
	void setMonsterVulnerable(Monster* mPtr);	//Sets Monsters to vulnerable if possible
};


//-----------------------------------------MONSTER-----------------------------------------//
class Monster: public Actor
{
public:
	Monster(int id, int sx, int sy, colors sColor, World* ptr, monsterStatus status);
	virtual ~Monster();
	virtual void DoSomething();

	void nextCoordToTarget(int tx, int ty, int& fx, int& fy);	//Algorithm used to move one tile closer to target tile
	void moveRandom();	//Algorithm to target random location on maze and move towards it
	
	virtual void moveNormal() = 0;	//Monster movement in Normal state.  All monsters have different algorithms
	
	virtual void moveVulnerable(colors clr);	//Monster movement in Vulnerable state.  Only Inky and Clyde have diff Algorithm
		//Changes monster back to Normal state if ticks = 0 or moves monster randomly if still vulnerable

	void moveMonsterDie();			//Monster movement in MonsterDie state. all monsters have same algorithm
		//Sets monster to ReturnToHome status

	virtual void moveReturnToHome(colors clr);	//Monster movement in ReturnToHome state.  Only Inky has diff algorithm
		//Brings monster closer to starting tile. If at starting tile, completely revert to Normal State and moveNormal(ly)

	void MonsterMeetNachMan(NachMan* nmPtr);
	
	virtual void completeReset();

	//SET FUNCTIONS
	void setStatus(monsterStatus status);	//Set the status of the monster
	void decrementTicks();		//Decrement vulerability ticks
	void resetTicks();			//Reset vulnerability ticks
	void setPastX(int px);		//Set x coordinate of last tile visited
	void setPastY(int py);		//Set y coordinate of last tile visited
	
	//GETTER FUNCTION
	monsterStatus GetStatus() const;	//Get status of monster
	int getTicks() const;		//Get remaining vulnerability ticks
	
private:
	monsterStatus m_status;
	int pastX;
	int pastY;
	int m_ticks;
	colors m_oColor;
};


//-----------------------------------------INKY-----------------------------------------//
class Inky: public Monster
{
public:
	Inky(World* ptr, int sx, int sy);
	~Inky();

	virtual void DoSomething();	
	
	virtual void moveNormal();		//Movement during NORMAL status for Inky
	virtual void moveVulnerable(colors clr);
	virtual void moveReturnToHome(colors clr);

	virtual void completeReset();
private:
	bool m_chase;
	int m_decTicks;			//Keeps track of number of ticks until next descision
		
	void chaseDecide();		//Decides whether or not to chase Nachman
};


//-----------------------------------------STINKY-----------------------------------------//
class Stinky: public Monster
{
public:
	Stinky(World* ptr, int sx, int sy);
	~Stinky();

	virtual void moveNormal();
};


//-----------------------------------------DINKY-----------------------------------------//
class Dinky: public Monster
{
public:
	Dinky(World* ptr, int sx, int sy);
	~Dinky();

	virtual void moveNormal();
};


//-----------------------------------------CLYDE-----------------------------------------//
class Clyde: public Monster
{
public:
	Clyde(World* ptr, int sx, int sy);
	~Clyde();

	virtual void moveNormal();
	virtual void moveVulnerable(colors clr);
};


#endif // #ifndef _ACTOR_H_
