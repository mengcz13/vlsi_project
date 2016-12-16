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

	DynamicMST dynamicMST;

	int originMstL;
	std::vector<std::pair<Point, Point>> originMst;

	int steinerMstL;
	std::vector<std::pair<Point, Point>> steinerMst;

	Batch1Steiner(const std::vector<Point>& points);

	void route();

	void printSolution(std::ostream& os);
};

#endif