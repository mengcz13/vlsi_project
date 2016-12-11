#include "utils.h"
#include "pso.h"
#include <algorithm>
#include <cmath>
#include <iostream>
using namespace std;

void Router::genHananPs() {
	std::set<Point> tPSet = std::set<Point>(targetPs.begin(), targetPs.end());
	std::set<Point> hPSet = std::set<Point>();
	for (auto iterx = targetPs.begin(); iterx != targetPs.end(); ++iterx)
		for (auto itery = targetPs.begin(); itery != targetPs.end(); ++itery) {
			Point tp = Point(iterx->x, itery->y);
			if (tPSet.find(tp) == tPSet.end())
				hPSet.insert(tp);
		}
	hananPs = std::vector<Point>(hPSet.begin(), hPSet.end());
	hPSet.insert(targetPs.begin(), targetPs.end());
	allPs = std::vector<Point>(hPSet.begin(), hPSet.end());
}

void Router::route() {
	initParticles();
	for (int i = 0; i < 10000; ++i) {
		W = 0.9 - i / 10000.0 * 0.5;
		moveParticles();
		evalParticles();
		if (i % 1000 == 0)
			cout << "Iter: " << i << " , globalBest: " << globalBest.fitness 
				<< ", L: " << L 
				<< ", sigma: " << sigma
				<< endl;
	}
}

void Router::initParticles() {
	globalBest.fitness = INT_MIN;
	// 计算目标点的MST
	mstMhtDist(targetPs, L);
	// 生成PNUM个粒子
	for (int i = 0; i < PNUM; ++i) {
		shuffle(hananPs.begin(), hananPs.end(), GEN);
		auto end = hananPs.end();
		--end;
		--end;
		particles.push_back(Particle(vector<Point>(hananPs.begin(), end)));
	}
	// 第一次evaluate
	evalParticles();
}

void Router::moveParticles() {
	// 更新粒子num
	for (int i = 0; i < PNUM; ++i) {
		Particle& particle = particles.at(i);
		particle.num = max(particle.num, max(particle.localBest.num, globalBest.num));
	}
	// 计算分布方差
	vector<Pos> average = particles.at(0).posvec;
	for (int i = 1; i < PNUM; ++i) {
		for (int j = 0; j < average.size(); ++j) {
			average.at(j) = average.at(j) + particles.at(i).posvec.at(j);
		}
	}
	for (int i = 0; i < average.size(); ++i) {
		average.at(i).x /= PNUM;
		average.at(i).y /= PNUM;
	}
	sigma = 0;
	for (int i = 0; i < PNUM; ++i) {
		for (int j = 0; j < average.size(); ++j) {
			Pos dp = average.at(j) - particles.at(i).posvec.at(j);
			sigma += (dp.x * dp.x + dp.y * dp.y);
		}
	}
	sigma /= PNUM;
	//if (sigma > 2)
	//	return;
	// 更新粒子位置
	//W = 1.0 / (1.0 + 10.0 / sigma * exp(-sigma * sigma));
	for (int i = 0; i < PNUM; ++i) {
		// 更新
		Particle& particle = particles.at(i);
		vector<Pos> newv = particle.vvec;
		vector<Pos> newx = particle.posvec;
		int len = particle.maxnum();
		double r1 = DIS01(GEN), r2 = DIS01(GEN);
		// formula
		for (int j = 0; j < len; ++j) {
			newv.at(j).x = W * particle.vvec.at(j).x + C1 * r1 * (particle.localBest.posvec.at(j).x - particle.posvec.at(j).x) + C2 * r2 * (globalBest.posvec.at(j).x - particle.posvec.at(j).x);
			newv.at(j).y = W * particle.vvec.at(j).y + C1 * r1 * (particle.localBest.posvec.at(j).y - particle.posvec.at(j).y) + C2 * r2 * (globalBest.posvec.at(j).y - particle.posvec.at(j).y);
			newx.at(j).x = particle.posvec.at(j).x + particle.vvec.at(j).x;
			newx.at(j).x = max(0., newx.at(j).x);
			newx.at(j).x = min((double)MAXX, newx.at(j).x);
			newx.at(j).y = particle.posvec.at(j).y + particle.vvec.at(j).y;
			newx.at(j).y = max(0., newx.at(j).y);
			newx.at(j).y = min((double)MAXY, newx.at(j).y);
		}
		particle.posvec = newx;
		particle.vvec = newv;
	}
}

void Router::evalParticles() {
	for (int i = 0; i < PNUM; ++i) {
		Particle& particle = particles.at(i);
		int fitness = calcFitness(particle);
		ParticleRecord rec = ParticleRecord(particle.num, particle.posvec, fitness);
		if (particle.localBest.fitness < fitness) {
			particle.localBest = rec;
		}
		if (globalBest.fitness < fitness) {
			globalBest = rec;
		}
	}
}

int Router::calcFitness(const Particle& particle) const {
	// 生成候选点集
	vector<Point> ps = targetPs;
	for (int i = 0; i < particle.num; ++i) {
		Point hp = Point();
		double mind = INFINITY;
		Pos p = particle.posvec.at(i);
		for (auto h = allPs.begin(); h != allPs.end(); ++h) {
			double dx = h->x - p.x;
			double dy = h->y - p.y;
			double dst = dx * dx + dy * dy;
			if (dst < mind) {
				mind = dst;
				hp = *h;
			}
		}
		ps.push_back(hp);
	}
	set<Point> pset = set<Point>(ps.begin(), ps.end());
	ps = vector<Point>(pset.begin(), pset.end());
	// MST
	int mstdist = 0;
	mstMhtDist(ps, mstdist);
	return L - mstdist;
}