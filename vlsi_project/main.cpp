#include <iostream>
#include <random>
#include <ctime>
#include "utils.h"
#include "pso.h"
using namespace std;

int SEED = time(0);
random_device RD;
mt19937 GEN(RD());
uniform_real_distribution<> DIS01(0, 1);

// ����˵��: main.exe <�����ӵĵ���> <X�������ֵ> <Y�������ֵ> <������> <C1> <C2>

int main(int argc, char** argv) {
	int N = 20;
	int MAXX = 10000;
	int MAXY = 10000;
	int PN = 50;
	double C1 = 0.5;
	double C2 = 0.9;
	
	if (argc < 7) {
		cout << "����˵��: main.exe <�����ӵĵ���> <X�������ֵ> <Y�������ֵ> <������> <C1> <C2>" << endl;
		return -1;
	}

	N = atoi(argv[1]);
	MAXX = atoi(argv[2]);
	MAXY = atoi(argv[3]);
	PN = atoi(argv[4]);
	C1 = atof(argv[5]);
	C2 = atof(argv[6]);

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
	router.route();
	return 0;
}