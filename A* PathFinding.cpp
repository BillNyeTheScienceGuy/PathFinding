#include <iostream>
#include <limits>	// to set int to max value
 
using namespace std;
 
#define SIZE 10	// upper limit: 159; lower limit: 2
 
int startx = 0, starty = 0, startd = 0;
int stopx = SIZE - 1, stopy = SIZE - 1;
int hfactor = 10;	// higher the factor, faster the solution but not guaranteed to be the shortest solution
 
static int dmap[4][4] = { {3, 2, 1, 2}, {2, 3, 2, 1}, {1, 2, 3, 2}, {2, 1, 2, 3} };	// maps move costs with directions
 
class node {
	bool walkable;	// walkability
	int x, y, d;	// coordinates & direction
	int g, h;		// costs
	int xp, yp;		// parent coordinates
	int status;		// unvisited/open/closed status (0/1/2 respectively)
	public:
		node() {
			walkable = true;
			x = 0;
			y = 0;
			d = 0;
			g = 0;
			h = 0;
			xp = 0;
			yp = 0;
			status = 0;
		}
		node(int xi, int yi) {
			x = xi;
			y = yi;
		}
		bool iswalkable() { return walkable; }
		int getx() { return x; }
		int gety() { return y; }
		int getd() { return d; }
		int getf() { return g + h; }
		int getg() { return g; }
		int geth() { return h; }
		int getxp() { return xp; }
		int getyp() { return yp; }
		int getstatus() { return status; }
		void setwalkable(bool w) { walkable = w; }
		void setx(int xi) { x = xi; }
		void sety(int yi) { y = yi; }
		void setd(int di) { d = di; }
		void setg(int gi) { g = gi; }
		void seth(int hi) { h = hi; }
		void setxy(int xi, int yi) { x = xi; y = yi; }
		void setxp(int xi) { xp = xi; }
		void setyp(int yi) { yp = yi; }
		void setxpyp(int xi, int yi) {
			xp = xi;
			yp = yi;
			d = ((xi - x) > 0) + 2*((yi - y) < 0) + 3*((xi - x) < 0);	// set direction to point at parent
		}
		void setstatus(int s) { status = s; }
		void calcg(node f) {
			g = f.getg();																		// previous movement cost
			int dir = ((f.getx() - x) > 0) + 2*((f.gety() - y) < 0) + 3*((f.getx() - x) < 0);	// direction to next node
			g += dmap[d][dir];																	// direction-cost mapping
		}
		void calch(int xf, int yf) {
			h = abs(xf - x) + abs(yf - y);	// 
			int dir;	// direction value to next node
			switch ((d + 2)%4) {	// direction will be facing in node
				case 0:
					dir = 0*((yf - y) > 0) + 1*((xf - x) > 0) + 2*((yf - y) < 0) + 1*((xf - x) < 0);
					break;
				case 1:
					dir = 1*((yf - y) > 0) + 0*((xf - x) > 0) + 1*((yf - y) < 0) + 2*((xf - x) < 0);
					break;
				case 2:
					dir = 2*((yf - y) > 0) + 1*((xf - x) > 0) + 0*((yf - y) < 0) + 1*((xf - x) < 0);
					break;
				case 3:
					dir = 1*((yf - y) > 0) + 2*((xf - x) > 0) + 1*((yf - y) < 0) + 0*((xf - x) < 0);
					break;
			}
			if (dir == 3) { dir = 2; }
			h += dir;
			h *= hfactor;
		}
};
 
void initializeNodeCoords(node a[SIZE][SIZE]) {
	for (int i = 0; i < SIZE; i++) {
		for (int j = 0; j < SIZE; j++) {
			a[i][j].setxy(i, j);
		}
	}
}
 
void print(node n[SIZE][SIZE]) {
	cout << " y" << endl << " ^" << endl;
	for (int i = SIZE - 1; i >= 0; i--) {	// y-coords
		cout << " |\t";
		for (int j = 0; j < SIZE; j++) {	// x-coords
			cout << n[j][i].getx() << "," << n[j][i].gety() << "\t";	// node coords
		}
		cout << endl << " |\t";
		for (int j = 0; j < SIZE; j++) {
			cout << n[j][i].getxp() << "," << n[j][i].getyp() << "," << n[j][i].getd() << "\t";	// parent coords
		}
		cout << endl << " |\t";
		for (int j = 0; j < SIZE; j++) {
			cout << n[j][i].getg() << "," << n[j][i].geth() << "\t";	// g and h values
		}
		cout << endl << " |" << endl;
	}
	cout << " ";
	for (int i = 0; i <= SIZE; i++) {
		cout << "--------";
	}
	cout << ">x" << endl;
}
 
void setWalls(node n[SIZE][SIZE]) {
	n[SIZE - 2][SIZE - 1].setwalkable(false);
	n[SIZE - 2][SIZE - 2].setwalkable(false);
	n[SIZE - 2][0].setwalkable(false);
	n[0][1].setwalkable(false);
}
 
void lowestF(node n[SIZE][SIZE], int &x, int &y) {
	int lowf = numeric_limits<int>::max();
	x = -1;	// x and y negatives if failed to find anything in open list
	y = -1;
 
	for (int i = 0; i < SIZE; i++) {
		for (int j = 0; j < SIZE; j++) {
			if (n[i][j].iswalkable() && n[i][j].getstatus() == 1 && n[i][j].getf() < lowf) {	// if walkable and on open list and lower than lowest F encountered
				lowf = n[i][j].getf();
				x = i;
				y = j;
			}
		}
	}
}
 
void main() {
	node n[SIZE][SIZE];
	//print(b);
	initializeNodeCoords(n);
	setWalls(n);
 
	int curx = startx, cury = starty;
	int oldg;
 
	n[startx][starty].setstatus(1);
 
	while (n[stopx][stopy].getstatus() != 2 && curx >= 0 && cury >= 0) {	// while final node is not closed and open list is not empty
		lowestF(n, curx, cury);	// set the current x, y coords to the node with the lowest F score
 
		n[curx][cury].setstatus(2);	// closed
 
		if (curx + 1 < SIZE) {	// if there's a node to the right
			if (n[curx + 1][cury].iswalkable()) {	// if the node is walkable
				switch (n[curx + 1][cury].getstatus()) {
					case 1:	// if the node is in the open list
						oldg = n[curx + 1][cury].getg();	// save the old G score for comparison
						n[curx + 1][cury].calcg(n[curx][cury]);	// recalculate a new G score
						if (n[curx + 1][cury].getg() >= oldg) {	// if the new G score is not less than the old
							n[curx + 1][cury].setg(oldg);	// set the score back to the old
						}
						else {
							n[curx + 1][cury].setxpyp(curx, cury);	// else set the node's parent to current node
							n[curx + 1][cury].calch(stopx, stopy);	// recalculate the H score
						}
						break;
					case 0:	// if the node is not in a list
						n[curx + 1][cury].setstatus(1);	// place in open list
						n[curx + 1][cury].calcg(n[curx][cury]);	// calculate the G score
						n[curx + 1][cury].calch(stopx, stopy);	// calculate the H score
						n[curx + 1][cury].setxpyp(curx, cury);	// set the node's parent to current node
						break;
				}
			}
		}
		if (curx > 0) {	// if there's a node to the left
			if (n[curx - 1][cury].iswalkable()) {	// if the node is walkable
				switch (n[curx - 1][cury].getstatus()) {
					case 1:	// if the node is in the open list
						oldg = n[curx - 1][cury].getg();	// save the old G score for comparison
						n[curx - 1][cury].calcg(n[curx][cury]);	// recalculate a new G score
						if (n[curx - 1][cury].getg() >= oldg) {	// if the new G score is not less than the old
							n[curx - 1][cury].setg(oldg);	// set the score back to the old
						}
						else {
							n[curx - 1][cury].setxpyp(curx, cury);	// else set the node's parent to current node
							n[curx - 1][cury].calch(stopx, stopy);	// recalculate the H score
						}
						break;
					case 0:	// if the node is not in a list
						n[curx - 1][cury].setstatus(1);	// place in open list
						n[curx - 1][cury].calcg(n[curx][cury]);	// calculate the G score
						n[curx - 1][cury].calch(stopx, stopy);	// calculate the H score
						n[curx - 1][cury].setxpyp(curx, cury);	// set the node's parent to current node
						break;
				}
			}
		}
		if (cury + 1 < SIZE) {	// if there's a node above
			if (n[curx][cury + 1].iswalkable()) {	// if the node is walkable
				switch (n[curx][cury + 1].getstatus()) {
					case 1:	// if the node is in the open list
						oldg = n[curx][cury + 1].getg();	// save the old G score for comparison
						n[curx][cury + 1].calcg(n[curx][cury]);	// recalculate a new G score
						if (n[curx][cury + 1].getg() >= oldg) {	// if the new G score is not less than the old
							n[curx][cury + 1].setg(oldg);	// set the score back to the old
						}
						else {
							n[curx][cury + 1].setxpyp(curx, cury);	// else set the node's parent to current node
							n[curx][cury + 1].calch(stopx, stopy);	// recalculate the H score
						}
						break;
					case 0:	// if the node is not in a list
						n[curx][cury + 1].setstatus(1);	// place in open list
						n[curx][cury + 1].calcg(n[curx][cury]);	// calculate the G score
						n[curx][cury + 1].calch(stopx, stopy);	// calculate the H score
						n[curx][cury + 1].setxpyp(curx, cury);	// set the node's parent to current node
						break;
				}
			}
		}
		if (cury > 0) {	// if there's a node below
			if (n[curx][cury - 1].iswalkable()) {	// if the node is walkable
				switch (n[curx][cury - 1].getstatus()) {
					case 1:	// if the node is in the open list
						oldg = n[curx][cury - 1].getg();	// save the old G score for comparison
						n[curx][cury - 1].calcg(n[curx][cury]);	// recalculate a new G score
						if (n[curx][cury - 1].getg() >= oldg) {	// if the new G score is not less than the old
							n[curx][cury - 1].setg(oldg);	// set the score back to the old
						}
						else {
							n[curx][cury - 1].setxpyp(curx, cury);	// else set the node's parent to current node
							n[curx][cury - 1].calch(stopx, stopy);	// recalculate the H score
						}
						break;
					case 0:	// if the node is not in a list
						n[curx][cury - 1].setstatus(1);	// place in open list
						n[curx][cury - 1].calcg(n[curx][cury]);	// calculate the G score
						n[curx][cury - 1].calch(stopx, stopy);	// calculate the H score
						n[curx][cury - 1].setxpyp(curx, cury);	// set the node's parent to current node
						break;
				}
			}
		}
	}

	int path[SIZE*SIZE], pointer = 0;
	int nextx, nexty;
	curx = stopx;
	cury = stopy;

	while (curx != startx || cury != starty) {
		path[pointer] = (n[curx][cury].getd() + 2)%4;
		pointer++;
		nextx = n[curx][cury].getxp();
		nexty = n[curx][cury].getyp();
		curx = nextx;
		cury = nexty;
	}

	print(n);
	for (int i = pointer - 1; i >= 0; i--) {
		cout << path[i] << ", ";
	}
	cout << endl;
}
