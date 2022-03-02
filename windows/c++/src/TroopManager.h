#pragma once

#include "Squad.h"
#include "Troop.h"
#include <vector>
#include <BWAPI.h>

class TroopManager {
private:
	std::vector<Squad> squads;

public:
	void update();
	void assignNewTroop(BWAPI::Unit unit);
};