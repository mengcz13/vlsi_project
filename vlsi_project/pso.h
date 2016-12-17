#pragma once
#ifndef PSO_H
#define PSO_H

#include "utils.h"

extern std::mt19937 GEN;
extern std::uniform_real_distribution<> DIS01;

// 二维坐标
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

// 记录最优解类
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

// 粒子
struct Particle {
	int num;					// 前num个点参加计算
	std::vector<Pos> posvec;	// 所有点的坐标
	std::vector<Pos> vvec;		// 粒子速度

	ParticleRecord localBest;	// 个体最优解

	Particle() {}

	Particle(const std::vector<Point> ips) {	// ips: n-2个Hanan点, 由router随机shuffle得到
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

// 布线区域
struct Router {
	std::vector<Point> targetPs;	// 需连接的所有点
	std::vector<Point> hananPs;		// Hanan点
	std::vector<Point> allPs;		// targetPs + hananPs
	std::set<double> xs;
	std::set<double> ys;
	std::vector<Particle> particles;	// 种群

	int PNUM;	// 种群大小
	double C1, C2;	// 学习因子
	double W;		// 惯性系数
	int iterTime;	// 迭代次数

	int L;		// 目标点的MST长度, 为生成线网的总长度上限

	ParticleRecord globalBest;	// 全局最优解

	double sigma;				// 粒子分散度

	int MAXX;					// x坐标最大值
	int MAXY;					// y坐标最大值

	std::vector<std::pair<Point, Point>> originMst;
	std::vector<std::pair<Point, Point>> steinerMst;

	Router(const std::vector<Point>& points, int pnum, double c1, double c2) {
		PNUM = pnum;
		C1 = c1;
		C2 = c2;
		targetPs = points;
		genHananPs();
	}

	void genHananPs();		// 生成Hanan点

	void route();			// 布线函数

	double timing;			// timing (ms)

	void routeWithTiming() {
		LARGE_INTEGER st, et, freq;
		QueryPerformanceFrequency(&freq);
		QueryPerformanceCounter(&st);
		route();
		QueryPerformanceCounter(&et);
		timing = (et.QuadPart - st.QuadPart) * 1000.0 / freq.QuadPart;
	}

	void initParticles();	// 初始化粒子群

	void moveParticles();	// 更新粒子群

	void evalParticles();	// 对每个粒子求fitness, 更新个体最优解, 全局最优解

	int calcFitness(const Particle& particle) const;

	std::vector<std::pair<Point, Point>> getGlobalBestMst() const;

	void printSolution(std::ostream& os) const;
};

#endif