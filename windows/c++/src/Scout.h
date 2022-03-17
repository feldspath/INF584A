#pragma once

#include "Unit.h"
#include "Tools.h"
#include <BWAPI.h>
#include <vector>

class Scout : Unit {
private:
	int target;
	std::vector<BWAPI::Position> scoutPositions;

	static int count;

public:
	Scout::Scout(const BWAPI::Unit& unit);
	void update();
};

