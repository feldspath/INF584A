#pragma once

#include "Squad.h"
#include "Troop.h"
#include <vector>
#include <BWAPI.h>

class TroopManager {
private:
	std::vector<Squad> squads;

	bool attacking = false;

public:
	TroopManager();
	void update();
	void launchAttack();
	void assignNewTroop(BWAPI::Unit unit);
};