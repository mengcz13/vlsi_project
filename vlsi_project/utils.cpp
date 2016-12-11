#include "utils.h"
#include <algorithm>
using namespace std;

// Prim algorithm
vector<Point> mstMhtDist(const vector<Point>& ps, int& dist) {
	vector<int> score = vector<int>(ps.size(), INT_MAX);
	int next = 0;
	score.at(next) = 0;
	int pnum = ps.size();
	dist = 0;
	for (int i = 0; i < pnum; ++i) {
		dist += score.at(next);
		score.at(next) = 0;
		int minscore = INT_MAX;
		for (int j = 0; j < pnum; ++j) {
			score.at(j) = min(score.at(j), mhtDist(ps.at(next), ps.at(j)));
			if (score.at(j) > 0 && score.at(j) < minscore) {
				minscore = score.at(j);
				next = j;
			}
		}
	}
	return vector<Point>();
}