#include "TroopManager.h"

#include <BWAPI.h>


TroopManager::TroopManager() {
	squads.emplace_back();
}

void TroopManager::update() {
	if (attacking = false) {
		
	}
}

void TroopManager::assignNewTroop(BWAPI::Unit unit) {
	squads.at(0).addUnit(unit);
}

void TroopManager::launchAttack() {
	attacking = true;
}
