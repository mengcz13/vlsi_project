#pragma once
#ifndef UTILS_H
#define UTILS_H

#include <vector>
#include <random>
#include <cmath>
#include <set>
#include <map>
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

	bool operator == (const Point& p2) const {
		return (x == p2.x && y == p2.y);
	}
};

// �����پ���
inline int mhtDist(const Point& p1, const Point& p2) {
	return abs(p1.x - p2.x) + abs(p1.y - p2.y);
}

// �����پ����µ���С������
std::vector<std::pair<Point, Point>> mstMhtDist(const std::vector<Point>& ps, int& dist);

// ��̬ά����С������(����), ֧�ֵ������
struct DynamicMST {
	int mstValue;
	std::map<Point, std::set<Point>> adjs;
	std::vector<Point> points;

	DynamicMST() {}

	DynamicMST(const std::vector<std::pair<Point, Point>>& edges, int initMstValue);

	void addPoint(const Point& np);

	// ������ά��adjs
	void addEdge(const std::pair<Point, Point>& edge);

	// ������ά��adjs, �豣֤��ɾ����һ������
	void removeEdge(const std::pair<Point, Point>& edge);

	int getRegion(const Point& p1, const Point& p2) const {
		int dx = p2.x - p1.x;
		int dy = p2.y - p1.y;
		int absdx = abs(dx);
		int absdy = abs(dy);
		if (absdx == absdy) {
			if (dx > 0 && dy < 0)
				return 0;
			else if (dx > 0 && dy > 0)
				return 1;
			else if (dx < 0 && dy > 0)
				return 2;
			else
				return 3;
		}
		else {
			if (dx > 0 && absdy < absdx)
				return 0;
			else if (dy > 0 && absdy > absdx)
				return 1;
			else if (dx < 0 && absdy < absdx)
				return 2;
			else
				return 3;
		}
	}

	// ��np�����, BFS�һ�·���������
	void removeLoop(const Point& np);
};

#endif