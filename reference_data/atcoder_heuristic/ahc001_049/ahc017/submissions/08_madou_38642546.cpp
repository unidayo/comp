#pragma GCC optimize("O3")
#pragma GCC optimize("unroll-loops")

#include <cstdio>
#include <cstdlib>
#include <climits>

#include <iostream>
#include <sstream>
#include <array>
#include <vector>
#include <deque>
#include <queue>
#include <set>
#include <unordered_set>
#include <algorithm>
#include <memory>
#include <chrono>

auto StartTime = std::chrono::high_resolution_clock::now();
using namespace std;

template <typename T>
struct Span {
	T* start;
	int count;

	Span() = default;
	Span(T* start, int count) : start(start), count(count) {}

	int size() const {
		return count;
	}

	T& operator[](int i) {
		return start[i];
	}

	const T& operator[](int i) const {
		return start[i];
	}

	auto begin() {
		return start;
	}
	auto end() {
		return start + count;
	}
};


constexpr int MAX_CITY = 1000;
constexpr int MAX_ROAD = 3000;
constexpr int MAX_DAY = 30;
constexpr int MAX_REPAIR = 1200;
constexpr int MAX_DISTANCE = 1000000000;

struct City {
	short x;
	short y;
};

struct Road {
	short city0;
	short city1;
	int length;
};

struct CityRoad {
	int length;
	short roadIndex;
	short cityIndex; 
};


struct World {
	int cityCount;
	int roadCount;
	int dayCount;
	int repairCount;

	array<City, MAX_CITY> cities;
	array<Road, MAX_ROAD> roads;

	array<Span<CityRoad>, MAX_CITY> cityRoads;
	array<CityRoad, MAX_ROAD * 2> cityRoadsMem; 

	void init() {
		array<vector<CityRoad>, MAX_CITY> cityRoadsTp;
		for (int ci = 0; ci < cityCount; ci++) {
			cityRoadsTp[ci].clear();
		}
		
		for (short ri = 0; ri < roadCount; ri++) {
			Road road = roads[ri];
			cityRoadsTp[road.city0].push_back({ road.length, ri, road.city1 });
			cityRoadsTp[road.city1].push_back({ road.length, ri, road.city0 });
		}

		int counter = 0;
		for (int ci = 0; ci < cityCount; ci++) {
			auto& v = cityRoadsTp[ci];
			int s = (int)v.size();
			Span<CityRoad> cs{ &cityRoadsMem[counter], s };
			counter += s;
			std::copy(v.begin(), v.end(), cs.begin());
			cityRoads[ci] = cs;
		}
	}
};


struct Route {
	int distance;
	int lastRoad;
};



namespace Operation {

	struct RouteMaps {
		array<Route, MAX_CITY* MAX_CITY> route;
		array<long long, MAX_CITY> distanceSum;
	};

	struct RepairRoads {
		array<bool, MAX_ROAD> memory = {};
		int count = 0;

		void clear() {
			memory = {};
			count = 0;
		}

		int size() const {
			return count;
		}

		void insert(int index) {
			if (!memory[index]) {
				memory[index] = true;
				count++;
			}
		}

		void erase(int index) {
			if (memory[index]) {
				memory[index] = false;
				count--;
			}
		}

		bool contains(int index) const {
			return memory[index];
		}
	};

	struct Day {
		RepairRoads repairRoads;

		unordered_set<short> cities;
		int dayIndex;

		RouteMaps routeMaps;

		void clear() {
			repairRoads.clear();
			cities.clear();
		}

		int size() const {
			return (int)repairRoads.size();
		}

		bool contains(int roadIndex) const {
			return repairRoads.contains(roadIndex);
		}

		bool isFull(const World& w) const {
			return size() >= w.repairCount;
		}
	};

	struct CityFlags {
		int version;
		array<unsigned char, MAX_CITY> flags;

		void init() {
			version = 1;
			flags = {};
		}

		void clear() {
			version++;
			if (version == 256) {
				init();
			}
		}

		void set(int index) {
			flags[index] = version;
		}

		bool isSet(int index) const {
			return flags[index] == version;
		}
	};

	struct RepairDiff {
		int recalcCount;
		array<short, MAX_CITY> recalcList; 
		CityFlags recalcTable; 
		array<Route, MAX_CITY> route; 
		long long distanceDiff;

		void init() {
			recalcCount = 0;
			recalcTable.init();
		}

		void clear() {
			recalcCount = 0;
			recalcTable.clear();
		}

		void addRecalc(int index) {
			set(index);
			recalcList[recalcCount] = index;
			recalcCount++;
		}

		void set(int index) {
			recalcTable.set(index);
		}

		bool isSet(int index) const {
			return recalcTable.isSet(index);
		}

		void from(const RepairDiff& src) {
			recalcCount = src.recalcCount;
			distanceDiff = src.distanceDiff;
			if (recalcCount > 0) {
				copy(src.recalcList.begin(), src.recalcList.begin() + recalcCount, recalcList.begin());
				for (int i = 0; i < recalcCount; i++) {
					int ci = recalcList[i];
					route[ci] = src.route[ci];
				}
			}
		}
	};

	struct DayRef {
		Day* day;

		bool operator<(const DayRef& right) const {
			return day->size() < right.day->size();
		}
	};

	struct DistanceIndex {
		int distance;
		int index;

		bool operator<(const DistanceIndex& right) const {
			return distance > right.distance;
		}
	};

	int phase;

	int currentRoadIndex;
	int currentEpoch;

	vector<short> calcCities;

	priority_queue<DistanceIndex> priorityQueue;


	array<Day, MAX_DAY> days;

	CityFlags cityFlags;
	CityFlags cityInserted;
	array<RepairDiff, MAX_CITY> repairDiffList;
	array<RepairDiff, MAX_CITY> bestResult;
	RouteMaps addRouteMaps;

	vector<short> roadRefs;


	void addCalcCities(const World& w) {
		if (calcCities.size() >= w.cityCount) {
			return;
		}
		int maxDistance = INT_MIN;
		int maxIndex = -1;
		for (int ci = 0; ci < w.cityCount; ci++) {
			City c = w.cities[ci];
			int minD = INT_MAX;
			for (short ci2 : calcCities) {
				City c2 = w.cities[ci2];
				int dx = c.x - c2.x;
				int dy = c.y - c2.y;
				int d = dx * dx + dy * dy;
				if (minD > d) {
					minD = d;
				}
			}
			if (maxDistance < minD) {
				maxDistance = minD;
				maxIndex = ci;
			}
		}
		calcCities.push_back(maxIndex);
	}

	void clearQueue() {
		while (priorityQueue.size() > 0) {
			priorityQueue.pop();
		}
	}

	void findRoute(const World& w, const Day& day, RouteMaps& routeMaps, int startCityIndex) {
		int cityCount = w.cityCount;
		auto& repairRoads = day.repairRoads;

		clearQueue();

		Route* rt = &routeMaps.route[startCityIndex * cityCount];
		for (int ci = 0; ci < cityCount; ci++) {
			rt[ci] = { MAX_DISTANCE, -1 };
		}
		auto& done = cityFlags;
		done.clear();

		rt[startCityIndex].distance = 0;
		priorityQueue.push({ 0, startCityIndex });

		while (!priorityQueue.empty()) {
			DistanceIndex di = priorityQueue.top();
			priorityQueue.pop();
			done.set(di.index);
			Route& r = rt[di.index];
			int distance = r.distance;
			if (distance < di.distance) {
				continue;
			}
			Span<CityRoad> crs = w.cityRoads[di.index];
			for (CityRoad cr : crs) {
				if (done.isSet(cr.cityIndex)) {
					continue;
				}
				if (repairRoads.contains(cr.roadIndex)) {
					continue; 
				}
				int nLen = distance + cr.length;
				Route& r2 = rt[cr.cityIndex];
				if (r2.distance > nLen) {
					r2 = { nLen, cr.roadIndex };
					priorityQueue.push({ nLen, cr.cityIndex });
				}
			}
		}

		long long distanceSum = 0;
		for (int i = 0; i < cityCount; i++) {
			distanceSum += rt[i].distance;
		}

		routeMaps.distanceSum[startCityIndex] = distanceSum;
	}

	
	
	void findRouteRemoveRepair(const World& w, Day& day, RouteMaps& routeMaps, int startCityIndex, int removeRepairRoadIndex) {
		int cityCount = w.cityCount;
		auto& repairRoads = day.repairRoads;

		vector<short> removeRecalcCities;
		removeRecalcCities.clear();

		day.repairRoads.erase(removeRepairRoadIndex); 

		int offset = startCityIndex * cityCount;
		const Route* srcRt = &day.routeMaps.route[offset];

		Road newRepairRoad = w.roads[removeRepairRoadIndex];
		const Route& route0 = srcRt[newRepairRoad.city0];
		const Route& route1 = srcRt[newRepairRoad.city1];
		int minDistance = min(route0.distance, route1.distance);

		clearQueue();

		auto& done = cityFlags;
		done.clear();
		cityInserted.clear();
		Route* rt = &routeMaps.route[offset];
		for (int ci = 0; ci < cityCount; ci++) {
			const Route& r = srcRt[ci];
			if (r.distance <= minDistance) {
				rt[ci] = r; 
				done.set(ci);
			} else {
				rt[ci] = { MAX_DISTANCE, -1 };
			}
		}
		for (int ci = 0; ci < cityCount; ci++) {
			if (done.isSet(ci)) {
				continue;
			}
			Span<CityRoad> crs = w.cityRoads[ci];
			for (CityRoad cr : crs) {
				if (repairRoads.contains(cr.roadIndex)) {
					continue; 
				}
				int cityIndex = cr.cityIndex;
				if (done.isSet(cityIndex) && !cityInserted.isSet(cityIndex)) {
					priorityQueue.push({ rt[cityIndex].distance, cityIndex });
					cityInserted.set(cityIndex);
				}
			}
		}

		while (!priorityQueue.empty()) {
			DistanceIndex di = priorityQueue.top();
			priorityQueue.pop();
			done.set(di.index);
			Route& r = rt[di.index];
			int distance = r.distance;
			if (distance < di.distance) {
				continue;
			}
			Span<CityRoad> crs = w.cityRoads[di.index];
			for (CityRoad cr : crs) {
				if (done.isSet(cr.cityIndex)) {
					continue;
				}
				if (repairRoads.contains(cr.roadIndex)) {
					continue; 
				}
				int nLen = distance + cr.length;
				Route& r2 = rt[cr.cityIndex];
				if (r2.distance > nLen) {
					r2 = { nLen, cr.roadIndex };
					priorityQueue.push({ nLen, cr.cityIndex });
				}
			}
		}

		long long distanceSum = 0;
		for (int i = 0; i < cityCount; i++) {
			distanceSum += rt[i].distance;
		}

		routeMaps.distanceSum[startCityIndex] = distanceSum;
	}

	
	
	long long findRouteAddRepair(const World& w, Day& day, int startCityIndex, int addRepairRoadIndex) {
		int cityCount = w.cityCount;
		auto& repairRoads = day.repairRoads;

		const Route* rt = &day.routeMaps.route[startCityIndex * cityCount];
		RepairDiff& rd = repairDiffList[startCityIndex];
		rd.clear();

		Road newRepairRoad = w.roads[addRepairRoadIndex];
		if (rt[newRepairRoad.city0].lastRoad == addRepairRoadIndex) {
			rd.addRecalc(newRepairRoad.city0);
		} else if (rt[newRepairRoad.city1].lastRoad == addRepairRoadIndex) {
			rd.addRecalc(newRepairRoad.city1);
		} else {
			rd.distanceDiff = 0;
			return 0; 
		}

		repairRoads.insert(addRepairRoadIndex); 

		int firstIndex = 0;
		int lastIndex = 1;
		while (firstIndex != lastIndex) {
			for (int i = firstIndex; i < lastIndex; i++) {
				int ci = rd.recalcList[i];
				Span<CityRoad> crs = w.cityRoads[ci];
				for (CityRoad cr : crs) {
					if (rd.isSet(cr.cityIndex)) {
						continue; 
					}
					if (repairRoads.contains(cr.roadIndex)) {
						continue; 
					}
					if (rt[cr.cityIndex].lastRoad == cr.roadIndex) {
						rd.addRecalc(cr.cityIndex);
					}
				}
			}

			firstIndex = lastIndex;
			lastIndex = rd.recalcCount;
		}

		
		clearQueue();

		
		for (int i = 0; i < rd.recalcCount; i++) {
			int ci = rd.recalcList[i];
			rd.route[ci] = { MAX_DISTANCE, -1 };
			Span<CityRoad> crs = w.cityRoads[ci];
			for (CityRoad cr : crs) {
				int cityIndex = cr.cityIndex;
				if (rd.isSet(cityIndex)) {
					continue; 
				}
				if (repairRoads.contains(cr.roadIndex)) {
					continue; 
				}
				priorityQueue.push({ rt[cityIndex].distance, cityIndex });
			}
		}

		auto& done = cityFlags;
		done.clear();

		while (!priorityQueue.empty()) {
			DistanceIndex di = priorityQueue.top();
			priorityQueue.pop();
			done.set(di.index);
			const Route* r;
			if (rd.isSet(di.index)) {
				r = &rd.route[di.index]; 
			} else {
				r = &rt[di.index];
			}
			int distance = r->distance;
			if (distance < di.distance) {
				continue;
			}
			Span<CityRoad> crs = w.cityRoads[di.index];
			for (CityRoad cr : crs) {
				int cityIndex = cr.cityIndex;
				if (done.isSet(cityIndex)) {
					continue; 
				}
				if (!rd.isSet(cityIndex)) {
					continue; 
				}
				if (repairRoads.contains(cr.roadIndex)) {
					continue; 
				}
				int nLen = distance + cr.length;
				Route& r2 = rd.route[cityIndex];
				if (r2.distance > nLen) {
					r2 = { nLen, cr.roadIndex };
					priorityQueue.push({ nLen, cityIndex });
				}
			}
		}


		repairRoads.erase(addRepairRoadIndex); 

		long long distanceDiffSum = 0;
		for (int i = 0; i < rd.recalcCount; i++) {
			int ci = rd.recalcList[i];
			distanceDiffSum += rd.route[ci].distance - rt[ci].distance;
		}

		rd.distanceDiff = distanceDiffSum;
		return distanceDiffSum;
	}


	void findRouteAllDay(const World& w, int startCityIndex) {
		for (int i = 0; i < w.dayCount; i++) {
			Day& day = days[i];
			findRoute(w, day, day.routeMaps, startCityIndex);
		}
	}

	void firstPhase(const World& w) {
		for (int di = 0; di < w.dayCount; di++) {
			Day& day = days[di];
			day.clear();
			day.dayIndex = di;
		}

		
		City firstCity = w.cities[0];
		int minX = firstCity.x;
		int maxX = firstCity.x;
		int minXIndex = 0;
		int maxXIndex = 0;
		for (int ci = 1; ci < w.cityCount; ci++) {
			City c = w.cities[ci];
			int x = c.x;
			if (minX > x) {
				minX = x;
				minXIndex = ci;
			}
			if (maxX < x) {
				maxX = x;
				maxXIndex = ci;
			}
		}
		calcCities.clear();
		calcCities.push_back(minXIndex);
		calcCities.push_back(maxXIndex);
		int add = (w.dayCount <= 10 ? 11 : (w.dayCount <= 17 ? 19 : 27));
		for (int i = 0; i < add; i++) {
			addCalcCities(w);
		}

		cityFlags.init();
		cityInserted.init();
		for (int ci = 0; ci < w.cityCount; ci++) {
			repairDiffList[ci].init();
		}
		for (int ci : calcCities) {
			findRouteAllDay(w, ci);
		}

		
		roadRefs.clear();
		for (int ri = 0; ri < w.roadCount; ri++) {
			roadRefs.push_back(ri);
		}
		auto roadOrder = [](const World& w, int roadIndex) {
			Road r = w.roads[roadIndex];
			City c0 = w.cities[r.city0];
			City c1 = w.cities[r.city1];
			int x = c0.x + c1.x - 1000;
			int y = c0.y + c1.y - 1000;
			return x * x + y * y;
		};
		sort(roadRefs.begin(), roadRefs.end(), [&w, &roadOrder](short left, short right) {
			return roadOrder(w, left) < roadOrder(w, right);
		});

		for (int n = 0; n < w.roadCount; n++) {
			int ri = roadRefs[n];
			Road r = w.roads[ri];
			long long maxScore = LLONG_MIN;
			int removeDayIndex = -1;
			for (int di = 0; di < w.dayCount; di++) {
				Day& day = days[di];
				if (day.isFull(w)) {
					continue;
				}
				long long score = day.size() * -40000;
				for (int ci : calcCities) {
					score -= findRouteAddRepair(w, day, ci, ri);
				}

				if (maxScore < score) {
					maxScore = score;
					removeDayIndex = di;
					for (int ci : calcCities) {
						bestResult[ci].from(repairDiffList[ci]); 
					}
				}
			}

			Day& removeDay = days[removeDayIndex];
			removeDay.repairRoads.insert(ri);
			for (int ci : calcCities) {
				int offset = ci * w.cityCount;

				const RepairDiff& rd = bestResult[ci];
				Route* route = &removeDay.routeMaps.route[offset];
				for (int i = 0; i < rd.recalcCount; i++) {
					int recalcCi = rd.recalcList[i];
					route[recalcCi] = rd.route[recalcCi];
				}
				removeDay.routeMaps.distanceSum[ci] += rd.distanceDiff;
			}
		}

		phase = 2;
	}

	void secondPhase(const World& w) {
		vector<int> q;
		unordered_set<int> done;
		int n = 0;
		for (; n < 5; n++) {
			bool notSplit = true;
			for (int di = 0; di < w.dayCount; di++) {
				Day& day = days[di];
				q.clear();
				q.push_back(0);
				done.clear();
				done.insert(0);
				while (!q.empty()) {
					int ci = q.back();
					q.pop_back();
					Span<CityRoad> crs = w.cityRoads[ci];
					for (CityRoad cr : crs) {
						if (done.count(cr.cityIndex) != 0) {
							continue;
						}
						if (day.contains(cr.roadIndex)) {
							continue; 
						}
						q.push_back(cr.cityIndex);
						done.insert(cr.cityIndex);
					}
				}
				if (done.size() < w.cityCount) {
					notSplit = false;
					bool found = false;
					for (int ci = 0; ci < w.cityCount; ci++) {
						if (done.count(ci) == 0) {
							Span<CityRoad> crs = w.cityRoads[ci];
							for (CityRoad cr : crs) {
								if (done.count(cr.cityIndex) == 0) {
									continue;
								}
								if (day.contains(cr.roadIndex)) {
									day.repairRoads.erase(cr.roadIndex);
									for (int i = 1; i < w.dayCount; i++) {
										int index = di - i;
										if (index < 0) {
											index += w.dayCount;
										}
										if (!days[index].isFull(w)) {
											days[index].repairRoads.insert(cr.roadIndex);
											found = true;
											break;
										}
									}
									if (found) {
										break;
									}
								}
							}
							if (found) {
								break;
							}
						}
					}
				}
			}
			if (notSplit) {
				break;
			}
		}
		
		currentRoadIndex = 0;
		currentEpoch = 0;
		if (n != 0) {
			for (int ci : calcCities) {
				findRouteAllDay(w, ci);
			}
		}
		phase = 3;
	}

	void thirdPhase(const World& w, bool isAdd) {
		int roadIndex = roadRefs[currentRoadIndex];
		int addDayIndex = -1;
		for (int i = 0; i < w.dayCount; i++) {
			if (days[i].contains(roadIndex)) {
				addDayIndex = i;
				break;
			}
		}
		Day& addDay = days[addDayIndex];
		long long addDistanceDiff = 0;
		for (int ci : calcCities) {
			findRouteRemoveRepair(w, addDay, addRouteMaps, ci, roadIndex);
			addDistanceDiff += addRouteMaps.distanceSum[ci] - addDay.routeMaps.distanceSum[ci];
		}

		long long minRemoveDistanceDiff = LLONG_MAX;
		int removeDayIndex = -1;
		for (int di = 0; di < w.dayCount; di++) {
			if (di == addDayIndex) {
				continue;
			}
			Day& day = days[di];
			if (day.isFull(w)) {
				continue;
			}
			long long distanceDiff = 0;
			for (int ci : calcCities) {
				distanceDiff += findRouteAddRepair(w, day, ci, roadIndex);
			}
			if (isAdd) {
				if (day.size() >= w.repairCount - 1) {
					distanceDiff += 150000;
				} else if (day.size() >= w.repairCount - 2) {
					distanceDiff += 50000;
				} else if (day.size() >= w.repairCount - 3) {
					distanceDiff += 20000;
				}
			}
			if (minRemoveDistanceDiff > distanceDiff) {
				minRemoveDistanceDiff = distanceDiff;
				removeDayIndex = di;
				for (int ci : calcCities) {
					bestResult[ci].from(repairDiffList[ci]); 
				}
			}
		}

		Day& removeDay = days[removeDayIndex];
		if (addDistanceDiff + minRemoveDistanceDiff < 0) { 
			removeDay.repairRoads.insert(roadIndex);
			for (int ci : calcCities) {
				int offset = ci * w.cityCount;
				copy(addRouteMaps.route.begin() + offset, addRouteMaps.route.begin() + offset + w.cityCount, addDay.routeMaps.route.begin() + offset);
				addDay.routeMaps.distanceSum[ci] = addRouteMaps.distanceSum[ci];

				const RepairDiff& rd = bestResult[ci];
				Route* route = &removeDay.routeMaps.route[offset];
				for (int i = 0; i < rd.recalcCount; i++) {
					int recalcCi = rd.recalcList[i];
					route[recalcCi] = rd.route[recalcCi];
				}
				removeDay.routeMaps.distanceSum[ci] += rd.distanceDiff;
			}
		} else {
			addDay.repairRoads.insert(roadIndex); 
		}

		currentRoadIndex++;
		if (currentRoadIndex == w.roadCount) {
			currentRoadIndex = 0;
			if (isAdd && calcCities.size() < w.cityCount) {
				int addCount = 4;
				if (currentEpoch <= 1) {
					addCount = 4;
				} else if (currentEpoch <= 3) {
					addCount = 8;
				} else {
					addCount = 32;
				}
				for (int i = 0; i < addCount; i++) {
					if (calcCities.size() < w.cityCount) {
						addCalcCities(w);
						findRouteAllDay(w, calcCities.back());
					}
				}
			}
			currentEpoch++;
		}
	}


}

World world;

auto elapsedTime() {
	return chrono::high_resolution_clock::now() - StartTime;
}

int main()
{
  scanf("%d %d %d %d", &world.cityCount, &world.roadCount, &world.dayCount, &world.repairCount);
	for (int i = 0; i < world.roadCount; i++) {
		int city0, city1, length;
		scanf("%d %d %d", &city0, &city1, &length);
		world.roads[i] = { (short)(city0 - 1), (short)(city1 - 1), length };
	}
	for (int i = 0; i < world.cityCount; i++) {
		int x, y;
		scanf("%d %d", &x, &y);
		world.cities[i] = { (short)x, (short)y };
	}

	world.init();

	Operation::firstPhase(world);

	Operation::secondPhase(world);

	constexpr long long m = 1000000LL;
	int counter = 0;
	while (true) {
		long long elapsed = elapsedTime().count();
		if(elapsed >= 5950 * m){
			break;
		}
		bool limited = (elapsed >= 5500 * m);
		int loop = (limited ? 1 : 5);
		for (int i = 0; i < loop; i++) {
			Operation::thirdPhase(world, !limited);
			counter++;
		}
	}

	stringstream ss;
	for (int ri = 0; ri < world.roadCount; ri++) {
		if (ri != 0) {
			ss << " ";
		}
		for (int di = 0; di < world.dayCount; di++) {
			if (Operation::days[di].repairRoads.contains(ri)) {
				ss << (di + 1);
				break;
			}
		}
	}
	string output = ss.str();
	cout << output << endl;

	return 0;
}
