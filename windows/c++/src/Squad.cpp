#pragma once

#include "Squad.h"
#include <BWAPI.h>
#include <set>
#include <vector>
#include "Tools.h"
#include <assert.h>

void Squad::addUnit(const BWAPI::Unit& unit) {
	troops.insert(unit->getID());
}

void Squad::computeCenter() {
	center = BWAPI::Position(0, 0);
	std::set<int> newSet;
	int count = 0;
	for (const auto& unit : BWAPI::Broodwar->self()->getUnits()) {
		const auto& troop = troops.find(unit->getID());
		if (troop != troops.end() && unit->exists()) {
			center += unit->getPosition();
			newSet.insert(unit->getID());
			++count;
		}
	}
	center /= count;
	troops.clear();
	troops = newSet;
	BWAPI::Broodwar->drawCircle(BWAPI::CoordinateType::Map, center.x, center.y, mergeDistance, BWAPI::Colors::Red);
}

BWAPI::Unitset Squad::getTroops() {
	BWAPI::Unitset result;
	result.reserve(troops.size());
	for (const auto& unit : BWAPI::Broodwar->self()->getUnits()) {
		const auto& troop = troops.find(unit->getID());
		if (troop != troops.end()) {
			result.insert(unit);
		}
	}
	return result;
}

void Squad::removeDeadTroops() {
	for (const auto& unit : BWAPI::Broodwar->self()->getUnits()) {
		if (troops.find(unit->getID()) != troops.end() && !(unit->exists())) {
			troops.erase(troops.find(unit->getID()));
		}
	}
}
