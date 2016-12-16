#include "batch_1_steiner.h"
#include <list>
#include <set>
#include <iostream>
using namespace std;

Batch1Steiner::Batch1Steiner(const std::vector<Point>& points) {
	targetPs = points;
	std::set<Point> tPSet = std::set<Point>(targetPs.begin(), targetPs.end());
	std::set<Point> hPSet = std::set<Point>();
	for (auto iterx = targetPs.begin(); iterx != targetPs.end(); ++iterx)
		for (auto itery = targetPs.begin(); itery != targetPs.end(); ++itery) {
			Point tp = Point(iterx->x, itery->y);
			if (tPSet.find(tp) == tPSet.end())
				hPSet.insert(tp);
		}
	hananPs = std::vector<Point>(hPSet.begin(), hPSet.end());

	originMst = mstMhtDist(targetPs, originMstL);
	dynamicMST = DynamicMST(originMst, originMstL);
}

void Batch1Steiner::route() {
	vector<Point> psPoints = targetPs;
	set<Point> unusedHanans = set<Point>(hananPs.begin(), hananPs.end());
	vector<HananInfo> leftHanan = vector<HananInfo>();
	int pMstL = 0;
	int psMstL = 0;
	while (1) {
		leftHanan.clear();
		mstMhtDist(psPoints, pMstL);
		psMstL = pMstL;
		for (auto iter = unusedHanans.begin(); iter != unusedHanans.end(); ++iter) {
			//psPoints.push_back(*iter);
			int withXL = 0;
			//mstMhtDist(psPoints, withXL);
			DynamicMST temp = dynamicMST;
			temp.addPoint(*iter);
			withXL = temp.mstValue;
			//cout << withXL << endl;
			int diff = pMstL - withXL;
			if (diff > 0) {
				leftHanan.push_back(HananInfo(*iter, diff));
			}
			//psPoints.pop_back();
		}
		if (leftHanan.empty()) {
			break;
		}
		sort(leftHanan.begin(), leftHanan.end());
		for (auto iter = leftHanan.begin(); iter != leftHanan.end(); ++iter) {
			int psxMstL = 0;
			//psPoints.push_back(iter->p);
			//mstMhtDist(psPoints, psxMstL);
			DynamicMST temp = dynamicMST;
			temp.addPoint(iter->p);
			psxMstL = temp.mstValue;
			int diffx = psMstL - psxMstL;
			if (diffx >= iter->diff) {
				psMstL = psxMstL;
				dynamicMST = temp;
				psPoints.push_back(iter->p);
				unusedHanans.erase(iter->p);
			}
			else {
				//psPoints.pop_back();
			}
		}
	}
	steinerMst = mstMhtDist(psPoints, steinerMstL);
	cout << originMstL << " " << steinerMstL << endl;
	cout << dynamicMST.mstValue << endl;
}

void Batch1Steiner::printSolution(ostream& os) {
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