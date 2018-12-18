#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <vector>
#include <stack>
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
void visit(char**, Point, int**);
//int visit(int**, Point, Point);
void print(char**);
void countSteps(Point start, char** map, int** minStep, bool init);
void charge(char** map, int back, int** minStep);
void findNext(char**, int**);
bool isolated(char** map, int x, int y);

int main (int argc, char* argv[]) {
    string dir = argv[1], dir_in, dir_out;
    dir_in = "test_case/" + dir + "/floor.data";
    dir_out = "test_case/" + dir + "/final.path";
    string dir_count = "test_case/" + dir + "/stepCount.txt";
    ifstream input(dir_in);
    ofstream output(dir_out);
    //ofstream stepCount(dir_count);

    if(input.is_open() && output.is_open()) {
        input >> max_row >> max_col >> max_Battery;
        Point start;

        /* Read the map */
        int** minStep = new int*[max_row];
        char** map = new char*[max_row];
        for(int i=0; i<max_row; i++) {
            minStep[i] = new int[max_col];
            map[i] = new char[max_col];
            for(int j=0; j<max_col; j++) {
                do {
                    input >> map[i][j];
                } while(map[i][j]==' '|| map[i][j]=='\n');
                if(map[i][j]=='R') {
                    start.x = i;
                    start.y = j;
                }
                else if(map[i][j]=='0') total_zero++;
                minStep[i][j] = 0;
            }
        }
        /*cout << start.x << ' ' << start.y << endl;
        for(int i=0; i<max_row; i++) {
            for(int j=0; j<max_col-1; j++) {
                output << map[i][j] << ' ';
            }
            output << map[i][max_col-1] << endl;
        }*/
        
        Battery = max_Battery;
        minStep[start.x][start.y] = -1;
        countSteps(start, map, minStep, 1);
        /*for (int i = 0; i < max_row; i++) {
            for (int j = 0; j < max_col; j++) {
                output << setw(4) << minStep[i][j];
            }
            output << endl;
        }
        output << max_step;*/
        minStep[start.x][start.y] = 0;
        map[start.x][start.y] = 0;
        //visit(map, pt(start.x, start.y), minStep);
        //now.x = endd.x, now.y = endd.y;

        now.x = start.x, now.y = start.y;
        print(map);
        while(total_zero) {
            if(Battery<=minStep[now.x][now.y] || (Battery<max_Battery*0.75 && (map[now.x][now.y-1]=='R'||map[now.x][now.y+1]=='R'||map[now.x-1][now.y]=='R'||map[now.x+1][now.y]=='R'))) 
                charge(map, 1, minStep);
            if(isolated(map, now.x, now.y+1)) now.y++;
            else if(isolated(map, now.x+1, now.y)) now.x++;
            else if(isolated(map, now.x, now.y-1)) now.y--;
            else if(isolated(map, now.x-1, now.y)) now.x--;
            else if(map[now.x][now.y+1]=='0') now.y++;
            else if(map[now.x+1][now.y]=='0') now.x++;
            else if(map[now.x][now.y-1]=='0') now.y--;
            else if(map[now.x-1][now.y]=='0') now.x--;
            else {
                findNext(map, minStep);
                continue;
            }
            map[start.x][start.y] = 'R';
            map[now.x][now.y] = '2';
            print(map);
        }

        charge(map, 0, minStep);
        output << path.size() << endl;
        for(auto pt : path) output << pt.x << ' ' << pt.y << endl;

        input.close();
        output.close();
    }

    else if(!input.is_open()) cout << "Unable to open testcase" << endl;
    else cout << "Unable to open output" << endl;
    //system("Pause");
    return 0;
}

Point pt(int x, int y)
{
	Point p = { x, y };
	return p;
}

bool isolated(char** map, int x, int y) {
    if(map[x][y]=='0') {
        int count = 0;
        if(map[x][y+1]!='0') count++;
        if(map[x][y-1]!='0') count++;
        if(map[x+1][y]!='0') count++;
        if(map[x-1][y]!='0') count++;
        //cout << count << endl;
        if(count>2) return true;
    }
    return false;
}

bool check(int i, int j, unsigned int steps, Point *queue, char** map, int** minStep)
{
    //cout << i << ' ' << j << endl;
    if(i>0 && j>0 && i<max_row && j<max_col && !minStep[i][j] && (i!=queue[0].x || j!=queue[0].y)) {
        if(map[i][j]=='1'||map[i][j]=='R') minStep[i][j] = -1;
        else {
            minStep[i][j] = steps;
            queue[current].x = i;
            queue[current].y = j;
            if(map[i][j]=='0') return true;
            current++;
        }
    }
    return false;
}

Point *constructPath(int** minStep, Point p) {
    Point *pathTo0 = new Point[minStep[p.x][p.y]+1];
    while(minStep[p.x][p.y]) {
        pathTo0[minStep[p.x][p.y]].x = p.x;
        pathTo0[minStep[p.x][p.y]].y = p.y;
        if(minStep[p.x][p.y-1]==minStep[p.x][p.y]-1) p.y--;
        else if(minStep[p.x][p.y+1]==minStep[p.x][p.y]-1) p.y++;
        else if(minStep[p.x-1][p.y]==minStep[p.x][p.y]-1) p.x--;
        else if(minStep[p.x+1][p.y]==minStep[p.x][p.y]-1) p.x++;
    }
    return pathTo0;
}

void findNext(char** map, int** minStep) {
    current = 1;
    Point* queue = new Point[max_row*max_col];
    int** miniStep = new int*[max_row];
    for(int i=0; i<max_row; i++) {
        miniStep[i] = new int[max_col];
        for(int j=0; j<max_col; j++) 
            miniStep[i][j] = 0;
    }
    queue[0].x = now.x;
    queue[0].y = now.y;
    unsigned int read = 0, endd = 0, steps = 0;
    bool found = false;
    for(steps=1; current!=endd && !found; ++steps){
        endd = current;
        for(; read<endd && !found; read++) {
            if(check(queue[read].x+1, queue[read].y, steps, queue, map, miniStep)) found = true;
            else if(check(queue[read].x-1, queue[read].y, steps, queue, map, miniStep)) found = true;
            else if(check(queue[read].x, queue[read].y+1, steps, queue, map, miniStep)) found = true;
            else if(check(queue[read].x, queue[read].y-1, steps, queue, map, miniStep)) found = true;
        }
        if(Battery<steps) {         
            delete[] queue;
            delete[] miniStep;
            charge(map, 1, minStep);
            return;
        }
    } 
	//system("CLS");
    /*for (int i = 0; i < max_row; i++)
	{
		for (int j = 0; j < max_col; j++) {
            if(i==now.x && j==now.y) printf("0 ");
            //else if(map[i][j]=='0') total_zero++;
            else switch (map[i][j])
            {
            case '0': total_zero++; printf("* "); break;
            case '1': printf("X "); break;
            case '2': printf("  "); break;
            case 'R': printf("R "); break;
            }
        }
		printf("\n");
	}
    for (int i = 0; i < max_row; i++) {
        for (int j = 0; j < max_col; j++) {
            cout << setw(2) << miniStep[i][j];
        }
        cout << endl;
    }
    cout << "now: " << now.x << " " << now.y << endl;
    cout << "next: " << queue[current].x << " " << queue[current].y << endl;*/
    if(Battery < steps + minStep[queue[current].x][queue[current].y]) {         
        delete[] queue;
        delete[] miniStep;
        charge(map, 1, minStep);
        return;
    }
    int totalStep = miniStep[queue[current].x][queue[current].y];
    Point *pathTo0 = constructPath(miniStep, queue[current]);

    for(int i=1; i<=totalStep; i++) {
        now.x = pathTo0[i].x;
        now.y = pathTo0[i].y;
        map[now.x][now.y] = '2';
        print(map);
        //cout << now.x << ' ' << now.y << endl;
    }
    delete[] queue;
    delete[] miniStep;
}

void visit(char** map, Point start, int** minStep)
{
    //cout << total_zero << endl;
	if (map[start.x][start.y]=='0')
	{
		map[start.x][start.y] = '2';
        now.x = start.x;
        now.y = start.y;
		if(total_zero) {
            print(map); 
            if(Battery<=minStep[start.x][start.y]) 
                charge(map, 1, minStep);
            visit(map, pt(start.x, start.y + 1), minStep);
            visit(map, pt(start.x + 1, start.y), minStep);
            visit(map, pt(start.x, start.y - 1), minStep);
            visit(map, pt(start.x - 1, start.y), minStep);
            now.x = start.x;
            now.y = start.y;
            if(total_zero) print(map); 
            if(Battery<=minStep[start.x][start.y]) 
                charge(map, 1, minStep);
            //else charge(map);
        }
	}
	return;
}

void mark(int i, int j, unsigned int steps, Point *queue, char** map, int** minStep)
{
    //cout << i << ' ' << j << endl;
    if(i>0 && j>0 && i<max_row && j<max_col && map[i][j]=='0' && !minStep[i][j]) {
        minStep[i][j] = steps;
        queue[current].x = i;
        queue[current].y = j;
        current++;
    }
    else if(map[i][j]=='1') minStep[i][j] = -1;
}

void countSteps(Point start, char** map, int** minStep, bool init) {
    Point* queue = new Point[max_row*max_col];
    queue[0].x = start.x;
    queue[0].y = start.y;
    unsigned long read = 0, endd = 0, steps = 0;
    for(steps=1; current!=endd; ++steps){
        endd = current;
        for(; read<endd; read++) {
            //cout << read << endl;
            mark(queue[read].x+1, queue[read].y, steps, queue, map, minStep);
            mark(queue[read].x-1, queue[read].y, steps, queue, map, minStep);
            mark(queue[read].x, queue[read].y+1, steps, queue, map, minStep);
            mark(queue[read].x, queue[read].y-1, steps, queue, map, minStep);
        }
    }
    max_step = steps;
    delete[] queue;
    /*for (int i = 0; i < max_row; i++) {
        for (int j = 0; j < max_col; j++) {
            output << setw(4) << minStep[i][j];
        }
        output << endl;
    }*/
}

void charge(char** map, int back, int** minStep) {
    unsigned step = minStep[now.x][now.y];
    Point pathToCharge[step+1];
    int direction;
    while(minStep[now.x][now.y]) {
        //pathToCharge[minStep[now.x][now.y]].x = now.x;
        //pathToCharge[minStep[now.x][now.y]].y = now.y;
        if(minStep[now.x][now.y-1]==minStep[now.x][now.y]-1 && map[now.x][now.y-1]=='0') now.y--;
        else if(minStep[now.x][now.y+1]==minStep[now.x][now.y]-1 && map[now.x][now.y+1]=='0') now.y++;
        else if(minStep[now.x-1][now.y]==minStep[now.x][now.y]-1 && map[now.x-1][now.y]=='0') now.x--;
        else if(minStep[now.x+1][now.y]==minStep[now.x][now.y]-1 && map[now.x+1][now.y]=='0') now.x++;
        else if(minStep[now.x][now.y-1]==minStep[now.x][now.y]-1) now.y--, direction = 0;
        else if(minStep[now.x][now.y+1]==minStep[now.x][now.y]-1) now.y++, direction = 1;
        else if(minStep[now.x-1][now.y]==minStep[now.x][now.y]-1) now.x--, direction = 2;
        else if(minStep[now.x+1][now.y]==minStep[now.x][now.y]-1) now.x++, direction = 3;
        if(map[now.x][now.y] == '0') map[now.x][now.y] = '2';
        print(map);
    }
    Battery = max_Battery;
    if(back) {
        //cout << now.x << ' ' << now.y << ' ' << direction << endl;
        switch(direction) {
            case 0: now.y++; break;
            case 1: now.y--; break;
            case 2: now.x++; break;
            case 3: now.x--; break;
        }
        print(map);
        /*for(int i=1; i<=step; i++) {
            now.x = pathToCharge[i].x;
            now.y = pathToCharge[i].y;
            print(map);
        }*/
        findNext(map, minStep);
    }
}

void print(char** map) {
	int i, j;
    total_zero = 0;
	/*system("CLS");
	printf("Battery=%d\n", Battery);*/
    Battery--;
	for (i = 0; i < max_row; i++)
	{
		for (j = 0; j < max_col; j++) {
            if(i==now.x && j==now.y) ;//printf("0 ");
            else if(map[i][j]=='0') total_zero++;
            /*else switch (map[i][j])
            {
                case '0': total_zero++; printf("* "); break;
                case '1': printf("X "); break;
                case '2': printf("  "); break;
                case 'R': printf("R "); break;
            }*/
        }
		//printf("\n");
	}
    if(total_zero==0) endd.x = now.x,  endd.y = now.y;
    //printf("%u\n", total_zero);
    //cout << "pt" <<' ' << now.x << ' ' << now.y << " Battery:" << Battery << endl;
    path.push_back(pt(now.x, now.y));
	//Sleep(0.01);
    return;
}