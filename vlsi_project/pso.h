#pragma once
#ifndef PSO_H
#define PSO_H

#include "utils.h"

extern std::mt19937 GEN;
extern std::uniform_real_distribution<> DIS01;

// ��ά����
struct Pos {
	double x;
	double y;

	Pos() : x(0), y(0) {}

	Pos(double x, double y) : x(x), y(y) {}

	Pos operator + (const Pos& p2) {
		return Pos(x + p2.x, y + p2.y);
	}

	Pos operator - (const Pos& p2) {
		return Pos(x - p2.x, y - p2.y);
	}
};

// ��¼���Ž���
struct ParticleRecord {
	int num;
	std::vector<Pos> posvec;
	int fitness;
	
	ParticleRecord() {}

	ParticleRecord(int num1, std::vector<Pos> posvec1, int fitness1) {
		num = num1;
		posvec = posvec1;
		fitness = fitness1;
	}
};

// ����
struct Particle {
	int num;					// ǰnum����μӼ���
	std::vector<Pos> posvec;	// ���е������
	std::vector<Pos> vvec;		// �����ٶ�

	ParticleRecord localBest;	// �������Ž�

	Particle() {}

	Particle(const std::vector<Point> ips) {	// ips: n-2��Hanan��, ��router���shuffle�õ�
		std::uniform_int_distribution<> dis(1, ips.size());
		num = dis(GEN);
		posvec = std::vector<Pos>(ips.size());
		localBest.fitness = INT_MIN;
		auto it1 = posvec.begin();
		auto it2 = ips.begin();
		for (; it1 != posvec.end() && it2 != ips.end();) {
			it1->x = static_cast<double>(it2->x);
			it1->y = static_cast<double>(it2->y);
			++it1;
			++it2;
		}
		vvec = std::vector<Pos>(posvec.size(), Pos(0, 0));
	}

	int maxnum() { return posvec.size(); }

};

// ��������
struct Router {
	std::vector<Point> targetPs;	// �����ӵ����е�
	std::vector<Point> hananPs;		// Hanan��
	std::vector<Point> allPs;		// targetPs + hananPs
	std::set<double> xs;
	std::set<double> ys;
	std::vector<Particle> particles;	// ��Ⱥ

	int PNUM;	// ��Ⱥ��С
	double C1, C2;	// ѧϰ����
	double W;		// ����ϵ��
	int iterTime;	// ��������

	int L;		// Ŀ����MST����, Ϊ�����������ܳ�������

	ParticleRecord globalBest;	// ȫ�����Ž�

	double sigma;				// ���ӷ�ɢ��

	int MAXX;					// x�������ֵ
	int MAXY;					// y�������ֵ

	std::vector<std::pair<Point, Point>> originMst;
	std::vector<std::pair<Point, Point>> steinerMst;

	Router(const std::vector<Point>& points, int pnum, double c1, double c2) {
		PNUM = pnum;
		C1 = c1;
		C2 = c2;
		targetPs = points;
		genHananPs();
	}

	void genHananPs();		// ����Hanan��

	void route();			// ���ߺ���

	double timing;			// timing (ms)

	void routeWithTiming() {
		LARGE_INTEGER st, et, freq;
		QueryPerformanceFrequency(&freq);
		QueryPerformanceCounter(&st);
		route();
		QueryPerformanceCounter(&et);
		timing = (et.QuadPart - st.QuadPart) * 1000.0 / freq.QuadPart;
	}

	void initParticles();	// ��ʼ������Ⱥ

	void moveParticles();	// ��������Ⱥ

	void evalParticles();	// ��ÿ��������fitness, ���¸������Ž�, ȫ�����Ž�

	int calcFitness(const Particle& particle) const;

	std::vector<std::pair<Point, Point>> getGlobalBestMst() const;

	void printSolution(std::ostream& os) const;
};

#endif