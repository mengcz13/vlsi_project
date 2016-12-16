#include "utils.h"
#include <algorithm>
#include <deque>
#include <cassert>
#include <iostream>
using namespace std;

// Prim algorithm
vector<pair<Point, Point>> mstMhtDist(const vector<Point>& ps, int& dist) {
	vector<pair<Point, Point>> res = vector<pair<Point, Point>>();
	res.reserve(ps.size() - 1);
	vector<int> score = vector<int>(ps.size(), INT_MAX);
	vector<int> last_source = vector<int>(ps.size(), 0);
	int next = 0;
	score.at(next) = 0;
	int pnum = ps.size();
	dist = 0;
	for (int i = 0; i < pnum; ++i) {
		//dist += score.at(next);
		score.at(next) = 0;
		int last_next = next;
		int minscore = INT_MAX;
		for (int j = 0; j < pnum; ++j) {
			int dist = mhtDist(ps.at(next), ps.at(j));
			if (score.at(j) > dist) {
				score.at(j) = dist;
				last_source.at(j) = next;
			}
		}
		for (int j = 0; j < pnum; ++j) {
			if (score.at(j) > 0 && score.at(j) < minscore) {
				minscore = score.at(j);
				next = j;
			}
		}
		if (i < pnum - 1) {
			res.push_back(make_pair(ps.at(last_source.at(next)), ps.at(next)));
			dist += minscore;
		}
	}
	return res;
}

DynamicMST::DynamicMST(const vector<pair<Point, Point>>& edges, int initMstValue) {
	set<Point> pointsSet;
	mstValue = initMstValue;
	for (auto iter = edges.begin(); iter != edges.end(); ++iter) {
		addEdge(*iter);
		pointsSet.insert(iter->first);
		pointsSet.insert(iter->second);
	}
	points = vector<Point>(pointsSet.begin(), pointsSet.end());
}

void DynamicMST::addPoint(const Point& np) {
	adjs.insert(make_pair(np, set<Point>()));
	int mindst[4] = { INT_MAX, INT_MAX, INT_MAX, INT_MAX };
	Point candip[4];
	for (auto iter = points.begin(); iter != points.end(); ++iter) {
		int region = getRegion(np, *iter);
		int dst = mhtDist(np, *iter);
		if (mindst[region] > dst) {
			mindst[region] = dst;
			candip[region] = (*iter);
		}
	}

	bool firsthit = false;

	// 第一次连接必然无环
	/*
	if (mindst[0] != INT_MAX) {
		addEdge(make_pair(np, candip[0]));
		mstValue += mindst[0];
	}
	*/

	for (int i = 0; i < 4; ++i) {
		if (mindst[i] != INT_MAX) {
			addEdge(make_pair(np, candip[i]));
			mstValue += mindst[i];
			if (!firsthit)
				firsthit = true;
			else
				// 此时需要去掉产生的环路中最长边
				removeLoop(np);
		}
	}

	points.push_back(np);
}

void DynamicMST::addEdge(const std::pair<Point, Point>& edge) {
	auto it = adjs.find(edge.first);
	if (it == adjs.end()) {
		set<Point> s;
		s.insert(edge.second);
		adjs.insert(make_pair(edge.first, s));
	}
	else {
		it->second.insert(edge.second);
	}
	it = adjs.find(edge.second);
	if (it == adjs.end()) {
		set<Point> s;
		s.insert(edge.first);
		adjs.insert(make_pair(edge.second, s));
	}
	else {
		it->second.insert(edge.first);
	}
}

void DynamicMST::removeEdge(const std::pair<Point, Point>& edge) {
	adjs.find(edge.first)->second.erase(edge.second);
	adjs.find(edge.second)->second.erase(edge.first);
}

void DynamicMST::removeLoop(const Point& np) {
	map<Point, Point> pointWithParent;	// 记录BFS点及其父亲
	deque<Point> bfsQueue;	// BFS Queue
	bfsQueue.push_back(np);
	pointWithParent.insert(make_pair(np, Point(INT_MIN, INT_MIN)));
	bool hasLoop = false;
	Point onLoop[2];
	while (!bfsQueue.empty() && !hasLoop) {
		Point curr = bfsQueue.front();
		bfsQueue.pop_front();
		const set<Point>& nextps = adjs.find(curr)->second;
		for (auto iter = nextps.begin(); iter != nextps.end(); ++iter) {
			auto testiter = pointWithParent.find(*iter);
			if (testiter == pointWithParent.end()) {
				bfsQueue.push_back(*iter);
				pointWithParent.insert(make_pair(*iter, curr));
			}
			else {
				Point parent = pointWithParent.find(curr)->second;
				if (!(testiter->first == parent)) {
					hasLoop = true;
					onLoop[0] = testiter->first;
					onLoop[1] = curr;
					break;
				}
			}
		}
	}
	vector<pair<Point, Point>> loop;
	if (hasLoop) {
		loop.push_back(make_pair(onLoop[0], onLoop[1]));
		for (int i = 0; i < 2; ++i) {
			auto iter = pointWithParent.find(onLoop[i]);
			while (1) {
				loop.push_back(*iter);
				if (iter->second == np)
					break;
				else
					iter = pointWithParent.find(iter->second);
			}
		}
		int maxdst = INT_MIN;
		auto maxiter = loop.begin();
		for (auto loopiter = loop.begin(); loopiter != loop.end(); ++loopiter) {
			int dst = mhtDist(loopiter->first, loopiter->second);
			if (dst > maxdst) {
				maxdst = dst;
				maxiter = loopiter;
			}
		}
		removeEdge(*maxiter);
		mstValue -= maxdst;
	}
}