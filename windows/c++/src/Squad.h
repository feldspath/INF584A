#pragma once

#include "Troop.h"
#include <set>
#include <vector>

struct Squad {
	float mergeDistance = 128;
	float hostileDistance = 800;
	BWAPI::Position center;
	std::set<int> troops;
	int frameCount = 10;
	void addUnit(const BWAPI::Unit& unit);
	void computeCenter();
	BWAPI::Unitset getTroops();
	void removeDeadTroops();
};