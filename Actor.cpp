#include "Actor.h"
#include "World.h"
#include <vector>
#include <stdlib.h>
using namespace std;

//-----------------------------------------ACTOR-----------------------------------------//
//Constructor
Actor::Actor(int id, int sx, int sy, colors sColor, World* ptr)
{
	m_id = id;
	m_x = sx;
	m_y = sy;
	m_color = sColor;
	m_world = ptr;
}

//Destructor
Actor::~Actor()
{}

//Getter Functions for ACTOR
int Actor::GetX() const
{
	return m_x;
}

int Actor::GetY() const
{
	return m_y;
}

colors Actor::GetDisplayColor() const
{
	return m_color;
}

int Actor::getMyID() const
{
	return m_id;
}

World* Actor::getWorld() const
{
	return m_world;
}

//Setter Functions for ACTOR
void Actor::SetX(int newXCoord)
{
	m_x = newXCoord;
}

void Actor::SetY(int newYCoord)
{
	m_y = newYCoord;
}

void Actor::setColor(colors newColor)
{
	m_color = newColor;
}


//-----------------------------------------NACHMAN-----------------------------------------//

NachMan::NachMan(World* wPtr, int sx, int sy)
	: Actor(ITEM_NACHMAN, sx, sy, YELLOW, wPtr)
{
	m_score = 0;
	m_lives = 3;
	m_alive = true;
	m_direction = NONE;
}

NachMan::~NachMan()
{}

//Getter Functions
int NachMan::GetNumLivesLeft() const
{
	return m_lives;
}

int NachMan::GetScore() const
{
	return m_score;
}

bool NachMan::isAlive() const 
{
	return m_alive;
}

//Setter Functions
void NachMan::DecrementNumLives()
{
	m_lives--;
}

void NachMan::setDead()
{
	m_alive = false;
}

void NachMan::setAlive()
{
	m_alive = true;
}

void NachMan::setDirection(Direction dir)
{
	m_direction = dir;
}

void NachMan::incrementScore(int aScore)
{
	m_score += aScore;
}

bool NachMan::canMoveInNextDir(Direction dir)
{
	GridContents sub;
	if(dir == NONE)		
		return false;
	
	if(dir == NORTH)
	{
		sub = getWorld()->GetMaze()->GetGridContents(GetX(), GetY()-1);
		if(sub == WALL || sub == CAGEDOOR)
			return false;
		else
			return true;
	}

	if(dir == EAST)
	{
		sub = getWorld()->GetMaze()->GetGridContents(GetX()+1, GetY());
		if(sub == WALL || sub == CAGEDOOR)
			return false;
		else
			return true;
	}

	if(dir == SOUTH)
	{
		sub = getWorld()->GetMaze()->GetGridContents(GetX(), GetY()+1);
		if(sub == WALL || sub == CAGEDOOR)
			return false;
		else
			return true;
	}

	if(dir == WEST)
	{
		sub = getWorld()->GetMaze()->GetGridContents(GetX()-1, GetY());
		if(sub == WALL || sub == CAGEDOOR)
			return false;
		else
			return true;
	}
}

void NachMan::setNextCoord(Direction dir)
{
	if(dir == NORTH)
		SetY(GetY()-1);
	if(dir == SOUTH)
		SetY(GetY()+1);
	if(dir == EAST)
		SetX(GetX()+1);
	if(dir == WEST)
		SetX(GetX()-1);
}

void NachMan::DoSomething()
{
	GridContents curTile = getWorld()->GetMaze()->GetGridContents(GetX(), GetY());
	
	//Get Monster pointers
	Monster* iPtr = getWorld()->GetMonster(0);
	Monster* sPtr = getWorld()->GetMonster(1);
	Monster* dPtr = getWorld()->GetMonster(2);
	Monster* cPtr = getWorld()->GetMonster(3);
	
	if(curTile == PELLET)	//Check for regular Pellet
	{
		m_score += 10;
		getWorld()->GetMaze()->SetGridContents(GetX(), GetY(), EMPTY);
		SoundFX::playNachManSound(PAC_SOUND_SMALL_EAT);
	}
	if(curTile == POWERPELLET)	//Check for Power Pellet
	{
		m_score += 100;
		getWorld()->GetMaze()->SetGridContents(GetX(), GetY(), EMPTY);
		SoundFX::playNachManSound(PAC_SOUND_BIG_EAT);
		
		//Sets all monsters to Vulnerable if possible
		setMonsterVulnerable(iPtr);
		setMonsterVulnerable(sPtr);
		setMonsterVulnerable(dPtr);
		setMonsterVulnerable(cPtr);
	}

	//Checks if NachMan is on same square as a Monster
	NachManMeetMonster(iPtr);
	NachManMeetMonster(sPtr);
	NachManMeetMonster(dPtr);
	NachManMeetMonster(cPtr);

	if(m_alive == false)	//If died, cease doing something
		return;
	
	if(getWorld()->GetMaze()->GetRemainingFood() != 0)
	{
		char ch;
		Direction dSub = NONE;
	
		if (getCharIfAny(ch))
		{
			switch (ch)
			{
			case ARROW_LEFT:
				dSub = WEST;
				break;
			case ARROW_RIGHT:
				dSub = EAST;
				break;
			case ARROW_UP:
				dSub = NORTH;
				break;
			case ARROW_DOWN:
				dSub = SOUTH;
				break;
			}
		}

		if(canMoveInNextDir(dSub))
		{
			m_direction = dSub;
			setNextCoord(dSub);
		}
		if(!canMoveInNextDir(dSub))
		{
			if(!canMoveInNextDir(m_direction))
				m_direction = NONE;
			if(canMoveInNextDir(m_direction))
				setNextCoord(m_direction);
		}
	}
}

void NachMan::completeReset()
{
	m_alive = true;
	m_direction = NONE;
	SetX(getWorld()->GetMaze()->GetNachManStartX());
	SetY(getWorld()->GetMaze()->GetNachManStartY());
}

void NachMan::NachManMeetMonster(Monster* mPtr)
{
	if(mPtr->GetX() == GetX() && mPtr->GetY() == GetY())
	{
		if(mPtr->GetStatus() == NORMAL)
		{	
			setDead();
		}
		if(mPtr->GetStatus() == VULNERABLE)
		{
			m_score += 1000;
			mPtr->setStatus(MONSTERDIE);
		}
	}
}

void NachMan::setMonsterVulnerable(Monster* mPtr)
{
	if(mPtr->GetStatus() == NORMAL || mPtr->GetStatus() == VULNERABLE)
	{	
		mPtr->setStatus(VULNERABLE);
		mPtr->setColor(LIGHTBLUE);
		mPtr->resetTicks();
	}
}


//-----------------------------------------MONSTER-----------------------------------------//

Monster::Monster(int id, int sx, int sy, colors sColor, World* ptr, monsterStatus status)
	:Actor(id, sx, sy, sColor, ptr)
{
	pastX = sx;
	pastY = sy;
	m_status = status;
	m_oColor = sColor;
	
	if(getWorld()->GetLevel() <= 8)
		m_ticks = 100 - ((getWorld()->GetLevel()) * 10);
	else
		m_ticks = 20;
}

Monster::~Monster()
{}

void Monster::DoSomething()
{
	int cx = GetX();
	int cy = GetY();

	MonsterMeetNachMan(getWorld()->GetNachMan());
	
	if(getWorld()->GetNachMan()->isAlive() == false)
		return;

	if(m_status == NORMAL)
	{
		moveNormal();
		pastX = cx;
		pastY = cy;
		return;
	}
	if(m_status == VULNERABLE)
	{
		moveVulnerable(m_oColor);
		pastX = cx;
		pastY = cy;
		return;
	}
	if(m_status == MONSTERDIE)
	{
		moveMonsterDie();
		return;
	}
	if(m_status == RETURNTOHOME)
	{
		moveReturnToHome(m_oColor);
		pastX = cx;
		pastY = cy;
		return;
	}
}

void Monster::nextCoordToTarget(int tx, int ty, int &fx, int &fy)
{
	int monX = GetX();
	int monY = GetY();

	Direction dir;

	if(tx > monX)
		dir = EAST;
	else if(tx < monX)
		dir = WEST;
	else
		dir = NONE;

	//Checks if can move horizontally
	if(dir == EAST)
	{
		if(monX != MAZE_WIDTH-2 && (getWorld()->GetMaze()->GetGridContents(monX+1, monY) != WALL) && (monX+1 != pastX))
		{
			fx = monX+1;
			return;
		}

	}
	if(dir == WEST)
	{
		if(monX != 1 && (getWorld()->GetMaze()->GetGridContents(monX-1, monY) != WALL) && (monX-1 != pastX))
		{
			fx = monX-1;
			return;
		}
	}
	//If able to exit from if statements, means can't move horizontally.

	if(ty > monY)
		dir = SOUTH;
	else if(ty < monY)
		dir = NORTH;
	else
		dir = NONE;


	//Checks vertical movement
	if(dir == NORTH)
	{
		if(monY != 1 && (getWorld()->GetMaze()->GetGridContents(monX, monY-1) != WALL) && (monY-1 != pastY))
		{
			fy = monY-1;
			return;
		}

	}
	if(dir == SOUTH)
	{
		if(monY != MAZE_HEIGHT && (getWorld()->GetMaze()->GetGridContents(monX, monY+1) != WALL) && (monY+1 != pastY))
		{
			fy = monY+1;
			return;
		}
	}
	

	//if able to exit, unable to move directly
	
	vector<Direction> vDir;
	vDir.push_back(NORTH);
	vDir.push_back(SOUTH);
	vDir.push_back(EAST);
	vDir.push_back(WEST);
	int i = 4;
	
	do
	{
		int j = rand() % i;
		dir = vDir[j];
		
		if(dir == NORTH)
		{
			if(monY != 1 && (getWorld()->GetMaze()->GetGridContents(monX, monY-1) != WALL) && (monY-1 != pastY))
			{
				fy = monY-1;
				return;
			}

		}
		if(dir == SOUTH)
		{
			if(monY != MAZE_HEIGHT-2 && (getWorld()->GetMaze()->GetGridContents(monX, monY+1) != WALL) && (monY+1 != pastY))
			{
				fy = monY+1;
				return;
			}

		}
		if(dir == EAST)
		{
			if(monX != MAZE_WIDTH-2 && (getWorld()->GetMaze()->GetGridContents(monX+1, monY) != WALL) && (monX+1 != pastX))
			{
				fx = monX+1;
				return;
			}

		}
		if(dir == WEST)
		{
			if(monX != 1 && (getWorld()->GetMaze()->GetGridContents(monX-1, monY) != WALL) && (monX-1 != pastX))
			{
				fx = monX-1;
				return;
			}

		}
		vDir.erase(vDir.begin() + j);
		i--;
	} while(!vDir.empty());

	//If manage to exit all of those ifs, then must move in reverse direction
	fx = pastX;
	fy = pastY;
}

void Monster::moveRandom()
{
	int ix = GetX();
	int iy = GetY();
	int tx = 0;
	int ty = 0;
		
	do
	{
		tx = rand() % MAZE_WIDTH;
		ty = rand() % MAZE_HEIGHT;
	} while(tx  == ix && ty == iy);

	nextCoordToTarget(tx, ty, ix, iy);
	SetX(ix);
	SetY(iy);
}

void Monster::moveVulnerable(colors clr)
{
	if(m_ticks == 0)
	{
		m_status = NORMAL;
		setColor(clr);
		resetTicks();
		moveNormal();
	}
	else
	{
		moveRandom();
		m_ticks --;
	}
}

void Monster::moveMonsterDie()
{
	SoundFX::playNachManSound(PAC_SOUND_BIG_EAT);
	m_status = RETURNTOHOME;
	setColor(LIGHTGRAY);
}

void Monster::moveReturnToHome(colors clr)
{
	int cx = GetX();
	int cy = GetY();
	int ix = GetX();
	int iy = GetY();
	bool b = getWorld()->GetMaze()->GetNextCoordinate(GetX(), GetY(), ix, iy);
	if(b)
	{
		SetX(ix);
		SetY(iy);
	}
	else
	{
		setStatus(NORMAL);
		setColor(clr);
		resetTicks();
		moveNormal();

	}
}

void Monster::setStatus(monsterStatus status)
{
	m_status = status;
}

void Monster::decrementTicks()
{
	m_ticks --;
}

void Monster::resetTicks()
{
	if(getWorld()->GetLevel() <= 8)
		m_ticks = 100 - ((getWorld()->GetLevel()) * 10);
	else
		m_ticks = 20;
}

void Monster::setPastX(int px)
{
	pastX = px;
}

void Monster::setPastY(int py)
{
	pastY = py;
}

monsterStatus Monster::GetStatus() const
{
	return m_status;
}

int Monster::getTicks() const
{
	return m_ticks;
}

void Monster::MonsterMeetNachMan(NachMan* nmPtr)
{
	if(GetX() == nmPtr->GetX() && GetY() == nmPtr->GetY())
	{
		if(m_status == NORMAL)
		{
			nmPtr->setDead();
		}
		if(m_status == VULNERABLE)
		{
			nmPtr->incrementScore(1000);
			m_status = MONSTERDIE;
		}
	}
}

void Monster::completeReset()
{
	m_status = NORMAL;
	setColor(m_oColor);
	resetTicks();
	SetX(getWorld()->GetMaze()->GetMonsterStartX());
	SetY(getWorld()->GetMaze()->GetMonsterStartY());
}


//-----------------------------------------INKY-----------------------------------------//
Inky::Inky(World* ptr, int sx, int sy)
	:Monster(ITEM_MONSTER1, sx, sy, LIGHTRED, ptr, NORMAL)  
{
	setPastX(0);
	setPastY(0);
	m_decTicks = 10;
	chaseDecide();
}

Inky::~Inky()
{}

void Inky::chaseDecide()
{
	int b = rand() % 100;

	if(b < 80)
		m_chase = true;
	else
		m_chase = false;
}

void Inky::moveNormal()
{
	int ix = GetX();
	int iy = GetY();

	if(m_chase)
	{
		int nx = getWorld()->GetNachMan()->GetX();
		int ny = getWorld()->GetNachMan()->GetY();

		nextCoordToTarget(nx, ny, ix, iy);
		SetX(ix);
		SetY(iy);
	}
	else
	{
		moveRandom();
	}
}

void Inky::moveVulnerable(colors clr)
{
	if(getTicks() == 0)
	{
		setStatus(NORMAL);
		setColor(clr);
		resetTicks();
		m_decTicks = 10;
		chaseDecide();
		moveNormal();
	}
	else
	{
		moveRandom();
		decrementTicks();
	}
}

void Inky::moveReturnToHome(colors clr)
{
	int ix = GetX();
	int iy = GetY();
	bool b = getWorld()->GetMaze()->GetNextCoordinate(GetX(), GetY(), ix, iy);
	if(b)
	{
		SetX(ix);
		SetY(iy);
	}
	else
	{
		setStatus(NORMAL);
		setColor(clr);
		m_decTicks = 10;
		chaseDecide();
		moveNormal();
		resetTicks();
	}
}

void Inky::DoSomething()
{
	int cx = GetX();
	int cy = GetY();
	
	MonsterMeetNachMan(getWorld()->GetNachMan());
	if(getWorld()->GetNachMan()->isAlive() == false)
		return;
	
	if(GetStatus() == NORMAL)
	{
		if(m_decTicks == 0)
		{
			m_decTicks = 10;
			chaseDecide();
			moveNormal();
			setPastX(cx);
			setPastY(cy);
			return;
		}
		else
		{
			moveNormal();
			setPastX(cx);
			setPastY(cy);
			m_decTicks--;
			return;
		}
	}
	if(GetStatus() == VULNERABLE)
	{
		moveVulnerable(LIGHTRED);
		setPastX(cx);
		setPastY(cy);
		return;
	}
	if(GetStatus() == MONSTERDIE)
	{
		moveMonsterDie();
		return;
	}
	if(GetStatus() == RETURNTOHOME)
	{
		moveReturnToHome(LIGHTRED);
		setPastX(cx);
		setPastY(cy);
		return;
	}
}

void Inky::completeReset()
{
	Monster::completeReset();
	m_decTicks = 10;
	chaseDecide();
}

//-----------------------------------------STINKY-----------------------------------------//
Stinky::Stinky(World* ptr, int sx, int sy)
	:Monster(ITEM_MONSTER2, sx, sy, LIGHTGREEN, ptr, NORMAL)  
{
	setPastX(0);
	setPastY(0);
}

Stinky::~Stinky()
{}

void Stinky::moveNormal()
{
	int mx = GetX();
	int my = GetY();
	int lx = mx;
	int rx = mx;
	int uy = my;
	int dy = my;
	
	int tx = 0;
	int ty = 0;

	Maze* mPtr = getWorld()->GetMaze();
	NachMan* nmPtr = getWorld()->GetNachMan();

	while(lx != 1 && lx != (mx-5))	//Checks for leftmost "sniffing" area
		lx--;
	while(rx != MAZE_WIDTH-2 && rx != (mx+5))	//Checks for right "sniffing" area
		rx++;
	while(uy != 1 && uy != (my-5))	//Checks for upmost "sniffing" area
		uy--;
	while(dy != MAZE_HEIGHT-2 && dy != (my+5))	//Checks for downmost "sniffing" area
		dy++;

	for(int i = uy; i <= dy; i++)	//Find Nachman in area bounded by above while loops
	{
		for(int j = lx; j <= rx; j++)
		{
			if(j == nmPtr->GetX() && i == nmPtr->GetY())
			{
				tx = j;
				ty = i;
			}
		}
	}
	if(tx == 0 && ty == 0) //Not find NachMan
	{
		moveRandom();
	}
	else
	{
		nextCoordToTarget(tx, ty, mx, my);
		SetX(mx);
		SetY(my);
	}

}


//-----------------------------------------DINKY-----------------------------------------//
Dinky::Dinky(World* ptr, int sx, int sy)
	:Monster(ITEM_MONSTER3, sx, sy, LIGHTMAGENTA, ptr, NORMAL)  
{
	setPastX(0);
	setPastY(0);
}

Dinky::~Dinky()
{}

void Dinky::moveNormal()
{
	int mx = GetX();
	int my = GetY();
	int lx = mx;
	int rx = mx;
	int uy = my;
	int dy = my;
	
	int tx = 0;
	int ty = 0;

	Maze* mPtr = getWorld()->GetMaze();
	NachMan* nmPtr = getWorld()->GetNachMan();

	while(lx != 1 && mPtr->GetGridContents(lx, my) != WALL)
	{
		if(lx == nmPtr->GetX() && my == nmPtr->GetY())
		{
			tx = lx;
			ty = my;
			nextCoordToTarget(tx, ty, mx, my);
			SetX(mx);
			SetY(my);
			return;
		}
		lx--;
	}
	while(rx != MAZE_WIDTH-2 && mPtr->GetGridContents(rx, my) != WALL)
	{
		if(rx == nmPtr->GetX() && my == nmPtr->GetY())
		{
			tx = rx;
			ty = my;
			nextCoordToTarget(tx, ty, mx, my);
			SetX(mx);
			SetY(my);
			return;
		}
		rx++;
	}
	while(uy != 1 && mPtr->GetGridContents(mx, uy) != WALL)
	{
		if(mx == nmPtr->GetX() && uy == nmPtr->GetY())
		{
			tx = mx;
			ty = uy;
			nextCoordToTarget(tx, ty, mx, my);
			SetX(mx);
			SetY(my);
			return;
		}
		uy--;
	}
	while(dy != MAZE_HEIGHT-2 && mPtr->GetGridContents(mx, dy) != WALL)
	{
		if(mx == nmPtr->GetX() && dy == nmPtr->GetY())
		{
			tx = mx;
			ty = dy;
			nextCoordToTarget(tx, ty, mx, my);
			SetX(mx);
			SetY(my);
			return;
		}
		dy++;
	}

	if(tx == 0 && ty == 0)
		moveRandom();
}

//-----------------------------------------CLYDE-----------------------------------------//
Clyde::Clyde(World* ptr, int sx, int sy)
	:Monster(ITEM_MONSTER4, sx, sy, LIGHTCYAN, ptr, NORMAL)  
{
	setPastX(sx);
	setPastY(sy);
}

Clyde::~Clyde()
{}

void Clyde::moveNormal()
{
	int cx = GetX();
	int cy = GetY();
	
	MonsterMeetNachMan(getWorld()->GetNachMan());
	if(getWorld()->GetNachMan()->isAlive() == false)
		return;

	moveRandom();
	setPastX(cx);
	setPastY(cy);
}

void Clyde::moveVulnerable(colors clr)
{

	if(getTicks() == 0)
	{
		setStatus(NORMAL);
		setColor(clr);
		resetTicks();
		moveNormal();
	}
	else
	{
		int ix = GetX();
		int iy = GetY();

		NachMan* nmPtr = getWorld()->GetNachMan();
		int nx = nmPtr->GetX();
		int ny = nmPtr->GetY();
		
		if(nx <= 10)
		{
			if(ny <= 7)
			{
				nextCoordToTarget(MAZE_WIDTH-1, MAZE_HEIGHT-2, ix, iy);
				SetX(ix);
				SetY(iy);
				decrementTicks();
				return;
			}
			if(ny >= 8)
			{
				nextCoordToTarget(MAZE_WIDTH-1, 0, ix, iy);
				SetX(ix);
				SetY(iy);
				decrementTicks();
				return;
			}
		}
		if(nx >= 11)
		{
			if(ny <= 7)
			{
				nextCoordToTarget(0, MAZE_HEIGHT-2, ix, iy);
				SetX(ix);
				SetY(iy);
				decrementTicks();
				return;
			}
			if(ny >= 8)
			{
				nextCoordToTarget(0, 0, ix, iy);
				SetX(ix);
				SetY(iy);
				decrementTicks();
				return;
			}
		}
	}
}