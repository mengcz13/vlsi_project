#include <iostream>
#include <fstream>
#include <random>
#include <ctime>
#include "utils.h"
#include "pso.h"
#include "batch_1_steiner.h"
using namespace std;

// ����˵��: main.exe <�����ӵĵ���> <X�������ֵ> <Y�������ֵ> <������> <C1> <C2>

int main(int argc, char** argv) {
	int N = 20;
	int MAXX = 10000;
	int MAXY = 10000;
	int PN = 50;
	double C1 = 0.5;
	double C2 = 0.9;
	int maxIterTime = 10000;
	
	if (argc < 12) {
		if (argc == 2) {
			// just timing bs and bsa
			vector<Point> points = vector<Point>();
			uniform_int_distribution<> disx(0, MAXX);
			uniform_int_distribution<> disy(0, MAXY);
			N = atoi(argv[1]);
			for (int i = 0; i < N; ++i)
				points.push_back(Point(disx(GEN), disy(GEN)));
			ofstream f0("timingtemp.txt");

			Batch1Steiner batch1Steiner(points);
			batch1Steiner.routeWithTiming();
			batch1Steiner.printSolution(f0);

			Batch1SteinerAcc batch1SteinerAcc(points);
			batch1SteinerAcc.routeWithTiming();
			batch1SteinerAcc.printSolution(f0);

			return 0;
		}
		cout << "����˵��: main.exe <�����ӵĵ���> <X�������ֵ> <Y�������ֵ> <������> <C1> <C2> <��������> <����ļ�0> <����ļ�1> <����ļ�2> <����ļ�3>" << endl;
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
	ofstream f1(argv[9]);
	ofstream f2(argv[10]);
	ofstream f3(argv[11]);

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
	
	Router router(points, PN, C1, C2);
	router.MAXX = MAXX;
	router.MAXY = MAXY;
	router.iterTime = maxIterTime;
	router.routeWithTiming();
	router.printSolution(f0);
	
	/*
	Batch1Steiner batch1Steiner(points);
	batch1Steiner.routeWithTiming();
	batch1Steiner.printSolution(f1);
	*/

	Batch1SteinerAcc batch1SteinerAcc(points);
	batch1SteinerAcc.routeWithTiming();
	batch1SteinerAcc.printSolution(f1);

	Iterated2Steiner iterated2Steiner(points);
	iterated2Steiner.routeWithTiming();
	iterated2Steiner.printSolution(f2);

	EnhancedIterated2Steiner enhancedIterated2Steiner(points);
	enhancedIterated2Steiner.routeWithTiming();
	enhancedIterated2Steiner.printSolution(f3);
	return 0;
}