#include <iostream> // for I/O
#include <limits>   // to set int to max value
#include <stdlib.h> // for rand and srand
#include <time.h>   // for time
#include <vector>   // for vectors
#include <windows.h>

//#include <boost/archive/text_oarchive.hpp> // to be used for node class serialization
//#include <boost/archive/text_iarchive.hpp> // tutorial: <http://www.boost.org/doc/libs/1_36_0/libs/serialization/doc/index.html>

// TO DO: add time stamps and change to lua

using namespace std;

#define XSIZE 5
#define YSIZE 5
#define ZSIZE 5

int hfactor = 1; // higher the factor, faster the solution but not guaranteed to be the shortest solution

static int dircostmap[4][4] = { {1, 2, 3, 2}, {2, 1, 2, 3}, {3, 2, 1, 2}, {2, 3, 2, 1} }; // maps move costs with directions

struct coordinate { // coordinates structure: holds x, y, z coordinates and direction
	int x, y, z;
	int d = -1; // -1 by default to show unassigned direction
};

class node {                // block map node (meant to be created as an array)
	int id;                 // block ID
	int x, y, z;            // x, y, and z coords and facing of node
	int g, h;               // g & h costs
	int xp, yp, zp, d;      // parent coordinates and direction of parent from current node
	int status;             // unvisited/open/closed status (0/1/2 respectively)
	bool changed;           // specifies if the node id has been changed
	unsigned int timeStamp; // time stamp of when node was last IDed
	public:
		node() {
			id      = 0;
			x       = 0;
			y       = 0;
			z       = 0;
			g       = 0; // g cost (cost needed to get to current node from start)
			h       = 0; // h cost (projected cost to get from current node to end)
			xp      = 0;
			yp      = 0;
			zp      = 0;
			d       = 0;
			status  = 0;
			changed = false;
		}
		int abs(int n) { return (n < 0) ? -1*n : n; }
		int getid() { return id; }
		int getx()  { return x; }
		int gety()  { return y; }
		int getz()  { return z; }
		int getf()  { return g + h; }
		int getg()  { return g; }
		int geth()  { return h; }
		int getxp() { return xp; }
		int getyp() { return yp; }
		int getzp() { return zp; }
		coordinate getxyzp() { coordinate p; p.x = xp; p.y = yp; p.z = zp; return p; }
		int getd() { return d; }
		int getstatus() { return status; }
		bool idchanged() { return changed; }
		unsigned int gettime() { return timeStamp; }
		void setid(int i) {
			changed = (id != i) ? true : false;
			id = i;
			timeStamp = time(NULL);
		}
		void setx(int xi) { x = xi; }
		void sety(int yi) { y = yi; }
		void setz(int zi) { z = zi; }
		void setd(int di) { d = di; }
		void setg(int gi) { g = gi; }
		void seth(int hi) { h = hi; }
		void setxyz(int xi, int yi, int zi) { x = xi; y = yi; z = zi; }
		void setxyz(coordinate c) { x = c.x; y = c.y; z = c.z; }
		void setxp(int xi) { xp = xi; }
		void setyp(int yi) { yp = yi; }
		void setzp(int zi) { zp = zi; }
		void setxyzp(node parent) {
			xp = parent.getx();
			yp = parent.gety();
			zp = parent.getz();

			if (z == zp) {
				d = ((xp - x) > 0) + 2*((yp - y) < 0) + 3*((xp - x) < 0);// + 4*((zp - z) > 0) + 5*((zp - z) < 0); // set direction to point at parent
			}
			else {
				d = parent.getd();
			}
		}
		void setstatus(int s) { status = s; }
		void setchanged(bool c) { changed = c; }
		void calcg(node parent, int finalD = -1) {
			g = parent.getg(); // previous movement cost
			int dir = ((parent.getx() - x) > 0) + 2*((parent.gety() - y) < 0) + 3*((parent.getx() - x) < 0); // direction to the parent currently being calculated

			if (z == parent.getz()) {
				g += dircostmap[parent.getd()][dir]; // x-y direction cost (uses direction faced in previous node and current direction facing now)
			}
			else {
				g += 1; // z direction cost
			}

			if (finalD >= 0) {
				if (z == parent.getz()) {
					g += dircostmap[dir][(finalD + 2)%4] - 1; // extra movement cost for adjusting direction on final node
				}
				else {
					g += dircostmap[parent.getd()][(finalD + 2)%4] - 1; // for z-movement, node inherits parent's direction
				}
			}
		}
		void calch(int xf, int yf, int zf) {
			int dirCost = 0;

			if ((d%2 == 0)&&(xf - x != 0) || (d%2 == 1)&&(yf - y != 0)) { // set cost to 1 if destination is to the side of current facing
					dirCost = 1;
			}
			if ((d == 2)&&(yf < y) || (d == 3)&&(xf < x) || (d == 0)&&(yf > y) || (d == 1)&&(xf > x)) { // set cost to 2 if destination is behind current position according to facing
					dirCost = 2;
			}

			h = abs(xf - x) + abs(yf - y) + abs(zf - z) + dirCost; // cost if all nodes travelled to finish from current node were walkable
			h *= hfactor;
		}
};

// Set XYZ coordinates for each node in the map
void initializeNodeCoordinates(node n[XSIZE][YSIZE][ZSIZE]) {
	for (int i = 0; i < XSIZE; i++) {
		for (int j = 0; j < YSIZE; j++) {
			for (int k = 0; k < ZSIZE; k++) {
				n[i][j][k].setxyz(i, j, k);
			}
		}
	}
}

// Print detailed grid with specific values of each node
void printValues(node n[XSIZE][YSIZE][ZSIZE]) {
	for (int i = 0; i < ZSIZE; i++) {
		cout << endl << " y" << endl << " ^" << endl;
		for (int j = YSIZE - 1; j >= 0; j--) { // y-coords
			cout << " |\t";
			for (int k = 0; k < XSIZE; k++) { // x-coords
				cout << n[k][j][i].getx() << "," << n[k][j][i].gety() << "," << n[k][j][i].getid() << "\t"; // node coords & id
			}
			cout << endl << " |\t";
			for (int k = 0; k < XSIZE; k++) {
				cout << n[k][j][i].getxp() << "," << n[k][j][i].getyp() << "," << n[k][j][i].getd() << "\t"; // parent coords & direction
			}
			cout << endl << " |\t";
			for (int k = 0; k < XSIZE; k++) {
				cout << n[k][j][i].getg() << "," << n[k][j][i].geth() << "," << n[k][j][i].getstatus() << "\t"; // g and h values & status
			}
			cout << endl << " |" << endl;
		}
		cout << " ";
		for (int j = 0; j <= XSIZE; j++) {
			cout << "--------";
		}
		cout << ">x" << endl;
		}
}

// Print map to show obstacles
void printMap(node n[XSIZE][YSIZE][ZSIZE], int curx = -1, int cury = -1, int curz = -1, int curd = -1) {
	int dir;

	for (int i = YSIZE - 1; i >= 0; i--) {
		for (int j = 0; j < ZSIZE; j++) {
			cout << "\t";
			for (int k = 0; k < XSIZE; k++) {
				if (k == curx && i == cury && j == curz) {
					if (curd == -1) {
						dir = n[k][i][j].getd();
					}
					else {
						dir = (curd + 2)%4;
					}
					switch (dir) {
						case 2:
							cout << "^";
							break;
						case 3:
							cout << ">";
							break;
						case 0:
							cout << "V";
							break;
						case 1:
							cout << "<";
							break;
						default:
							cout << ".";
					}
				}
				else {
					switch (n[k][i][j].getid()) {
						case 0:
							cout << (char)176;
							break;
						default:
							cout << (char)219;
					}
				}
			}
		}
		cout << endl;
	}
	cout << endl;
}

// Print elements of vector in horzontal list
void printPath(vector<int> path) {
	for (int i = 0; i < path.size(); i++) {
		cout << path[i] << " ";
	}
	cout << endl;
}

// Randomly block nodes in map
void generateRandomMaze(node n[XSIZE][YSIZE][ZSIZE]) {
	int percent = 30;

	for (int i = 0; i < ZSIZE; i++) {
		for (int j = 0; j < YSIZE; j++) {
			for (int k = 0; k < XSIZE; k++) {
				n[k][j][i].setid((int)(rand()%100 < percent));
			}
		}
	}
}

// Find the lowest F score in the valid nodes of the map
coordinate lowestF(node n[XSIZE][YSIZE][ZSIZE]) {
	int lowf = numeric_limits<int>::max();
	coordinate coord;
	coord.x = -1; // x and y negatives if failed to find anything in open list
	coord.y = -1;
	coord.z = -1;

	for (int i = 0; i < ZSIZE; i++) {
		for (int j = 0; j < YSIZE; j++) {
			for (int k = 0; k < XSIZE; k++) {
				if (n[k][j][i].getid() == 0 && n[k][j][i].getstatus() == 1 && n[k][j][i].getf() < lowf) { // if walkable and on open list and lower than lowest F encountered
					lowf = n[k][j][i].getf();
					coord.x = k;
					coord.y = j;
					coord.z = i;
				}
			}
		}
	}

	return coord;
}

// Looks one block ahead, one block above, and one block below to see what's in those other nodes
bool sense(node t[XSIZE][YSIZE][ZSIZE], node m[XSIZE][YSIZE][ZSIZE], coordinate cur) {
	bool changed = false;

	// get id of block in front of turtle
	if (cur.d == 0 && cur.y + 1 < YSIZE) {
		t[cur.x][cur.y + 1][cur.z].setid(m[cur.x][cur.y + 1][cur.z].getid());
		if (t[cur.x][cur.y + 1][cur.z].idchanged()) { changed = true; }
	}
	if (cur.d == 1 && cur.x + 1 < XSIZE) {
		t[cur.x + 1][cur.y][cur.z].setid(m[cur.x + 1][cur.y][cur.z].getid());
		if (t[cur.x + 1][cur.y][cur.z].idchanged()) { changed = true; }
	}
	if (cur.d == 2 && cur.y > 0) {
		t[cur.x][cur.y - 1][cur.z].setid(m[cur.x][cur.y - 1][cur.z].getid());
		if (t[cur.x][cur.y - 1][cur.z].idchanged()) { changed = true; }
	}
	if (cur.d == 3 && cur.x > 0) {
		t[cur.x - 1][cur.y][cur.z].setid(m[cur.x - 1][cur.y][cur.z].getid());
		if (t[cur.x - 1][cur.y][cur.z].idchanged()) { changed = true; }
	}

	// get id of blocks above and below turtle
	if (cur.z + 1 < ZSIZE) {
		t[cur.x][cur.y][cur.z + 1].setid(m[cur.x][cur.y][cur.z + 1].getid());
		if (t[cur.x][cur.y][cur.z + 1].idchanged()) { changed = true; }
	}
	if (cur.z > 0) {
		t[cur.x][cur.y][cur.z - 1].setid(m[cur.x][cur.y][cur.z - 1].getid());
		if (t[cur.x][cur.y][cur.z - 1].idchanged()) { changed = true; }
	}

	if (changed) { Sleep(1000); }
	//printMap(t, curx, cury, curz, curd);

	return changed;
}

// Rotate to face desired direction
void faceD(int &curd, int nexd) {
	switch (nexd) {
		case 0:
		case 1:
		case 2:
		case 3:
			curd = nexd;
	}
}

// Rotate and move forward in desired direction
bool moveSenseD(node t[XSIZE][YSIZE][ZSIZE], node m[XSIZE][YSIZE][ZSIZE], coordinate &cur, int nexd) {
	faceD(cur.d, nexd);
	bool changed = sense(t, m, cur);
	if (changed) { return true; }

	// if no change, move in next movement direction
	if (nexd == 0 && cur.y + 1 < YSIZE) {
		if (t[cur.x][cur.y + 1][cur.z].getid() == 0) {
			cur.y++;
		}
	}
	if (nexd == 1 && cur.x + 1 < XSIZE) {
		if (t[cur.x + 1][cur.y][cur.z].getid() == 0) {
			cur.x++;
		}
	}
	if (nexd == 2 && cur.y > 0) {
		if (t[cur.x][cur.y - 1][cur.z].getid() == 0) {
			cur.y--;
		}
	}
	if (nexd == 3 && cur.x > 0) {
		if (t[cur.x - 1][cur.y][cur.z].getid() == 0) {
			cur.x--;
		}
	}
	if (nexd == 4 && cur.z + 1 < ZSIZE) {
		if (t[cur.x][cur.y][cur.z + 1].getid() == 0) {
			cur.z++;
		}
	}
	if (nexd == 5 && cur.z > 0) {
		if (t[cur.x][cur.y][cur.z - 1].getid() == 0) {
			cur.z--;
		}
	}
	return false;
}

// Set G score, H score, and status to 0 for all nodes in map
void resetScoresAndStatus(node n[XSIZE][YSIZE][ZSIZE]) {
	for (int i = 0; i < ZSIZE; i++) {
		for (int j = 0; j < YSIZE; j++) {
			for (int k = 0; k < XSIZE; k++) {
				n[k][j][i].setg(0);
				n[k][j][i].seth(0);
				n[k][j][i].setstatus(0);
			}
		}
	}
}

// Set scores and statuses of adjacent node while running A* algorithm
void setAdjacentNodeScores(node n[XSIZE][YSIZE][ZSIZE], int x0, int y0, int z0, int x1, int y1, int z1, coordinate stop, bool boundaryCondition) {
	int oldg;
	bool nodeIsFinalNode = (x1 == stop.x) && (y1 == stop.y) && (z1 == stop.z);

	if (boundaryCondition) {                                                           // if there's a node in the boundary
		if (n[x1][y1][z1].getid() == 0) {                                              // if the node is walkable
			switch (n[x1][y1][z1].getstatus()) {
				case 1:                                                                // if the node is in the open list
					oldg = n[x1][y1][z1].getg();                                       // save the old G score for comparison
					n[x1][y1][z1].calcg(n[x0][y0][z0], nodeIsFinalNode ? stop.d : -1); // recalculate a new G score (accounting for final direction if adjacent node is the end)
					if (n[x1][y1][z1].getg() >= oldg) {                                // if the new G score is not less than the old
						n[x1][y1][z1].setg(oldg);                                      // set the score back to the old
					}
					else {
						n[x1][y1][z1].setxyzp(n[x0][y0][z0]);                          // else set the node's parent to current node
						n[x1][y1][z1].calch(stop.x, stop.y, stop.z);                   // recalculate the H score
					}
					break;
				case 0:                                                                // if the node is not in a list
					n[x1][y1][z1].setstatus(1);                                        // place in open list
					n[x1][y1][z1].setxyzp(n[x0][y0][z0]);                              // set the node's parent to current node
					n[x1][y1][z1].calcg(n[x0][y0][z0], nodeIsFinalNode ? stop.d : -1); // calculate the G score
					n[x1][y1][z1].calch(stop.x, stop.y, stop.z);                       // calculate the H score
					break;
			}
		}
	}
}

// Reverses a vector
vector<int> reverseVector(vector<int> v) {
	int temp, index = v.size() - 1;

	for (int i = 0; i < v.size()/2; i++) {
		temp = v[i];
		v[i] = v[index];
		v[index] = temp;

		index--;
	}

	return v;
}

// Outlines the best path by following the parents of each node
vector<int> bestPath(node n[XSIZE][YSIZE][ZSIZE], coordinate start, coordinate stop) {
	vector<int> path;
	coordinate cur = stop;
	int zdiff; // difference between node z and parent z

	while (cur.x != start.x || cur.y != start.y || cur.z != start.z) { // while not at starting node
		zdiff = n[cur.x][cur.y][cur.z].getz() - n[cur.x][cur.y][cur.z].getzp(); // -1 if parent above, 0 if no change, 1 if parent below
		switch (zdiff) {
			case 1:                                                 // if parent below
				path.push_back(4);                                  // set movement direction needed to get to current node to go up
				break;
			case -1:                                                // if parent above
				path.push_back(5);                                  // set movement direction needed to get to current node to go down
				break;
			default:                                                // if on same z-coord as parent
				path.push_back((n[cur.x][cur.y][cur.z].getd() + 2)%4); // set movement needed to get to current node from parent (reverse of direction to parent)
		}
		cur = n[cur.x][cur.y][cur.z].getxyzp(); // go to parent cell next
	}

	return reverseVector(path);
}

// Run A* maze-solving algorithm to find the least costly path from start to stop
vector<int> aStarPath(node n[XSIZE][YSIZE][ZSIZE], coordinate start, coordinate stop) {
	coordinate cur = start;

	resetScoresAndStatus(n);

	n[start.x][start.y][start.z].setd((start.d + 2)%4);

	//printMap(n, -1, -1, -1);

	n[cur.x][cur.y][cur.z].setstatus(1); // open starting node
	cur = lowestF(n); // set the current x, y coords to the node with the lowest F score
	//n[curx][cury][curz].setstatus(2); // close starting node to prevent any parent change

	while (n[stop.x][stop.y][stop.z].getstatus() != 2 && cur.x >= 0 && cur.y >= 0 && cur.z >= 0) { // while final node is not closed and open list is not empty

		//printMap(n, cur.x, cur.y, cur.z, cur.d);

		setAdjacentNodeScores(n, cur.x, cur.y, cur.z, cur.x + 1, cur.y, cur.z, stop, cur.x + 1 < XSIZE);
		setAdjacentNodeScores(n, cur.x, cur.y, cur.z, cur.x - 1, cur.y, cur.z, stop, cur.x > 0);
		setAdjacentNodeScores(n, cur.x, cur.y, cur.z, cur.x, cur.y + 1, cur.z, stop, cur.y + 1 < YSIZE);
		setAdjacentNodeScores(n, cur.x, cur.y, cur.z, cur.x, cur.y - 1, cur.z, stop, cur.y > 0);
		setAdjacentNodeScores(n, cur.x, cur.y, cur.z, cur.x, cur.y, cur.z + 1, stop, cur.z + 1 < ZSIZE);
		setAdjacentNodeScores(n, cur.x, cur.y, cur.z, cur.x, cur.y, cur.z - 1, stop, cur.z > 0);

		n[cur.x][cur.y][cur.z].setstatus(2); // close current node
		cur = lowestF(n); // set the current x, y coords to the node with the lowest F score
	}

	if (n[stop.x][stop.y][stop.z].getg() == 0) { // if no cost required to get to end, this means either start and/or finish are unwalkable or start = finish
		vector<int> blank;
		return blank;
	}

	return bestPath(n, start, stop);
}

// Traverses map, sensing and adding to nodes as it moves
void moveTo(node t[XSIZE][YSIZE][ZSIZE], node m[XSIZE][YSIZE][ZSIZE], coordinate start, coordinate stop) {
	coordinate cur = start;
	int pathCount = 0;
	vector<int> path;
	bool changed = false;

	path = aStarPath(t, start, stop);
	//printMap(t, curx, cury, curz, curd);
	while (path.size() != pathCount) {
		//turn and/or move
		changed = moveSenseD(t, m, cur, path[pathCount]);
		printMap(t, cur.x, cur.y, cur.z, cur.d);
		//printPath(path);

		//sense
		if (!changed) {
			pathCount++;
			//changed = sense(t, m, cur);
		}

		//if map changed, reevaluate path
		if (changed) {
			path = aStarPath(t, cur, stop);
			pathCount = 0;
			changed = false;
			//printMap(t, curx, cury, curz, curd);
			//cout << "New path..." << endl;
		}
	}
}

// Initializes maps and generates a random solvable maze
vector<int> setupNodeArrays(node turtleMap[XSIZE][YSIZE][ZSIZE], node randomMap[XSIZE][YSIZE][ZSIZE], coordinate start, coordinate stop) {
	vector<int> path;
	bool isSolvable = false;

	initializeNodeCoordinates(turtleMap);
	turtleMap[start.x][start.y][start.z].setd((start.d + 2)%4); // set starting direction
	initializeNodeCoordinates(randomMap);

	unsigned int seed = time(NULL);//1462483860;
	srand(seed);
	cout << seed << endl;

	while (!isSolvable) {
		generateRandomMaze(randomMap);
		path = aStarPath(randomMap, start, stop);
		if (randomMap[stop.x][stop.y][stop.z].getg()) { isSolvable = true; }
	}

	return path;
}

// Traverses map as many times as it takes to discover the best path (until the path repeats itself)
vector<int> moveUntilBestPath(node turtleMap[XSIZE][YSIZE][ZSIZE], node randomMap[XSIZE][YSIZE][ZSIZE], coordinate start, coordinate stop) {
	vector<int> path, lastPath; // path array used to store previous path
	bool pathsAreSame = false;

	while (!pathsAreSame) {
		moveTo(turtleMap, randomMap, start, stop);

		path = aStarPath(turtleMap, start, stop);

		pathsAreSame = true;
		if (path != lastPath) {
			pathsAreSame = false;
			lastPath = path;
			printMap(turtleMap);
		}
	}

	return path;
}

int main() {
	node turtleMap[XSIZE][YSIZE][ZSIZE]; // nodes turtle sees
	node randomMap[XSIZE][YSIZE][ZSIZE]; // actual map nodes for turtle to discover
	vector<int> path; // array of movement commands representing a path to the end-point

	coordinate start, stop;
	start.x = 0,         start.y = 0,         start.z = 0,         start.d = 0;
	stop.x  = XSIZE - 1, stop.y  = YSIZE - 1, stop.z  = ZSIZE - 1, stop.d  = 0; // if final direction is -1, final direction will be ignored

	path = setupNodeArrays(turtleMap, randomMap, start, stop);

	printMap(randomMap);
	printPath(path);
	cout << "Cost: " << randomMap[stop.x][stop.y][stop.z].getg() << endl;

	path = moveUntilBestPath(turtleMap, randomMap, start, stop);

	//printMap(turtleMap);

	path = aStarPath(turtleMap, start, stop);
	printPath(path);
	cout << "Cost: " << turtleMap[stop.x][stop.y][stop.z].getg() << endl;

	//printValues(turtleMap);

	return 0;
}
