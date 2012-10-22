#ifndef _MYWORLD_H_
#define _MYWORLD_H_

#include "World.h"

class MyWorld : public World
{
  public:
	virtual ~MyWorld();
	virtual GameStatus RunLevel();
};

#endif // #ifndef _MYWORLD_H_
