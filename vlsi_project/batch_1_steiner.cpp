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
			psPoints.push_back(*iter);
			int withXL = 0;
			mstMhtDist(psPoints, withXL);
			//cout << withXL << endl;
			int diff = pMstL - withXL;
			if (diff > 0) {
				leftHanan.push_back(HananInfo(*iter, diff));
			}
			psPoints.pop_back();
		}
		if (leftHanan.empty()) {
			break;
		}
		sort(leftHanan.begin(), leftHanan.end());
		for (auto iter = leftHanan.begin(); iter != leftHanan.end(); ++iter) {
			int psxMstL = 0;
			psPoints.push_back(iter->p);
			mstMhtDist(psPoints, psxMstL);
			int diffx = psMstL - psxMstL;
			if (diffx >= iter->diff) {
				psMstL = psxMstL;
				unusedHanans.erase(iter->p);
			}
			else {
				psPoints.pop_back();
			}
		}
	}
	steinerMst = mstMhtDist(psPoints, steinerMstL);
	//cout << originMstL << " " << steinerMstL << endl;
}

void Batch1Steiner::printSolution(ostream& os) {
	cout << originMstL << " " << steinerMstL << " " << timing << endl;
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

void Batch1SteinerAcc::route() {
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
			int withXL = 0;
			DynamicMST temp = dynamicMST;
			temp.addPoint(*iter);
			withXL = temp.mstValue;
			int diff = pMstL - withXL;
			if (diff > 0) {
				leftHanan.push_back(HananInfo(*iter, diff));
			}
		}
		if (leftHanan.empty()) {
			break;
		}
		sort(leftHanan.begin(), leftHanan.end());
		for (auto iter = leftHanan.begin(); iter != leftHanan.end(); ++iter) {
			int psxMstL = 0;
			DynamicMST temp = dynamicMST;
			temp.addPoint(iter->p);
			psxMstL = temp.mstValue;
			int diffx = psMstL - psxMstL;
			if (diffx >= iter->diff) {
				psMstL = psxMstL;
				//dynamicMST = temp;
				dynamicMST.addPoint(iter->p);
				psPoints.push_back(iter->p);
				unusedHanans.erase(iter->p);
			}
		}
	}
	steinerMst = mstMhtDist(psPoints, steinerMstL);
	//cout << originMstL << " " << steinerMstL << endl;
	//cout << dynamicMST.mstValue << endl;
}

void Iterated2Steiner::route() {
	vector<Point> psPoints = targetPs;
	set<Point> unusedHanans = set<Point>(hananPs.begin(), hananPs.end());
	vector<HananSetInfo> leftHanan;
	while (1) {
		vector<Point> hananVec(unusedHanans.begin(), unusedHanans.end());
		leftHanan.clear();
		// choose 1
		for (auto iter = hananVec.begin(); iter != hananVec.end(); ++iter) {
			DynamicMST temp = dynamicMST;
			temp.addPoint(*iter);
			int diff = dynamicMST.mstValue - temp.mstValue;
			if (diff > 0) {
				set<Point> ps;
				ps.insert(*iter);
				leftHanan.push_back(HananSetInfo(ps, diff));
			}
		}
		// choose 2(pair)
		int len = hananVec.size();
		for (int i = 0; i < len; ++i) {
			for (int j = i + 1; j < len; ++j) {
				DynamicMST temp = dynamicMST;
				temp.addPoint(hananVec.at(i));
				temp.addPoint(hananVec.at(j));
				int diff = dynamicMST.mstValue - temp.mstValue;
				if (diff > 0) {
					set<Point> ps;
					ps.insert(hananVec.at(i));
					ps.insert(hananVec.at(j));
					leftHanan.push_back(HananSetInfo(ps, diff));
				}
			}
		}
		if (leftHanan.empty())
			break;
		HananSetInfo choice = choose(leftHanan);
		
		for (auto iter = choice.ps.begin(); iter != choice.ps.end(); ++iter) {
			unusedHanans.erase(*iter);
			psPoints.push_back(*iter);
			dynamicMST.addPoint(*iter);
		}
	}
	steinerMst = mstMhtDist(psPoints, steinerMstL);
	//cout << originMstL << " " << steinerMstL << endl;
	//cout << dynamicMST.mstValue << endl;
}

HananSetInfo Iterated2Steiner::choose(const std::vector<HananSetInfo>& leftHanan) const {
	int maxd = INT_MIN;
	HananSetInfo ret;
	for (auto iter = leftHanan.begin(); iter != leftHanan.end(); ++iter) {
		if (iter->diff > maxd) {
			maxd = iter->diff;
			ret = *iter;
		}
	}
	return ret;
}

// softmax, rescale by mstvalue
HananSetInfo EnhancedIterated2Steiner::choose(const std::vector<HananSetInfo>& leftHanan) const {
	int maxd = INT_MIN;
	int mind = INT_MAX;
	for (auto iter = leftHanan.begin(); iter != leftHanan.end(); ++iter) {
		if (iter->diff > maxd) {
			maxd = iter->diff;
		}
		if (iter->diff < mind) {
			mind = iter->diff;
		}
	}
	vector<double> score(leftHanan.size(), 0.0);
	double sum = 0;
	for (int i = 0; i < leftHanan.size(); ++i) {
		sum += exp((leftHanan.at(i).diff - maxd) * 100.0 / (maxd - mind));
		score.at(i) = sum;
	}
	for (int i = 0; i < score.size(); ++i)
		score.at(i) /= sum;
	double r = DIS01(GEN);
	auto iter = upper_bound(score.begin(), score.end(), r);
	if (iter == score.end())
		return leftHanan.back();
	else
		return leftHanan.at(iter - score.begin());
}