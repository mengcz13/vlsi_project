#pragma once
#ifndef BATCH_1_STEINER
#define BATCH_1_STEINER

#include "utils.h"
#include <vector>
#include <list>
#include <iostream>
#include <fstream>
#include <map>

struct HananInfo {
	Point p;
	int diff;

	HananInfo() {}

	HananInfo(const Point& p, const int& diff) : p(p), diff(diff) {}

	bool operator < (const HananInfo& hi2) const {
		return diff > hi2.diff;
	}
};

struct Batch1Steiner {
	std::vector<Point> targetPs;
	std::vector<Point> hananPs;

	int originMstL;
	std::vector<std::pair<Point, Point>> originMst;

	int steinerMstL;
	std::vector<std::pair<Point, Point>> steinerMst;

	Batch1Steiner(const std::vector<Point>& points);

	double timing;

	virtual void routeWithTiming() {
		LARGE_INTEGER st, et, freq;
		QueryPerformanceFrequency(&freq);
		QueryPerformanceCounter(&st);
		route();
		QueryPerformanceCounter(&et);
		timing = (et.QuadPart - st.QuadPart) * 1000.0 / freq.QuadPart;
	}

	virtual void route();

	void printSolution(std::ostream& os);
};

// 使用dynamic MST 加速
struct Batch1SteinerAcc : public Batch1Steiner {
	DynamicMST dynamicMST;

	Batch1SteinerAcc(const std::vector<Point>& points) : Batch1Steiner(points) {
		dynamicMST = DynamicMST(originMst, originMstL);
	}

	virtual void route();
};

// struct for IkS method
struct HananSetInfo {
	std::set<Point> ps;
	int diff;

	HananSetInfo() {}

	HananSetInfo(const std::set<Point> ps, const int& diff) : ps(ps), diff(diff) {}

	bool operator < (const HananInfo& hi2) const {
		return diff > hi2.diff;
	}
};

// I2S
struct Iterated2Steiner : public Batch1SteinerAcc {
	Iterated2Steiner(const std::vector<Point>& points) : Batch1SteinerAcc(points) {}

	virtual void route();

	virtual HananSetInfo choose(const std::vector<HananSetInfo>& leftHanan) const;
};

// I2S with softmax
struct EnhancedIterated2Steiner : public Iterated2Steiner {
	EnhancedIterated2Steiner(const std::vector<Point>& points) : Iterated2Steiner(points) {}

	virtual HananSetInfo choose(const std::vector<HananSetInfo>& leftHanan) const;
};

#endif