#pragma once

#include "Squad.h"
#include <BWAPI.h>
#include <set>
#include <vector>
#include "Tools.h"
#include <assert.h>

int Squad::nSquadReset = 0;

void Squad::addUnit(const BWAPI::Unit& unit) {
	troops.insert(unit->getID());
}

void Squad::update() {
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

	previousTargetUnit = std::nullopt;
	if (previousTarget.has_value()) previousTargetUnit = previousTarget.value().getUnit();

	// Vespene Geyser bug fix: reset previous target if it is a geyser
	if (previousTargetUnit.has_value() && previousTargetUnit.value()->getType().getID() == BWAPI::UnitTypes::Resource_Vespene_Geyser.getID()) {
		previousTargetUnit = std::nullopt;
		previousTarget = std::nullopt;
	}
}

BWAPI::Unitset Squad::getTroops() const {
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

bool Squad::tryAddUnit(const BWAPI::Unit& unit) {
	if (troops.size() == 0) {
		addUnit(unit);
		center = unit->getPosition();
		++nSquadReset;
		return true;
	}
	if (BWAPI::Position(unit->getPosition()).getDistance(center) < mergeDistance) {
		addUnit(unit);
		return true;
	}
	return false;
}

BWAPI::Position Squad::getCenter() const {
	return center;
}

std::optional<BWAPI::Unit> Squad::getPreviousUnit() const {
	if (previousTargetUnit.has_value() && previousTargetUnit.value()->exists()) return previousTargetUnit;
	else return std::nullopt;
}

float Squad::getDPSEstimate() const {
	float totalDPS = 0.0f;
	for (const auto& troop : getTroops()) {
		totalDPS += troop->getType().groundWeapon().damageAmount() / troop->getType().groundWeapon().damageCooldown();
	}
	return totalDPS;
}

void Squad::updatePreviousTarget(std::optional<BWAPI::Unit> target) {
	if (target.has_value()) previousTarget = Unit(target.value());
	else previousTarget = std::nullopt;
}
