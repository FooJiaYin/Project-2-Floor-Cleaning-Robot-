#include <iostream>
#include <fstream>
#include <string>
#include <vector>
//#include "stdafx.h"
#include "stdio.h"
#include "stdlib.h"
#include "windows.h"
using namespace std;

typedef struct
{
	int x;
	int y;
} Point;

unsigned int total_zero=1;
unsigned long errorCount = 0;
void error(int error_code, int line, int x, int y);
void show(char**);

int main (int argc, char* argv[]) {
    string dir = argv[1], dir_test, dir_ans;
    dir_test = "test_case/" + dir + "/floor.data";
    dir_ans = "test_case/" + dir + "/final.path";
    ifstream testcase(dir_test);
    ifstream ans(dir_ans);

    if(testcase.is_open() && ans.is_open()) {
        int max_row, max_col;
        long int Battery, max_Battery;
        long int total_zero;
        testcase >> max_row >> max_col >> max_Battery;
        Point start, now;

        /* Read the map */
        char** map = new char*[max_row];
        for(int i=0; i<max_row; i++) {
            map[i] = new char[max_col];
            for(int j=0; j<max_col; j++) {
                do {
                    testcase >> map[i][j];
                } while(map[i][j]==' '|| map[i][j]=='\n');
                if(map[i][j]=='R') {
                    start.x = i;
                    start.y = j;
                }
            }
        }

        unsigned long steps;
        ans >> steps;
        Battery = max_Battery;
        for(unsigned long int i=1; i<=steps; i++) {
            Point tempStep = now;
            ans >> now.x >> now.y;
            Battery--;
            if(map[now.x][now.y]=='0') 
                map[now.x][now.y]='2';
            else if(map[now.x][now.y]=='R') Battery = max_Battery;
            else if(map[now.x][now.y]=='1') error(1, i, now.x, now.y);
            if(now.x==tempStep.x && now.y==tempStep.y) error(2, i, now.x, now.y);
            else if(now.x==tempStep.x+1 && now.y==tempStep.y);
            else if(now.x==tempStep.x-1 && now.y==tempStep.y);
            else if(now.x==tempStep.x && now.y==tempStep.y+1);
            else if(now.x==tempStep.x && now.y==tempStep.y-1);
            else if(i>1) error(3, i, now.x, now.y);
            if(Battery<0) error(4, i, now.x, now.y);
            //show(map);
            /*int k, j;
            total_zero = 0;
            system("CLS");
            printf("Battery=%d\n", Battery);
            for (k = 0; k < max_row; k++)
            {
                for (j = 0; j < max_col; j++) {
                    if(k==now.x && j==now.y) printf("0 ");
                    //else if(map[k][j]=='0') total_zero++;
                    else switch (map[k][j])
                    {
                        case '0': total_zero++; printf("* "); break;
                        case '1': printf("X "); break;
                        case '2': printf("  "); break;
                        case 'R': printf("R "); break;
                    }
                }
                printf("\n");
            }
            printf("%u\n", total_zero);*/
            //cout << "pt" <<' ' << now.x << ' ' << now.y << endl;
            //Sleep(0.01);
        }

        for(int i=0; i<max_row; i++) 
            for(int j=0; j<max_col; j++) 
                if(map[i][j]==0) error(5, 0, i, j);
        
        /*for(int i=0; i<max_row; i++) {
            for(int j=0; j<max_col; j++) 
                cout << map[i][j] << ' ';
            cout << endl;
        }*/
        
        cout << "Error Count: " << errorCount << endl;

        testcase.close();
        ans.close();
    }

    //system("Pause");
    return 0;
}

void error(int error_code, int line, int x, int y) {
    printf("line %5d (%3d, %3d) ", line, x, y);
    switch(error_code) {
        case 1: cout << "Error: Step into 1\n"; break;
        case 2: cout << "Warning: Redundant step\n"; break;
        case 3: cout << "Error: Step skipped\n"; break;
        case 4: cout << "Error: Battery < 0\n"; break;
        case 5: cout << "Error: Area not cleared\n"; break;
    }
    errorCount++;
}

/*void show(char** map) {
	int i, j;
    total_zero = 0;
	system("CLS");
	printf("Battery=%d\n", Battery);
	for (i = 0; i < max_row; i++)
	{
		for (j = 0; j < max_col; j++) {
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
    if(total_zero==0) endd.x = now.x,  endd.y = now.y;
    printf("%u\n", total_zero);
    //cout << "pt" <<' ' << now.x << ' ' << now.y << endl;
    path.push_back(pt(now.x, now.y));
	//Sleep(0.01);
    return;
}*/