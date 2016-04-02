#include <iostream>
#include <limits>	// to set int to max value

using namespace std;

#define SIZE 5	// upper limit: 159; lower limit: 2

int startx = 0, starty = 0, startd = 0;
int stopx = SIZE - 1, stopy = SIZE - 1;
int hfactor = 1;	// higher the factor, faster the solution but not guaranteed to be the shortest solution

static int dircostmap[4][4] = { {1, 2, 3, 2}, {2, 1, 2, 3}, {3, 2, 1, 2}, {2, 3, 2, 1} };	// maps move costs with directions

class node {        // block map node (meant to be created as an array)
	int id;         // block ID
	int x, y, d;	// coordinates & direction
	int g, h;		// g & h costs
	int xp, yp;		// parent coordinates
	int status;		// unvisited/open/closed status (0/1/2 respectively)
	public:
		node() {
			id = NULL;
			x = NULL;
			y = NULL;
			d = NULL;
			g = NULL;
			h = NULL;
			xp = NULL;
			yp = NULL;
			status = 0;
		}
		node(int xi, int yi) {
			x = xi;
			y = yi;
		}
		int abs(int n) { return (n < 0) ? -1*n : n; }
		int getid() { return id; }
		int getx() { return x; }
		int gety() { return y; }
		int getd() { return d; }
		int getf() { return g + h; }
		int getg() { return g; }
		int geth() { return h; }
		int getxp() { return xp; }
		int getyp() { return yp; }
		int getstatus() { return status; }
		void setid(int i) { id = i; }
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
			g = f.getg() + dircostmap[f.getd()][d];   // previous movement cost + direction-cost (uses direction faced in previous node and current direction facing now)
		}
		void calch(int xf, int yf) {
			int dir = 0;
			d = (d + 2)%4;
			if ((d%2 == 0)&&(xf - x != 0) || (d%2 == 1)&&(yf - y != 0)) { dir = 1; }
			if ((d == 0)&&(yf < y) || (d == 1)&&(xf < x) || (d == 2)&&(yf > y) || (d == 3)&&(xf > x)) { dir = 2; }
			d = (d + 2)%4;
			h = abs(xf - x) + abs(yf - y) + dir;    // cost if all nodes travelled to finish from current node were walkable
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

void printValues(node n[SIZE][SIZE]) {
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

void printMap(node n[SIZE][SIZE]) {
    for (int i = SIZE - 1; i >= 0; i--) {
        cout << "\t";
        for (int j = 0; j < SIZE; j++) {
            switch (n[j][i].getid()) {
                case 0:
                    cout << ".";
                    break;
                default:
                    cout << "0";
            }
        }
        cout << endl;
    }
    cout << endl;
}

void setWalls(node n[SIZE][SIZE]) {
	n[SIZE - 2][SIZE - 1].setid(1);
	n[SIZE - 2][SIZE - 2].setid(1);
	n[SIZE - 2][0].setid(1);
	//n[0][1].setid(1);
}

void lowestF(node n[SIZE][SIZE], int &x, int &y) {
	int lowf = numeric_limits<int>::max();
	x = -1;	// x and y negatives if failed to find anything in open list
	y = -1;

	for (int i = 0; i < SIZE; i++) {
		for (int j = 0; j < SIZE; j++) {
			if (n[i][j].getid() == 0 && n[i][j].getstatus() == 1 && n[i][j].getf() < lowf) {	// if walkable and on open list and lower than lowest F encountered
				lowf = n[i][j].getf();
				x = i;
				y = j;
			}
		}
	}
}

void aStar(node n[SIZE][SIZE], int path[SIZE*SIZE], int &pathlength, int x0, int y0, int x1, int y1) {
    int curx = x0, cury = y0;
	int oldg;

	n[curx][cury].setstatus(1);

	lowestF(n, curx, cury);	// set the current x, y coords to the node with the lowest F score

	while (n[x1][y1].getstatus() != 2 && curx >= 0 && cury >= 0) {	// while final node is not closed and open list is not empty
		n[curx][cury].setstatus(2);	// closed

		if (curx + 1 < SIZE) {	// if there's a node in the positive x direction
			if (n[curx + 1][cury].getid() == 0) {	// if the node is walkable
				switch (n[curx + 1][cury].getstatus()) {
					case 1:	// if the node is in the open list
						oldg = n[curx + 1][cury].getg();	// save the old G score for comparison
						n[curx + 1][cury].calcg(n[curx][cury]);	// recalculate a new G score
						if (n[curx + 1][cury].getg() >= oldg) {	// if the new G score is not less than the old
							n[curx + 1][cury].setg(oldg);	// set the score back to the old
						}
						else {
							n[curx + 1][cury].setxpyp(curx, cury);	// else set the node's parent to current node
							n[curx + 1][cury].calch(x1, y1);	// recalculate the H score
						}
						break;
					case 0:	// if the node is not in a list
						n[curx + 1][cury].setstatus(1);	// place in open list
						n[curx + 1][cury].setxpyp(curx, cury);	// set the node's parent to current node
						n[curx + 1][cury].calcg(n[curx][cury]);	// calculate the G score
						n[curx + 1][cury].calch(x1, y1);	// calculate the H score
						break;
				}
			}
		}
		if (curx > 0) {	// if there's a node in the negative x direction
			if (n[curx - 1][cury].getid() == 0) {	// if the node is walkable
				switch (n[curx - 1][cury].getstatus()) {
					case 1:	// if the node is in the open list
						oldg = n[curx - 1][cury].getg();	// save the old G score for comparison
						n[curx - 1][cury].calcg(n[curx][cury]);	// recalculate a new G score
						if (n[curx - 1][cury].getg() >= oldg) {	// if the new G score is not less than the old
							n[curx - 1][cury].setg(oldg);	// set the score back to the old
						}
						else {
							n[curx - 1][cury].setxpyp(curx, cury);	// else set the node's parent to current node
							n[curx - 1][cury].calch(x1, y1);	// recalculate the H score
						}
						break;
					case 0:	// if the node is not in a list
						n[curx - 1][cury].setstatus(1);	// place in open list
						n[curx - 1][cury].setxpyp(curx, cury);	// set the node's parent to current node
						n[curx - 1][cury].calcg(n[curx][cury]);	// calculate the G score
						n[curx - 1][cury].calch(x1, y1);	// calculate the H score
						break;
				}
			}
		}
		if (cury + 1 < SIZE) {	// if there's a node in the positive y direction
			if (n[curx][cury + 1].getid() == 0) {	// if the node is walkable
				switch (n[curx][cury + 1].getstatus()) {
					case 1:	// if the node is in the open list
						oldg = n[curx][cury + 1].getg();	// save the old G score for comparison
						n[curx][cury + 1].calcg(n[curx][cury]);	// recalculate a new G score
						if (n[curx][cury + 1].getg() >= oldg) {	// if the new G score is not less than the old
							n[curx][cury + 1].setg(oldg);	// set the score back to the old
						}
						else {
							n[curx][cury + 1].setxpyp(curx, cury);	// else set the node's parent to current node
							n[curx][cury + 1].calch(x1, y1);	// recalculate the H score
						}
						break;
					case 0:	// if the node is not in a list
						n[curx][cury + 1].setstatus(1);	// place in open list
						n[curx][cury + 1].setxpyp(curx, cury);	// set the node's parent to current node
						n[curx][cury + 1].calcg(n[curx][cury]);	// calculate the G score
						n[curx][cury + 1].calch(x1, y1);	// calculate the H score
						break;
				}
			}
		}
		if (cury > 0) {	// if there's a node in the negative y direction
			if (n[curx][cury - 1].getid() == 0) {	// if the node is walkable
				switch (n[curx][cury - 1].getstatus()) {
					case 1:	// if the node is in the open list
						oldg = n[curx][cury - 1].getg();	// save the old G score for comparison
						n[curx][cury - 1].calcg(n[curx][cury]);	// recalculate a new G score
						if (n[curx][cury - 1].getg() >= oldg) {	// if the new G score is not less than the old
							n[curx][cury - 1].setg(oldg);	// set the score back to the old
						}
						else {
							n[curx][cury - 1].setxpyp(curx, cury);	// else set the node's parent to current node
							n[curx][cury - 1].calch(x1, y1);	// recalculate the H score
						}
						break;
					case 0:	// if the node is not in a list
						n[curx][cury - 1].setstatus(1);	// place in open list
						n[curx][cury - 1].setxpyp(curx, cury);	// set the node's parent to current node
						n[curx][cury - 1].calcg(n[curx][cury]);	// calculate the G score
						n[curx][cury - 1].calch(x1, y1);	// calculate the H score
						break;
				}
			}
		}

		lowestF(n, curx, cury);	// set the current x, y coords to the node with the lowest F score
	}

	pathlength = 0;
	int nextx, nexty;   // middle-men integers to prevent curx and cury from changing before done calling functions
	curx = x1;
	cury = y1;

	while (curx != x0 || cury != y0) {  // while not at starting node
		path[pathlength] = n[curx][cury].getd();    // set movement needed to get to current node
		pathlength++;   // iterate path length
		nextx = n[curx][cury].getxp();  // go to parent cell next
		nexty = n[curx][cury].getyp();
		curx = nextx;
		cury = nexty;
	}
}

int main() {
	node n[SIZE][SIZE];
	//print(b);
	initializeNodeCoords(n);
	setWalls(n);
    int path[SIZE*SIZE], pathlength;

    aStar(n, path, pathlength, startx, starty, stopx, stopy);

	printValues(n);
	for (int i = pathlength - 1; i >= 0; i--) {
		cout << path[i] << ", ";
	}
	cout << endl;

	return 0;
}
