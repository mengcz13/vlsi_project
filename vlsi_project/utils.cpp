#include "utils.h"
#include <algorithm>
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