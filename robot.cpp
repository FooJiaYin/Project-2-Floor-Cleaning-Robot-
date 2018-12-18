#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <vector>
//#include "stdafx.h"
#include "stdio.h"
#include "stdlib.h"
#include "windows.h"
using namespace std;

long int max_Battery, Battery;

typedef struct
{
	int x;
	int y;
} Point;

unsigned long total_zero=1;
unsigned long current = 1;
unsigned long max_step = 0;
int max_row, max_col;
Point pt(int, int);
Point now;
Point endd;
vector<Point> path;
int** minStep;
void visit(int**, Point);
//int visit(int**, Point, Point);
void print(int**);
void countSteps(Point *queue, int** map);
void charge(int** map, int back);

int main (int argc, char* argv[]) {
    string dir = argv[1], dir_in, dir_out;
    dir_in = "test_case/" + dir + "/floor.data";
    dir_out = "test_case/" + dir + "/final.path";
    //dir_count = "test_case/" + dir + "/stepCount.txt";
    ifstream input(dir_in);
    ofstream output(dir_out);
    //ofstream stepCount(dir_count);

    if(input.is_open() && output.is_open()) {
        input >> max_row >> max_col >> max_Battery;
        char temp;
        Point start;
        /* Read the map */
        minStep = new int*[max_row];
        int** map = new int*[max_row];
        for(int i=0; i<max_row; i++) {
            minStep[i] = new int[max_col];
            map[i] = new int[max_col];
            for(int j=0; j<max_col; j++) {
                do {
                    input >> temp;
                } while(temp==' '|| temp=='\n');
                if(temp=='R') {
                    start.x = i;
                    start.y = j;
                    map[i][j] = 1;
                }
                else if(temp=='0') map[i][j] = 0;
                else if(temp=='1') map[i][j] = 1;
                minStep[i][j] = 0;
            }
        }
        /*for(int i=0; i<max_row; i++) {
            for(int j=0; j<max_col-1; j++) {
                output << map[i][j] << ' ';
            }
            output << map[i][max_col-1] << endl;
        }*/
        Point* queue = new Point[max_row*max_col];
        Battery = max_Battery;
        cout << start.x << ' ' << start.y << endl;
        queue[0].x = start.x;
        queue[0].y = start.y;
        minStep[start.x][start.y] = -1;
        cout << start.x << ' ' << start.y << ' ' << minStep[start.x][start.y] << endl;
        countSteps(queue, map);
        //output << max_step;
        minStep[start.x][start.y] = 0;
        map[start.x][start.y] = 0;

        /*for (int i = 0; i < max_row; i++) {
            for (int j = 0; j < max_col; j++) {
                output << setw(4) << minStep[i][j];
            }
            output << endl;
        }*/
        cout << "pt" << start.x << ' ' << start.y << ' ' << map[start.x][start.y] << endl;
        visit(map, pt(start.x, start.y));
        now.x = endd.x, now.y = endd.y;
        charge(map, 0);
        output << path.size() << endl;
        for(auto pt : path) output << pt.x << ' ' << pt.y << endl;

        input.close();
        output.close();
    }

    else if(!input.is_open()) cout << "Unable to open testcase" << endl;
    else cout << "Unable to open output" << endl;
    system("Pause");
    return 0;
}

Point pt(int x, int y)
{
	Point p = { x, y };
	return p;
}

void visit(int** map, Point start)
{
    //cout << total_zero << endl;
	if (!map[start.x][start.y])
	{
		map[start.x][start.y] = 2;
        now.x = start.x;
        now.y = start.y;
		if(total_zero) {
            print(map); 
            if(Battery<=minStep[start.x][start.y]) 
                charge(map, 1);
            visit(map, pt(start.x, start.y + 1));
            visit(map, pt(start.x + 1, start.y));
            visit(map, pt(start.x, start.y - 1));
            visit(map, pt(start.x - 1, start.y));
            now.x = start.x;
            now.y = start.y;
            if(total_zero) print(map); 
            if(Battery<=minStep[start.x][start.y]) 
                charge(map, 1);
            //else charge(map);
        }
	}
	return;
}

void check(int i, int j, unsigned int steps, Point *queue, int** map)
{
    //cout << i << ' ' << j << endl;
    if(i>0 && j>0 && i<max_row && j<max_col && !map[i][j] && !minStep[i][j]) {
        minStep[i][j] = steps;
        queue[current].x = i;
        queue[current].y = j;
        current++;
    }
    else if(map[i][j]) minStep[i][j] = -1;
}

void countSteps(Point *queue, int** map) {
    unsigned long read = 0, endd = 0, steps = 0;
    for(steps=1; current!=endd; ++steps){
        endd = current;
        for(; read<endd; read++) {
            //cout << read << endl;
            check(queue[read].x+1, queue[read].y, steps, queue, map);
            check(queue[read].x-1, queue[read].y, steps, queue, map);
            check(queue[read].x, queue[read].y+1, steps, queue, map);
            check(queue[read].x, queue[read].y-1, steps, queue, map);
        }
    }
    cout << steps;
    max_step = steps;
}

void charge(int** map, int back) {
    unsigned step = minStep[now.x][now.y];
    Point pathToCharge[step+1];
    while(minStep[now.x][now.y]) {
        pathToCharge[minStep[now.x][now.y]].x = now.x;
        pathToCharge[minStep[now.x][now.y]].y = now.y;
        if(minStep[now.x][now.y-1]==minStep[now.x][now.y]-1) now.y--;
        else if(minStep[now.x][now.y+1]==minStep[now.x][now.y]-1) now.y++;
        else if(minStep[now.x-1][now.y]==minStep[now.x][now.y]-1) now.x--;
        else if(minStep[now.x+1][now.y]==minStep[now.x][now.y]-1) now.x++;
        map[now.x][now.y] = 2;
        print(map);
    }
    Battery = max_Battery;
    if(back) {
        for(int i=1; i<=step; i++) {
            now.x = pathToCharge[i].x;
            now.y = pathToCharge[i].y;
            print(map);
        }
    }
}

void print(int** map) {
	int i, j;
    total_zero = 0;
	//system("CLS");
	//printf("Battery=%d\n", Battery);
    Battery--;
	for (i = 0; i < max_row; i++)
	{
		for (j = 0; j < max_col; j++) {
            if(i==now.x && j==now.y) ;//printf("0 ");
            else if(map[i][j]==0) total_zero++;
            /*else switch (map[i][j])
            {
            case 0: total_zero++; printf("* "); break;
            case 1: printf("X "); break;
            case 2: printf("  "); break;
            }*/
        }
		//printf("\n");
	}
    if(total_zero==0) endd.x = now.x,  endd.y = now.y;
    //printf("%u\n", total_zero);
    //cout << "pt" <<' ' << now.x << ' ' << now.y << endl;
    path.push_back(pt(now.x, now.y));
	//Sleep(0.01);
    return;
}