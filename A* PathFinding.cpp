#include <iostream>
#include <limits>	// to set int to max value
#include <stdlib.h>
#include <time.h>

using namespace std;

#define SIZE 5	// upper limit: 159; lower limit: 2

int startx = 0, starty = 0, startd = 0;
int stopx = 0, stopy = SIZE - 1;
int hfactor = 1;	// higher the factor, faster the solution but not guaranteed to be the shortest solution

static int dircostmap[4][4] = { {1, 2, 3, 2}, {2, 1, 2, 3}, {3, 2, 1, 2}, {2, 3, 2, 1} };	// maps move costs with directions

class node {            // block map node (meant to be created as an array)
	int id;             // block ID
	int x, y, z;        // x, y, and z coords of node
	int g, h;           // g & h costs
	int xp, yp, zp, d;  // parent coordinates and direction of parent from current node
	int status;         // unvisited/open/closed status (0/1/2 respectively)
	public:
		node() {
			id = 0;
			x = NULL;
			y = NULL;
			z = NULL;
			g = NULL;   // g cost (cost needed to get to current node from start)
			h = NULL;   // h cost (projected cost to get from current node to end)
			xp = NULL;
			yp = NULL;
			zp = NULL;
			d = NULL;
			status = 0;
		}
		node(int xi, int yi, int zi) {
			x = xi;
			y = yi;
			z = zi;
		}
		int abs(int n) { return (n < 0) ? -1*n : n; }
		int getid() { return id; }
		int getx() { return x; }
		int gety() { return y; }
		int getz() { return z; }
		int getf() { return g + h; }
		int getg() { return g; }
		int geth() { return h; }
		int getxp() { return xp; }
		int getyp() { return yp; }
		int getzp() { return zp; }
		int getd() { return d; }
		int getstatus() { return status; }
		void setid(int i) { id = i; }
		void setx(int xi) { x = xi; }
		void sety(int yi) { y = yi; }
		void setz(int zi) { z = zi; }
		void setd(int di) { d = di; }
		void setg(int gi) { g = gi; }
		void seth(int hi) { h = hi; }
		void setxyz(int xi, int yi, int zi) { x = xi; y = yi; z = zi; }
		void setxp(int xi) { xp = xi; }
		void setyp(int yi) { yp = yi; }
		void setzp(int zi) { zp = zi; }
		void setxyzp(int xi, int yi, int zi) {
			xp = xi;
			yp = yi;
			zp = zi;
			d = ((xi - x) > 0) + 2*((yi - y) < 0) + 3*((xi - x) < 0);	// set direction to point at parent
		}
		void setstatus(int s) { status = s; }
		void calcg(node f) {
			g = f.getg() + dircostmap[f.getd()][d];   // previous movement cost + direction-cost (uses direction faced in previous node and current direction facing now)
		}
		void calch(int xf, int yf, int zf) {
			int dir = 0;
			if ((d%2 == 0)&&(xf - x != 0) || (d%2 == 1)&&(yf - y != 0)) { dir = 1; }
			if ((d == 2)&&(yf < y) || (d == 3)&&(xf < x) || (d == 0)&&(yf > y) || (d == 1)&&(xf > x)) { dir = 2; }
			h = abs(xf - x) + abs(yf - y) + dir;    // cost if all nodes travelled to finish from current node were walkable
			h *= hfactor;
		}
};

void initializeNodeCoords(node a[SIZE][SIZE][1]) {
	for (int i = 0; i < SIZE; i++) {
		for (int j = 0; j < SIZE; j++) {
			a[i][j][0].setxyz(i, j, 0);
		}
	}
	a[startx][starty][0].setd(2);
}

void generateRandomMaze(node n[SIZE][SIZE][1]) {
    int percent = 30;

    srand((unsigned int)time(NULL));

    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            n[j][i][0].setid((int)(rand()%100 < percent));
        }
    }
}

void printValues(node n[SIZE][SIZE][1]) {
	cout << " y" << endl << " ^" << endl;
	for (int i = SIZE - 1; i >= 0; i--) {	// y-coords
		cout << " |\t";
		for (int j = 0; j < SIZE; j++) {	// x-coords
			cout << n[j][i][0].getx() << "," << n[j][i][0].gety() << "," << n[j][i][0].getid() << "\t";	// node coords
		}
		cout << endl << " |\t";
		for (int j = 0; j < SIZE; j++) {
			cout << n[j][i][0].getxp() << "," << n[j][i][0].getyp() << "," << n[j][i][0].getd() << "\t";	// parent coords
		}
		cout << endl << " |\t";
		for (int j = 0; j < SIZE; j++) {
			cout << n[j][i][0].getg() << "," << n[j][i][0].geth() << "," << n[j][i][0].getstatus() << "\t";	// g and h values
		}
		cout << endl << " |" << endl;
	}
	cout << " ";
	for (int i = 0; i <= SIZE; i++) {
		cout << "--------";
	}
	cout << ">x" << endl;
}

void printMap(node n[SIZE][SIZE][1], int curx, int cury) {
    for (int i = SIZE - 1; i >= 0; i--) {
        cout << "\t";
        for (int j = 0; j < SIZE; j++) {
            if (j == curx && i == cury) {
                switch (n[j][i][0].getd()) {
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
                switch (n[j][i][0].getid()) {
                    case 0:
                        cout << ".";
                        break;
                    default:
                        cout << "0";
                }
            }

        }
        cout << endl;
    }
    cout << endl;
}

void setWalls(node n[SIZE][SIZE][1]) {
	n[SIZE - 2][SIZE - 1][0].setid(1);
	n[SIZE - 2][SIZE - 2][0].setid(1);
	n[SIZE - 2][0][0].setid(1);
	//n[0][1][0].setid(1);
}

void lowestF(node n[SIZE][SIZE][1], int &x, int &y) {
	int lowf = numeric_limits<int>::max();
	x = -1;	// x and y negatives if failed to find anything in open list
	y = -1;

	for (int i = 0; i < SIZE; i++) {
		for (int j = 0; j < SIZE; j++) {
			if (n[i][j][0].getid() == 0 && n[i][j][0].getstatus() == 1 && n[i][j][0].getf() < lowf) {	// if walkable and on open list and lower than lowest F encountered
				lowf = n[i][j][0].getf();
				x = i;
				y = j;
			}
		}
	}
}

void aStarPath(node n[SIZE][SIZE][1], int path[SIZE*SIZE], int &pathlength, int x0, int y0, int x1, int y1) {
    int curx = x0, cury = y0;
	int oldg;

	printMap(n, -1, -1);

	n[curx][cury][0].setstatus(1); // open starting node
	lowestF(n, curx, cury);	// set the current x, y coords to the node with the lowest F score

	while (n[x1][y1][0].getstatus() != 2 && curx >= 0 && cury >= 0) {	// while final node is not closed and open list is not empty

        printMap(n, curx, cury);

		if (curx + 1 < SIZE) {	// if there's a node in the positive x direction
			if (n[curx + 1][cury][0].getid() == 0) {	// if the node is walkable
				switch (n[curx + 1][cury][0].getstatus()) {
					case 1:	// if the node is in the open list
						oldg = n[curx + 1][cury][0].getg();	// save the old G score for comparison
						n[curx + 1][cury][0].calcg(n[curx][cury][0]);	// recalculate a new G score
						if (n[curx + 1][cury][0].getg() >= oldg) {	// if the new G score is not less than the old
							n[curx + 1][cury][0].setg(oldg);	// set the score back to the old
						}
						else {
							n[curx + 1][cury][0].setxyzp(curx, cury, 0);	// else set the node's parent to current node
							n[curx + 1][cury][0].calch(x1, y1, 0);	// recalculate the H score
						}
						break;
					case 0:	// if the node is not in a list
						n[curx + 1][cury][0].setstatus(1);	// place in open list
						n[curx + 1][cury][0].setxyzp(curx, cury, 0);	// set the node's parent to current node
						n[curx + 1][cury][0].calcg(n[curx][cury][0]);	// calculate the G score
						n[curx + 1][cury][0].calch(x1, y1, 0);	// calculate the H score
						break;
				}
			}
		}
		if (curx > 0) {	// if there's a node in the negative x direction
			if (n[curx - 1][cury][0].getid() == 0) {	// if the node is walkable
				switch (n[curx - 1][cury][0].getstatus()) {
					case 1:	// if the node is in the open list
						oldg = n[curx - 1][cury][0].getg();	// save the old G score for comparison
						n[curx - 1][cury][0].calcg(n[curx][cury][0]);	// recalculate a new G score
						if (n[curx - 1][cury][0].getg() >= oldg) {	// if the new G score is not less than the old
							n[curx - 1][cury][0].setg(oldg);	// set the score back to the old
						}
						else {
							n[curx - 1][cury][0].setxyzp(curx, cury, 0);	// else set the node's parent to current node
							n[curx - 1][cury][0].calch(x1, y1, 0);	// recalculate the H score
						}
						break;
					case 0:	// if the node is not in a list
						n[curx - 1][cury][0].setstatus(1);	// place in open list
						n[curx - 1][cury][0].setxyzp(curx, cury, 0);	// set the node's parent to current node
						n[curx - 1][cury][0].calcg(n[curx][cury][0]);	// calculate the G score
						n[curx - 1][cury][0].calch(x1, y1, 0);	// calculate the H score
						break;
				}
			}
		}
		if (cury + 1 < SIZE) {	// if there's a node in the positive y direction
			if (n[curx][cury + 1][0].getid() == 0) {	// if the node is walkable
				switch (n[curx][cury + 1][0].getstatus()) {
					case 1:	// if the node is in the open list
						oldg = n[curx][cury + 1][0].getg();	// save the old G score for comparison
						n[curx][cury + 1][0].calcg(n[curx][cury][0]);	// recalculate a new G score
						if (n[curx][cury + 1][0].getg() >= oldg) {	// if the new G score is not less than the old
							n[curx][cury + 1][0].setg(oldg);	// set the score back to the old
						}
						else {
							n[curx][cury + 1][0].setxyzp(curx, cury, 0);	// else set the node's parent to current node
							n[curx][cury + 1][0].calch(x1, y1, 0);	// recalculate the H score
						}
						break;
					case 0:	// if the node is not in a list
						n[curx][cury + 1][0].setstatus(1);	// place in open list
						n[curx][cury + 1][0].setxyzp(curx, cury, 0);	// set the node's parent to current node
						n[curx][cury + 1][0].calcg(n[curx][cury][0]);	// calculate the G score
						n[curx][cury + 1][0].calch(x1, y1, 0);	// calculate the H score
						break;
				}
			}
		}
		if (cury > 0) {	// if there's a node in the negative y direction
			if (n[curx][cury - 1][0].getid() == 0) {	// if the node is walkable
				switch (n[curx][cury - 1][0].getstatus()) {
					case 1:	// if the node is in the open list
						oldg = n[curx][cury - 1][0].getg();	// save the old G score for comparison
						n[curx][cury - 1][0].calcg(n[curx][cury][0]);	// recalculate a new G score
						if (n[curx][cury - 1][0].getg() >= oldg) {	// if the new G score is not less than the old
							n[curx][cury - 1][0].setg(oldg);	// set the score back to the old
						}
						else {
							n[curx][cury - 1][0].setxyzp(curx, cury, 0);	// else set the node's parent to current node
							n[curx][cury - 1][0].calch(x1, y1, 0);	// recalculate the H score
						}
						break;
					case 0:	// if the node is not in a list
						n[curx][cury - 1][0].setstatus(1);	// place in open list
						n[curx][cury - 1][0].setxyzp(curx, cury, 0);	// set the node's parent to current node
						n[curx][cury - 1][0].calcg(n[curx][cury][0]);	// calculate the G score
						n[curx][cury - 1][0].calch(x1, y1, 0);	// calculate the H score
						break;
				}
			}
		}

		n[curx][cury][0].setstatus(2);	// close current node
		lowestF(n, curx, cury);	// set the current x, y coords to the node with the lowest F score
	}

    if (n[x1][y1][0].getg() == 0) { return; }  // if no cost required to get to end, this means either start and/or finish are unwalkable or start = finish

	pathlength = 0;
	int nextx, nexty;   // middle-men integers to prevent curx and cury from changing before done calling functions
	curx = x1;
	cury = y1;

	while (curx != x0 || cury != y0) {  // while not at starting node
		path[pathlength] = (n[curx][cury][0].getd() + 2)%4;    // set movement needed to get to current node from parent (reverse of direction to parent)
		pathlength++;   // iterate path length
		nextx = n[curx][cury][0].getxp();  // go to parent cell next
		nexty = n[curx][cury][0].getyp();
		curx = nextx;
		cury = nexty;
	}

    int temp, last = pathlength - 1;

    for (int i = 0; i < pathlength/2; i++) { // reverse path array so returned array is sequential instructions on which direction to go
        temp = path[i];
        path[i] = path[last];
        path[last] = temp;

        last--;
    }
}

int main() {
	node n[SIZE][SIZE][1];
	initializeNodeCoords(n);
	n[startx][stopx][0].setd((startd + 2)%4);  // set starting direction
	generateRandomMaze(n);
    int path[SIZE*SIZE], pathlength;

    aStarPath(n, path, pathlength, startx, starty, stopx, stopy);

	printValues(n);
	for (int i = 0; i < pathlength; i++) {
		cout << path[i] << ", ";
	}
	cout << endl;

	cout << "Cost: " << n[stopx][stopy][0].getg() << endl;

	return 0;
}
