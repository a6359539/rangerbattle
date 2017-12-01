#pragma once

#include <iostream>
#include <iomanip>
#include <queue>
#include <string>
#include <math.h>
#include <ctime>

using namespace std;

const int n = 20;
const int m = 20;
static int map[n][m];
static int closed_nodes_map[n][m];
static int open_nodes_map[n][m];
static int dir_map[n][m];
const int dir = 4;

static int dx[dir] = { 1, 0, -1, 0 };
static int dy[dir] = { 0, 1, 0, -1 };

class node
{	
	int xPos;
	int yPos;
	int level;
	int priority;

public:
	node(int xp, int yp, int d, int p);

	int getxPos() const { return xPos; }
	int getyPos() const { return yPos; }
	int getLevel() const { return level; }
	int getPriority() const { return priority; }

	void updatePriority(const int & xDest, const int & yDest);
	void nextLevel(const int & i);
	const int & estimate(const int & xDest, const int & yDest) const;
};

bool operator<(const node & a, const node & b);
string pathFind(const int & xStart, const int & yStart, const int & xFinish, const int & yFinish);
void indextonode(int &x, int &y);
