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
//#include <ncurses.h>
//#include <unistd.h>
using namespace std;

long int max_Battery, Battery;

typedef struct
{
	int x;
	int y;
} Point;

unsigned long total_zero=0;
unsigned long current = 1;
unsigned long max_step = 0;
int max_row, max_col;
Point pt(int, int);
Point now;
Point endd;
vector<Point> path;
bool visited[5] = {1, 0, 0, 0, 0};
void visit(char**, Point, int**);
//int visit(int**, Point, Point);
void walk(char**);
void countSteps(Point start, char** map, int** minStep, short** port_id, bool init);
void charge(char** map, int back, int** minStep, short** port_id);
int findNext(char**, int**, short**);
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
        short** port_id = new short*[max_row];
        char** map = new char*[max_row];
        for(int i=0; i<max_row; i++) {
            port_id[i] = new short[max_col];
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
        
        Battery = max_Battery;
        minStep[start.x][start.y] = -1;
        countSteps(start, map, minStep, port_id, 1);
        minStep[start.x][start.y] = 0;
        map[start.x][start.y] = 0;

        now.x = start.x, now.y = start.y;
        walk(map);
        while(total_zero) {
            if(Battery<=minStep[now.x][now.y] || (Battery<max_Battery*0.8 && ((now.y>0 && map[now.x][now.y-1]=='R')||(now.y<max_col-1 && map[now.x][now.y+1]=='R')||(now.x>0 && map[now.x-1][now.y]=='R')||(now.x<max_row-1 && map[now.x+1][now.y]=='R')))) 
                charge(map, 1, minStep, port_id);
            else {
                if(isolated(map, now.x, now.y+1)) now.y++;
                else if(isolated(map, now.x+1, now.y)) now.x++;
                else if(isolated(map, now.x, now.y-1)) now.y--;
                else if(isolated(map, now.x-1, now.y)) now.x--;
                else if(now.y<max_col-1 && map[now.x][now.y+1]=='0') now.y++;
                else if(now.x<max_row-1 && map[now.x+1][now.y]=='0') now.x++;
                else if(now.y>0 && map[now.x][now.y-1]=='0') now.y--;
                else if(now.x>0 && map[now.x-1][now.y]=='0') now.x--;
                else {
                    if(findNext(map, minStep, port_id)==0) charge(map, 1, minStep, port_id);
                    continue;
                }
                map[start.x][start.y] = 'R';
                if(map[now.x][now.y]=='0') total_zero--;
                map[now.x][now.y] = '2';
                walk(map);
            }
        }
        charge(map, 0, minStep, port_id);
        output << path.size() << endl;
        for(auto pt : path) output << pt.x << ' ' << pt.y << endl;

        input.close();
        output.close();
        delete[] port_id;
        delete[] map;
        delete[] minStep;
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
    if(x>=0 && y>=0 && x<max_row && y<max_col && map[x][y]=='0') {
        int count = 0;
        if(y==max_col-1 || map[x][y+1]!='0') count++;
        if(y==0 || map[x][y-1]!='0') count++;
        if(x==max_row-1 || map[x+1][y]!='0') count++;
        if(x==0 || map[x-1][y]!='0') count++;
        if(count>2) return true;
    }
    return false;
}

/*bool isolated(char** map, int x, int y) {
    int count[4] = {0};
    for(int i=0; i<4; i++){
        switch(i){
            case 0: y++; break;
            case 1: y-=2; break;
            case 2: y++, x--; break;
            case 3: x+=2; break;
        }
        if(x>=0 && y>=0 && x<max_row && y<max_col && map[x][y]=='0') {
            if(y==max_col-1 || map[x][y+1]!='0') count[i]++;
            if(y==0 || map[x][y-1]!='0') count[i]++;
            if(x==max_row-1 || map[x+1][y]!='0') count[i]++;
            if(x==0 || map[x-1][y]!='0') count[i]++;
            if(count[i]==4) {
                now.x = x, now.y = y;
                return true;
            }
        }
    }
    for(int i=0; i<4; i++) {
        if(count[i]==3) {
            switch(i){
                case 0: now.y++; break;
                case 1: now.y--; break;
                case 2: now.x++; break;
                case 3: now.x--; break;
            }
            return true;
        }
    }
    return false;
}*/

bool check(int i, int j, unsigned int steps, Point *queue, char** map, int** minStep)
{
    if(i>=0 && j>=0 && i<max_row && j<max_col) {
        if(!minStep[i][j] && (i!=queue[0].x || j!=queue[0].y)) {
            if(map[i][j]=='1'||map[i][j]=='R') minStep[i][j] = -1;
            else {
                minStep[i][j] = steps;
                queue[current].x = i;
                queue[current].y = j;
                if(map[i][j]=='0') return true;
                current++;
            }
        }
    }
    return false;
}

Point *constructPath(int** minStep, Point p) {
    Point *pathTo0 = new Point[minStep[p.x][p.y]+1];
    while(minStep[p.x][p.y]) {
        pathTo0[minStep[p.x][p.y]].x = p.x;
        pathTo0[minStep[p.x][p.y]].y = p.y;
        if(p.y>0 && minStep[p.x][p.y-1]==minStep[p.x][p.y]-1) p.y--;
        else if(p.y<max_col-1 && minStep[p.x][p.y+1]==minStep[p.x][p.y]-1) p.y++;
        else if(p.x>0 && minStep[p.x-1][p.y]==minStep[p.x][p.y]-1) p.x--;
        else if(p.x<max_row-1 && minStep[p.x+1][p.y]==minStep[p.x][p.y]-1) p.x++;
    }
    return pathTo0;
}

int findNext(char** map, int** minStep, short** port_id) {
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

    /* Build map of distance from queue[0] using BFS */
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
        /* Not enough battery to next 0 */
        if(Battery<steps && minStep[now.x][now.y]!=1) {         
            delete[] queue;
            delete[] miniStep;
            return 0;
        }
    } 
    /* Not enough battery to go back */
    if(Battery+1 < steps + minStep[queue[current].x][queue[current].y]) { 
        delete[] queue;
        delete[] miniStep;
        if(minStep[now.x][now.y]==1) return port_id[queue[current].x][queue[current].y];
        else return 0;
    }
    int totalStep = miniStep[queue[current].x][queue[current].y];
    Point *pathTo0 = constructPath(miniStep, queue[current]);
    for(int i=1; i<=totalStep; i++) {
        now.x = pathTo0[i].x;
        now.y = pathTo0[i].y;
        if(map[now.x][now.y]=='0') total_zero--;
        map[now.x][now.y] = '2';
        walk(map);
    }
    delete[] queue;
    delete[] miniStep;
    delete[] pathTo0;
    return -1;
}

void mark(int i, int j, unsigned int steps, Point *queue, char** map, int** minStep, short** port_id, short port)
{
    if(i>=0 && j>=0 && i<max_row && j<max_col) {
        if(map[i][j]=='0' && !minStep[i][j]) {
            minStep[i][j] = steps;
            queue[current].x = i;
            queue[current].y = j;
            if(i>0 && port_id[i-1][j]==-1) port_id[i][j] = 1;
            else if(j>0 && port_id[i][j-1]==-1) port_id[i][j] = 2;
            else if(i<max_row-1 && port_id[i+1][j]==-1) port_id[i][j] = 3;
            else if(j<max_col-1 && port_id[i][j+1]==-1) port_id[i][j] = 4;
            else port_id[i][j] = port;
            current++;
        }
        else if(map[i][j]=='1') {
            minStep[i][j] = -1;
            port_id[i][j] = 0;
        }
    }
}

void countSteps(Point start, char** map, int** minStep, short** port_id, bool init) {
    Point* queue = new Point[max_row*max_col];
    queue[0].x = start.x;
    queue[0].y = start.y;
    if(start.x>0) port_id[start.x-1][start.y] = 1;
    if(start.y>0) port_id[start.x][start.y-1] = 2;
    if(start.x<max_row-1) port_id[start.x+1][start.y] = 3;
    if(start.y<max_col-1) port_id[start.x][start.y+1] = 4;
    port_id[start.x][start.y] = -1;
    unsigned long read = 0, endd = 0, steps = 0;
    for(steps=1; current!=endd; ++steps){
        endd = current;
        for(; read<endd; read++) {
            mark(queue[read].x+1, queue[read].y, steps, queue, map, minStep, port_id, port_id[queue[read].x][queue[read].y]);
            mark(queue[read].x-1, queue[read].y, steps, queue, map, minStep, port_id, port_id[queue[read].x][queue[read].y]);
            mark(queue[read].x, queue[read].y+1, steps, queue, map, minStep, port_id, port_id[queue[read].x][queue[read].y]);
            mark(queue[read].x, queue[read].y-1, steps, queue, map, minStep, port_id, port_id[queue[read].x][queue[read].y]);
        }
    }
    max_step = steps;
    delete[] queue;
}

bool findPort(short nextPort, int i, int j, unsigned int steps, Point *queue, char** map, int** minStep, short** port_id)
{
    if(i>=0 && j>=0 && i<max_row && j<max_col) {
        if(!minStep[i][j] && (i!=queue[0].x || j!=queue[0].y)) {
            if(map[i][j]=='1'||map[i][j]=='R') minStep[i][j] = -1;
            else {
                minStep[i][j] = steps;
                queue[current].x = i;
                queue[current].y = j;
                if(port_id[i][j]==nextPort && minStep[i][j]+steps-1<=Battery) return true;
                current++;
            }
        }
    }
    return false;
}

bool gotoPort(short nextPort, char** map, int** minStep, short** port_id) {
    current = 1;
    bool found = false;
    visited[port_id[now.x][now.y]] = 1;
    visited[nextPort] = 1;
    Point* queue;
    int** miniStep ;
    while(!found) {
        queue = new Point[max_row*max_col];
        miniStep = new int*[max_row];
        for(int i=0; i<max_row; i++) {
            miniStep[i] = new int[max_col];
            for(int j=0; j<max_col; j++) 
                miniStep[i][j] = 0;
        }
        queue[0].x = now.x;
        queue[0].y = now.y;
        unsigned int read = 0, endd = 0, steps = 0;
        for(steps=1; current!=endd && !found; ++steps){
            endd = current;
            for(; read<endd && !found; read++) {
                if(findPort(nextPort, queue[read].x+1, queue[read].y, steps, queue, map, miniStep, port_id)) found = true;
                else if(findPort(nextPort, queue[read].x-1, queue[read].y, steps, queue, map, miniStep, port_id)) found = true;
                else if(findPort(nextPort, queue[read].x, queue[read].y+1, steps, queue, map, miniStep, port_id)) found = true;
                else if(findPort(nextPort, queue[read].x, queue[read].y-1, steps, queue, map, miniStep, port_id)) found = true;
            }
            if(Battery<steps) {         
                delete[] queue;
                delete[] miniStep;
                return false;
            }
        }
        if(found && Battery+1 < steps + minStep[queue[current].x][queue[current].y]) { 
            delete[] queue;
            delete[] miniStep;
            return false;
        }
    }
    int totalStep = miniStep[queue[current].x][queue[current].y];
    Point *pathTo0 = constructPath(miniStep, queue[current]);

    for(int i=1; i<=totalStep; i++) {
        now.x = pathTo0[i].x;
        now.y = pathTo0[i].y;
        if(map[now.x][now.y]=='0') total_zero--;
        map[now.x][now.y] = '2';
        walk(map);
    }
    delete[] queue;
    delete[] miniStep;
    delete[] pathTo0;
    return true;
}

void charge(char** map, int back, int** minStep, short** port_id) {
    unsigned step = minStep[now.x][now.y];
    //Point pathToCharge[step+1];
    int direction;
    while(minStep[now.x][now.y]) {
        //pathToCharge[minStep[now.x][now.y]].x = now.x;
        //pathToCharge[minStep[now.x][now.y]].y = now.y;
        if     (now.y>0 &&          minStep[now.x][now.y-1]==minStep[now.x][now.y]-1 && map[now.x][now.y-1]=='0') now.y--;
        else if(now.y<max_col-1 &&  minStep[now.x][now.y+1]==minStep[now.x][now.y]-1 && map[now.x][now.y+1]=='0') now.y++;
        else if(now.x>0 &&          minStep[now.x-1][now.y]==minStep[now.x][now.y]-1 && map[now.x-1][now.y]=='0') now.x--;
        else if(now.x<max_row-1 &&  minStep[now.x+1][now.y]==minStep[now.x][now.y]-1 && map[now.x+1][now.y]=='0') now.x++;
        else if(now.y>0 &&          minStep[now.x][now.y-1]==minStep[now.x][now.y]-1) now.y--, direction = 0;
        else if(now.y<max_col-1 &&  minStep[now.x][now.y+1]==minStep[now.x][now.y]-1) now.y++, direction = 1;
        else if(now.x>0 &&          minStep[now.x-1][now.y]==minStep[now.x][now.y]-1) now.x--, direction = 2;
        else if(now.x<max_row-1 &&  minStep[now.x+1][now.y]==minStep[now.x][now.y]-1) now.x++, direction = 3;
        if(map[now.x][now.y] == '0') {
            total_zero--;
            map[now.x][now.y] = '2';
        }
        walk(map);
    }
    Battery = max_Battery;
    if(back && total_zero) {
        switch(direction) {
            case 0: now.y++; break;
            case 1: now.y--; break;
            case 2: now.x++; break;
            case 3: now.x--; break;
        }
        walk(map);
        int nextPort = findNext(map, minStep, port_id);
        if(nextPort!=-1) {
            if(!gotoPort(nextPort, map, minStep, port_id)) {
                int i;
                for(i=1; i<5; i++) visited[i] = 0;
                visited[port_id[now.x][now.y]] = 1;
                visited[nextPort] = 1;
                for(i=1; i<5; visited[i]=1,i++) {
                    if(!visited[i]) {
                        if(gotoPort(i, map, minStep, port_id)) break;
                    }
                }
            }
            charge(map, 1, minStep, port_id);
        }
    }
}

void walk(char** map) {
    Battery--;
    path.push_back(pt(now.x, now.y));
    return;
}
