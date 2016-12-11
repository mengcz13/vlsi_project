#pragma once
#ifndef UTILS_H
#define UTILS_H

#include <vector>
#include <random>
#include <cmath>
#include <set>
#include <algorithm>

// ���
struct Point {
	int x;
	int y;

	Point() : x(0), y(0) {}

	Point(int x, int y) : x(x), y(y) {}

	bool operator < (const Point& p2) const {
		if (x < p2.x)
			return true;
		else if (x == p2.x && y < p2.y)
			return true;
		return false;
	}
};

// �����پ���
inline int mhtDist(const Point& p1, const Point& p2) {
	return abs(p1.x - p2.x) + abs(p1.y - p2.y);
}

// �����پ����µ���С������
std::vector<Point> mstMhtDist(const std::vector<Point>& ps, int& dist);

#endif