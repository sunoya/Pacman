#ifndef _MYMAZE_H_
#define _MYMAZE_H_

#include "Maze.h"

class MyMaze: public Maze
{
public:
	MyMaze(GraphManager* gm)
		: Maze(gm)
	{
	}

	~MyMaze();

	virtual bool GetNextCoordinate(int nCurX, int nCurY, int &nNextX, int &nNextY);

	virtual bool LoadMaze(const std::string &sMazeFile);

private:
	int m_dist[MAZE_HEIGHT][MAZE_WIDTH];

	struct Coord
	{
		int x;
		int y;
	};

	void determineDistance(int sx, int sy);
};

#endif //  _MYMAZE_H_
