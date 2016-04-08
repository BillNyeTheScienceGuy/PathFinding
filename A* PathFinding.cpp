#include <iostream>
#include <limits>	// to set int to max value
#include <stdlib.h>
#include <time.h>

using namespace std;

#define XSIZE 5    // upper limit: 159; lower limit: 2
#define YSIZE 5    //
#define ZSIZE 5    //

int startx = 0, starty = 0, startz = 0, startd = 0;
int stopx = XSIZE - 1, stopy = YSIZE - 1, stopz = ZSIZE - 1;
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
		void setxyzp(node parent) {
			xp = parent.getx();
			yp = parent.gety();
			zp = parent.getz();
			if (z == zp) {  // if z-coord hasn't changed
                d = ((xp - x) > 0) + 2*((yp - y) < 0) + 3*((xp - x) < 0);	// set direction to point at parent
			}
			else {
                d = parent.getd();
			}
		}
		void setstatus(int s) { status = s; }
		void calcg(node parent) {
			g = parent.getg();  // previous movement cost
			if (z == parent.getz()) { g += dircostmap[parent.getd()][d]; }   // x-y direction cost (uses direction faced in previous node and current direction facing now)
			else { g += 1; }    // z direction cost
		}
		void calch(int xf, int yf, int zf) {
			int dir = 0;
			if ((d%2 == 0)&&(xf - x != 0) || (d%2 == 1)&&(yf - y != 0)) { dir = 1; }
			if ((d == 2)&&(yf < y) || (d == 3)&&(xf < x) || (d == 0)&&(yf > y) || (d == 1)&&(xf > x)) { dir = 2; }
			h = abs(xf - x) + abs(yf - y) + abs(zf - z) + dir;    // cost if all nodes travelled to finish from current node were walkable
			h *= hfactor;
		}
};

void initializeNodeCoords(node a[XSIZE][YSIZE][ZSIZE]) {
	for (int i = 0; i < XSIZE; i++) {
		for (int j = 0; j < YSIZE; j++) {
            for (int k = 0; k < ZSIZE; k++) {
                a[i][j][k].setxyz(i, j, k);
            }
		}
	}
	a[startx][starty][startz].setd(2);
}

void generateRandomMaze(node n[XSIZE][YSIZE][ZSIZE]) {
    int percent = 30;

    srand((unsigned int)time(NULL));

    for (int i = 0; i < ZSIZE; i++) {
        for (int j = 0; j < YSIZE; j++) {
            for (int k = 0; k < XSIZE; k++) {
                n[k][j][i].setid((int)(rand()%100 < percent));
            }
        }
    }

    n[startx][starty][startz].setid(0);
    n[stopx][stopy][stopz].setid(0);
}

void printValues(node n[XSIZE][YSIZE][ZSIZE]) {
    for (int i = 0; i < ZSIZE; i++) {
        cout << endl << " y" << endl << " ^" << endl;
        for (int j = YSIZE - 1; j >= 0; j--) {	// y-coords
            cout << " |\t";
            for (int k = 0; k < XSIZE; k++) {	// x-coords
                cout << n[k][j][i].getx() << "," << n[k][j][i].gety() << "," << n[k][j][i].getid() << "\t";	// node coords
            }
            cout << endl << " |\t";
            for (int k = 0; k < XSIZE; k++) {
                cout << n[k][j][i].getxp() << "," << n[k][j][i].getyp() << "," << n[k][j][i].getd() << "\t";	// parent coords
            }
            cout << endl << " |\t";
            for (int k = 0; k < XSIZE; k++) {
                cout << n[k][j][i].getg() << "," << n[k][j][i].geth() << "," << n[k][j][i].getstatus() << "\t";	// g and h values
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

void printMap(node n[XSIZE][YSIZE][ZSIZE], int curx, int cury, int curz) {
    for (int i = YSIZE - 1; i >= 0; i--) {
        for (int j = 0; j < ZSIZE; j++) {
            cout << "\t";
            for (int k = 0; k < XSIZE; k++) {
                if (k == curx && i == cury && j == curz) {
                    switch (n[k][i][j].getd()) {
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

void setWalls(node n[XSIZE][YSIZE][ZSIZE]) {
	n[XSIZE - 2][YSIZE - 1][0].setid(1);
	n[XSIZE - 2][YSIZE - 2][0].setid(1);
	n[XSIZE - 2][0][0].setid(1);
	//n[0][1][0].setid(1);
}

void lowestF(node n[XSIZE][YSIZE][ZSIZE], int &x, int &y, int &z) {
	int lowf = numeric_limits<int>::max();
	x = -1;	// x and y negatives if failed to find anything in open list
	y = -1;
	z = -1;

	for (int i = 0; i < ZSIZE; i++) {
        for (int j = 0; j < YSIZE; j++) {
            for (int k = 0; k < XSIZE; k++) {
                if (n[k][j][i].getid() == 0 && n[k][j][i].getstatus() == 1 && n[k][j][i].getf() < lowf) {	// if walkable and on open list and lower than lowest F encountered
                    lowf = n[k][j][i].getf();
                    x = k;
                    y = j;
                    z = i;
                }
            }
        }
	}
}

void aStarPath(node n[XSIZE][YSIZE][ZSIZE], int path[XSIZE*YSIZE*ZSIZE], int &pathlength, int x0, int y0, int z0, int x1, int y1, int z1) {
    int curx = x0, cury = y0, curz = z0;
	int oldg;

	printMap(n, -1, -1, -1);

	n[curx][cury][curz].setstatus(1); // open starting node
	lowestF(n, curx, cury, curz);	// set the current x, y coords to the node with the lowest F score

	while (n[x1][y1][z1].getstatus() != 2 && curx >= 0 && cury >= 0 && curz >= 0) {	// while final node is not closed and open list is not empty

        printMap(n, curx, cury, curz);

		if (curx + 1 < XSIZE) {	// if there's a node in the positive x direction
			if (n[curx + 1][cury][curz].getid() == 0) {	// if the node is walkable
				switch (n[curx + 1][cury][curz].getstatus()) {
					case 1:	// if the node is in the open list
						oldg = n[curx + 1][cury][curz].getg();	// save the old G score for comparison
						n[curx + 1][cury][curz].calcg(n[curx][cury][curz]);	// recalculate a new G score
						if (n[curx + 1][cury][curz].getg() >= oldg) {	// if the new G score is not less than the old
							n[curx + 1][cury][curz].setg(oldg);	// set the score back to the old
						}
						else {
							n[curx + 1][cury][curz].setxyzp(n[curx][cury][curz]);	// else set the node's parent to current node
							n[curx + 1][cury][curz].calch(x1, y1, z1);	// recalculate the H score
						}
						break;
					case 0:	// if the node is not in a list
						n[curx + 1][cury][curz].setstatus(1);	// place in open list
						n[curx + 1][cury][curz].setxyzp(n[curx][cury][curz]);	// set the node's parent to current node
						n[curx + 1][cury][curz].calcg(n[curx][cury][curz]);	// calculate the G score
						n[curx + 1][cury][curz].calch(x1, y1, z1);	// calculate the H score
						break;
				}
			}
		}
		if (curx > 0) {	// if there's a node in the negative x direction
			if (n[curx - 1][cury][curz].getid() == 0) {	// if the node is walkable
				switch (n[curx - 1][cury][curz].getstatus()) {
					case 1:	// if the node is in the open list
						oldg = n[curx - 1][cury][curz].getg();	// save the old G score for comparison
						n[curx - 1][cury][curz].calcg(n[curx][cury][curz]);	// recalculate a new G score
						if (n[curx - 1][cury][curz].getg() >= oldg) {	// if the new G score is not less than the old
							n[curx - 1][cury][curz].setg(oldg);	// set the score back to the old
						}
						else {
							n[curx - 1][cury][curz].setxyzp(n[curx][cury][curz]);	// else set the node's parent to current node
							n[curx - 1][cury][curz].calch(x1, y1, z1);	// recalculate the H score
						}
						break;
					case 0:	// if the node is not in a list
						n[curx - 1][cury][curz].setstatus(1);	// place in open list
						n[curx - 1][cury][curz].setxyzp(n[curx][cury][curz]);	// set the node's parent to current node
						n[curx - 1][cury][curz].calcg(n[curx][cury][curz]);	// calculate the G score
						n[curx - 1][cury][curz].calch(x1, y1, z1);	// calculate the H score
						break;
				}
			}
		}
		if (cury + 1 < YSIZE) {	// if there's a node in the positive y direction
			if (n[curx][cury + 1][curz].getid() == 0) {	// if the node is walkable
				switch (n[curx][cury + 1][curz].getstatus()) {
					case 1:	// if the node is in the open list
						oldg = n[curx][cury + 1][curz].getg();	// save the old G score for comparison
						n[curx][cury + 1][curz].calcg(n[curx][cury][curz]);	// recalculate a new G score
						if (n[curx][cury + 1][curz].getg() >= oldg) {	// if the new G score is not less than the old
							n[curx][cury + 1][curz].setg(oldg);	// set the score back to the old
						}
						else {
							n[curx][cury + 1][curz].setxyzp(n[curx][cury][curz]);	// else set the node's parent to current node
							n[curx][cury + 1][curz].calch(x1, y1, z1);	// recalculate the H score
						}
						break;
					case 0:	// if the node is not in a list
						n[curx][cury + 1][curz].setstatus(1);	// place in open list
						n[curx][cury + 1][curz].setxyzp(n[curx][cury][curz]);	// set the node's parent to current node
						n[curx][cury + 1][curz].calcg(n[curx][cury][curz]);	// calculate the G score
						n[curx][cury + 1][curz].calch(x1, y1, z1);	// calculate the H score
						break;
				}
			}
		}
		if (cury > 0) {	// if there's a node in the negative y direction
			if (n[curx][cury - 1][curz].getid() == 0) {	// if the node is walkable
				switch (n[curx][cury - 1][curz].getstatus()) {
					case 1:	// if the node is in the open list
						oldg = n[curx][cury - 1][curz].getg();	// save the old G score for comparison
						n[curx][cury - 1][curz].calcg(n[curx][cury][curz]);	// recalculate a new G score
						if (n[curx][cury - 1][curz].getg() >= oldg) {	// if the new G score is not less than the old
							n[curx][cury - 1][curz].setg(oldg);	// set the score back to the old
						}
						else {
							n[curx][cury - 1][curz].setxyzp(n[curx][cury][curz]);	// else set the node's parent to current node
							n[curx][cury - 1][curz].calch(x1, y1, z1);	// recalculate the H score
						}
						break;
					case 0:	// if the node is not in a list
						n[curx][cury - 1][curz].setstatus(1);	// place in open list
						n[curx][cury - 1][curz].setxyzp(n[curx][cury][curz]);	// set the node's parent to current node
						n[curx][cury - 1][curz].calcg(n[curx][cury][curz]);	// calculate the G score
						n[curx][cury - 1][curz].calch(x1, y1, z1);	// calculate the H score
						break;
				}
			}
		}
		if (curz + 1 < ZSIZE) { // if there's a node in the positive z direction
            if (n[curx][cury][curz + 1].getid() == 0) {  // if the node is walkable
                switch (n[curx][cury][curz + 1].getstatus()) {
                    case 1: // if the node is in the open list
                        oldg = n[curx][cury][curz + 1].getg();  // save the old G score for comparison
                        n[curx][cury][curz + 1].calcg(n[curx][cury][curz]); // recalculate a new G score
                        if (n[curx][cury][curz + 1].getg() >= oldg) {   // if the new G score is not less than the old
                            n[curx][cury][curz + 1].setg(oldg); // set the score back to the old
                        }
                        else {
                            n[curx][cury][curz + 1].setxyzp(n[curx][cury][curz]);  // else set the node's parent to current node
                            n[curx][cury][curz + 1].calch(x1, y1, z1);  // recalculate the H score
                        }
                        break;
                    case 0: // if the node is not in a list
						n[curx][cury][curz + 1].setstatus(1);	// place in open list
						n[curx][cury][curz + 1].setxyzp(n[curx][cury][curz]);	// set the node's parent to current node
						n[curx][cury][curz + 1].calcg(n[curx][cury][curz]);	// calculate the G score
						n[curx][cury][curz + 1].calch(x1, y1, z1);	// calculate the H score
						break;
                }
            }
		}
		if (curz > 0) { // if there's a node in the negative z direction
            if (n[curx][cury][curz - 1].getid() == 0) {  // if the node is walkable
                switch (n[curx][cury][curz - 1].getstatus()) {
                    case 1: // if the node is in the open list
                        oldg = n[curx][cury][curz - 1].getg();  // save the old G score for comparison
                        n[curx][cury][curz - 1].calcg(n[curx][cury][curz]); // recalculate a new G score
                        if (n[curx][cury][curz - 1].getg() >= oldg) {   // if the new G score is not less than the old
                            n[curx][cury][curz - 1].setg(oldg); // set the score back to the old
                        }
                        else {
                            n[curx][cury][curz - 1].setxyzp(n[curx][cury][curz]);  // else set the node's parent to current node
                            n[curx][cury][curz - 1].calch(x1, y1, z1);  // recalculate the H score
                        }
                        break;
                    case 0: // if the node is not in a list
						n[curx][cury][curz - 1].setstatus(1);	// place in open list
						n[curx][cury][curz - 1].setxyzp(n[curx][cury][curz]);	// set the node's parent to current node
						n[curx][cury][curz - 1].calcg(n[curx][cury][curz]);	// calculate the G score
						n[curx][cury][curz - 1].calch(x1, y1, z1);	// calculate the H score
						break;
                }
            }
		}

		n[curx][cury][curz].setstatus(2);	// close current node
		lowestF(n, curx, cury, curz);	// set the current x, y coords to the node with the lowest F score
	}

    if (n[x1][y1][z1].getg() == 0) { return; }  // if no cost required to get to end, this means either start and/or finish are unwalkable or start = finish

	pathlength = 0;
	int nextx, nexty, nextz;   // middle-men integers to prevent curx and cury from changing before done calling functions
	int zdiff;  // difference between node z and parent z
	curx = x1;
	cury = y1;
	curz = z1;

	while (curx != x0 || cury != y0 || curz != z0) {  // while not at starting node
        zdiff = n[curx][cury][curz].getz() - n[curx][cury][curz].getzp();   // -1 if parent above, 0 if no change, 1 if parent below
        switch (zdiff) {
            case 1: // if parent below
                path[pathlength] = 4;   // set movement direction needed to get to current node to go up
                break;
            case -1:    // if parent above
                path[pathlength] = 5;   // set movement direction needed to get to current node to go down
                break;
            default: // if on same z-coord as parent
                path[pathlength] = (n[curx][cury][curz].getd() + 2)%4;  // set movement needed to get to current node from parent (reverse of direction to parent)
        }
		pathlength++;   // iterate path length
		nextx = n[curx][cury][curz].getxp();  // go to parent cell next
		nexty = n[curx][cury][curz].getyp();
		nextz = n[curx][cury][curz].getzp();
		curx = nextx;
		cury = nexty;
		curz = nextz;
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
	node n[XSIZE][YSIZE][ZSIZE];
	initializeNodeCoords(n);
	n[startx][starty][startz].setd((startd + 2)%4);  // set starting direction
	generateRandomMaze(n);
    int path[XSIZE*YSIZE*ZSIZE], pathlength;

    aStarPath(n, path, pathlength, startx, starty, startz, stopx, stopy, stopz);

	//printValues(n);
	for (int i = 0; i < pathlength; i++) {
		cout << path[i] << ", ";
	}
	cout << endl;

	cout << "Cost: " << n[stopx][stopy][stopz].getg() << endl;

	return 0;
}
