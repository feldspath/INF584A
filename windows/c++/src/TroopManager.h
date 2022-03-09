#pragma once

#include "Squad.h"
#include "Troop.h"
#include <vector>
#include <BWAPI.h>
#include <optional>

class TroopManager {
private:
	Squad squad;
	std::optional<int> previousTarget;

	bool attacking = false;

public:
	TroopManager();
	void update();
	void assignNewTroop(BWAPI::Unit unit);
};