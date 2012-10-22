#include "MyMaze.h"
#include <stack>
using namespace std;

MyMaze::~MyMaze()
{}

bool MyMaze::GetNextCoordinate(int nCurX, int nCurY, int &nNextX, int &nNextY)
{
	if(nCurX == GetMonsterStartX() && nCurY == GetMonsterStartY())
		return false;

	if(nCurY != 1 && m_dist[nCurY][nCurX]-1 == m_dist[nCurY-1][nCurX])	//North
	{	
		nNextY = nCurY-1;
		return true;
	}
	
	if(nCurY != MAZE_HEIGHT-2 && m_dist[nCurY][nCurX]-1 == m_dist[nCurY+1][nCurX])	//South
	{	
		nNextY = nCurY+1;
		return true;
	}
	
	if(nCurX != MAZE_WIDTH-2 && m_dist[nCurY][nCurX]-1 == m_dist[nCurY][nCurX+1])	//East
	{	
		nNextX = nCurX+1;
		return true;
	}
	
	if(nCurX != 1 && m_dist[nCurY][nCurX]-1 == m_dist[nCurY][nCurX-1])
	{	
		nNextX = nCurX-1;
		return true;
	}
}

bool MyMaze::LoadMaze(const string &sMazeFile)
{
	bool b = Maze::LoadMaze(sMazeFile);
	if(b == false)
		return b;
	
	determineDistance(GetMonsterStartX(), GetMonsterStartY());
	return b;
}

void MyMaze::determineDistance(int sx, int sy)
{
	stack<Coord> cStack;

    //Set every element of the distance array to 99.
	for(int i = 0; i < MAZE_HEIGHT; i ++)
	{
		for(int j = 0; j < MAZE_WIDTH; j ++)
		{
			m_dist[i][j] = 99;
		}
	}

	//Set position (sr,sc) of the distance array to 0.
	m_dist[sy][sx] = 0;
    //Push the starting coordinate (sr,sc) onto the coordinate stack.
	Coord a;
	a.x = sx;
	a.y = sy;
	cStack.push(a);

	while(!cStack.empty())	//the stack is not empty
    {   
		//Pop the top coordinate off the stack. This gives you the current (r,c) location that your algorithm is exploring.
		Coord b = cStack.top();
		cStack.pop();

		//Let m be the minimum of the values of the four neighbors of (r,c)
        //in the distance array.  If the value at (r,c) in the distance
        //array is greater than m+1, then set it to m+1.
		int m = 99;
		
		if(b.x != 0 && m_dist[b.y][(b.x)-1] <= m)	//West
			m = m_dist[b.y][(b.x)-1];
		if(b.x != MAZE_WIDTH-1 && m_dist[b.y][(b.x)+1] <= m)	//East
			m = m_dist[b.y][(b.x)+1];
		if(b.y != 0 && m_dist[(b.y)-1][b.x] <= m)	//North
			m = m_dist[(b.y)-1][b.x];
		if(b.y != MAZE_HEIGHT-1 && m_dist[(b.y)+1][b.x] <= m)	//South
			m = m_dist[(b.y)+1][b.x];
		
		if(m_dist[b.y][b.x] > (m+1))
			m_dist[b.y][b.x] = m+1;
		
		//Let v be the value at (r,c) in the distance array.
		int v = m_dist[b.y][b.x];

        //Check where you can move from the current position:
                
		//If you can move NORTH and the value at position (r-1,c) in the
        //distance array is greater than v+1, then set it to v+1 and
        //push (r-1,c) onto the coordinate stack.
		if(b.y != 0 && GetGridContents(b.x, (b.y)-1) != WALL && (m_dist[(b.y)-1][b.x] > v+1))
		{
			m_dist[(b.y)-1][b.x] = v+1;
			Coord c;
			c.x = b.x;
			c.y = (b.y)-1;
			cStack.push(c);
		}
        //If you can move EAST and the value at position (r,c+1) in the
        //distance array is greater than v+1, then set it to v+1 and
        //push (r,c+1) onto the coordinate stack.
		if(b.x != MAZE_WIDTH-1 && GetGridContents((b.x)+1, b.y) != WALL && (m_dist[b.y][(b.x)+1] > v+1))
		{
			m_dist[b.y][(b.x)+1] = v+1;
			Coord c;
			c.x = (b.x)+1;
			c.y = b.y;
			cStack.push(c);
		}
		//If you can move SOUTH and the value at position (r+1,c) in the
		//distance array is greater than v+1, then set it to v+1 and
		//push (r+1,c) onto the coordinate stack.
		if(b.y != MAZE_HEIGHT-1 && GetGridContents(b.x, (b.y)+1) != WALL && (m_dist[(b.y)+1][b.x] > v+1))
		{
			m_dist[(b.y)+1][b.x] = v+1;
			Coord c;
			c.x = b.x;
			c.y = (b.y)+1;
			cStack.push(c);
		}
		//If you can move WEST and the value at position (r,c-1) in the
		//distance array is greater than v+1, then set it to v+1 and
		//push (r,c-1) onto the coordinate stack.
		if(b.x != 0 && GetGridContents((b.x)-1, b.y) != WALL && (m_dist[b.y][(b.x)-1] > v+1))
		{
			m_dist[b.y][(b.x)-1] = v+1;
			Coord c;
			c.x = (b.x)-1;
			c.y = b.y;
			cStack.push(c);
		}
	}
}