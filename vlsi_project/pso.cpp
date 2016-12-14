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
	xs.clear();
	ys.clear();
	for (auto iter = targetPs.begin(); iter != targetPs.end(); ++iter) {
		xs.insert((double)(iter->x));
		ys.insert((double)(iter->y));
	}
}

void Router::route() {
	initParticles();
	int maxIter = iterTime;
	for (int i = 0; i < maxIter; ++i) {
		W = 0.9 - (double)i / (double)maxIter * 0.5;
		moveParticles();
		evalParticles();
		if (i % 1000 == 0)
			cout << "Iter: " << i << " , globalBest: " << globalBest.fitness 
				<< ", L: " << L 
				//<< ", sigma: " << sigma
				<< endl;
	}
	steinerMst = getGlobalBestMst();
}

void Router::initParticles() {
	globalBest.fitness = INT_MIN;
	// 计算目标点的MST
	originMst = mstMhtDist(targetPs, L);
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
	/*
	sigma = 0;
	for (int i = 0; i < PNUM; ++i) {
		for (int j = 0; j < average.size(); ++j) {
			Pos dp = average.at(j) - particles.at(i).posvec.at(j);
			sigma += (dp.x * dp.x + dp.y * dp.y);
		}
	}
	sigma /= PNUM;*/
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
		/*
		for (auto h = allPs.begin(); h != allPs.end(); ++h) {
			double dx = h->x - p.x;
			double dy = h->y - p.y;
			double dst = dx * dx + dy * dy;
			if (dst < mind) {
				mind = dst;
				hp = *h;
			}
		}*/
		auto lx = xs.lower_bound(p.x);
		auto hx = xs.upper_bound(p.x);
		auto ly = ys.lower_bound(p.y);
		auto hy = ys.upper_bound(p.y);
		int lxi = 0, hxi = MAXX, lyi = 0, hyi = MAXY;
		if (lx != xs.end())
			lxi = *lx;
		if (hx != xs.end())
			hxi = *hx;
		if (ly != ys.end())
			lyi = *ly;
		if (hy != ys.end())
			hyi = *hy;
		Point cps[4] = { Point(lxi, lyi), Point(lxi, hyi), Point(hxi, lyi), Point(hxi, hyi) };
		for (int j = 0; j < 4; ++j) {
			double dx = cps[j].x - p.x;
			double dy = cps[j].y - p.y;
			double dst = dx * dx + dy * dy;
			if (dst < mind) {
				mind = dst;
				hp = cps[j];
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

vector<pair<Point, Point>> Router::getGlobalBestMst() const {
	vector<Point> ps = targetPs;
	for (int i = 0; i < globalBest.num; ++i) {
		Point hp = Point();
		double mind = INFINITY;
		Pos p = globalBest.posvec.at(i);
		auto lx = xs.lower_bound(p.x);
		auto hx = xs.upper_bound(p.x);
		auto ly = ys.lower_bound(p.y);
		auto hy = ys.upper_bound(p.y);
		int lxi = 0, hxi = MAXX, lyi = 0, hyi = MAXY;
		if (lx != xs.end())
			lxi = *lx;
		if (hx != xs.end())
			hxi = *hx;
		if (ly != ys.end())
			lyi = *ly;
		if (hy != ys.end())
			hyi = *hy;
		Point cps[4] = { Point(lxi, lyi), Point(lxi, hyi), Point(hxi, lyi), Point(hxi, hyi) };
		for (int j = 0; j < 4; ++j) {
			double dx = cps[j].x - p.x;
			double dy = cps[j].y - p.y;
			double dst = dx * dx + dy * dy;
			if (dst < mind) {
				mind = dst;
				hp = cps[j];
			}
		}

		ps.push_back(hp);
	}
	set<Point> pset = set<Point>(ps.begin(), ps.end());
	ps = vector<Point>(pset.begin(), pset.end());
	// MST
	int mstdist = 0;
	return mstMhtDist(ps, mstdist);
}

void Router::printSolution(ostream& os) const {
	os << "T:" << endl;
	for (auto iter = targetPs.begin(); iter != targetPs.end(); ++iter) {
		os << iter->x << " " << iter->y << endl;
	}
	os << "MST:" << endl;
	for (auto iter = originMst.begin(); iter != originMst.end(); ++iter) {
		os << iter->first.x << " " << iter->first.y << " "
			<< iter->second.x << " " << iter->second.y << endl;
	}
	os << "RMST:" << endl;
	for (auto iter = steinerMst.begin(); iter != steinerMst.end(); ++iter) {
		os << iter->first.x << " " << iter->first.y << " "
			<< iter->second.x << " " << iter->second.y << endl;
	}
}