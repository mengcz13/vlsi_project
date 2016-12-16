#include <iostream>
#include <fstream>
#include <random>
#include <ctime>
#include "utils.h"
#include "pso.h"
#include "batch_1_steiner.h"
using namespace std;

int SEED = time(0);
random_device RD;
mt19937 GEN(RD());
uniform_real_distribution<> DIS01(0, 1);

// 参数说明: main.exe <需连接的点数> <X最大坐标值> <Y最大坐标值> <粒子数> <C1> <C2>

int main(int argc, char** argv) {
	int N = 20;
	int MAXX = 10000;
	int MAXY = 10000;
	int PN = 50;
	double C1 = 0.5;
	double C2 = 0.9;
	int maxIterTime = 10000;
	
	if (argc < 9) {
		cout << "参数说明: main.exe <需连接的点数> <X最大坐标值> <Y最大坐标值> <粒子数> <C1> <C2> <迭代次数> <输出文件>" << endl;
		return -1;
	}

	N = atoi(argv[1]);
	MAXX = atoi(argv[2]);
	MAXY = atoi(argv[3]);
	PN = atoi(argv[4]);
	C1 = atof(argv[5]);
	C2 = atof(argv[6]);
	maxIterTime = atoi(argv[7]);
	ofstream f0(argv[8]);

	vector<Point> points = vector<Point>();
	/*
	points.push_back(Point(0, 0));
	points.push_back(Point(1, 1));
	points.push_back(Point(2, 0));
	*/
	uniform_int_distribution<> disx(0, MAXX);
	uniform_int_distribution<> disy(0, MAXY);
	for (int i = 0; i < N; ++i)
		points.push_back(Point(disx(GEN), disy(GEN)));
	/*
	Router router(points, PN, C1, C2);
	router.MAXX = MAXX;
	router.MAXY = MAXY;
	router.iterTime = maxIterTime;
	router.route();
	router.printSolution(f0);
	*/

	Batch1Steiner batch1Steiner(points);
	batch1Steiner.route();
	batch1Steiner.printSolution(f0);
	return 0;
}