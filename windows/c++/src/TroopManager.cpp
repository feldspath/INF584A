#include "TroopManager.h"

#include <BWAPI.h>
#include <optional>
#include <algorithm>
#include <iterator>
#include "Tools.h"

void TroopManager::update() {
	for (auto& scout : scouts) scout.update();

	squad.update();

	// update troops: every military unit try to join the squad
	for (const auto& unit : BWAPI::Broodwar->self()->getUnits()) {
		if ((BWAPI::Filter::IsBuilding || !BWAPI::Filter::CanAttack || BWAPI::Filter::IsWorker || !BWAPI::Filter::IsCompleted)(unit)) continue;
		if (!(squad.tryAddUnit(unit)) && (unit->isIdle() || BWAPI::Broodwar->getFrameCount() % 20 == 0)) unit->attack(squad.getCenter());
	}

	// decision tree
	BWAPI::Unit target =
		getClosestHealingUnit().value_or(
			getDangerousEnemyUnit().value_or(
				getLowLifePreviousUnit().value_or(
					getClosestOffensiveUnit().value_or(
						getPreviousUnit().value_or(
							getClosetWorker().value_or(
								getClosetUnit().value_or(
									getClosestGarbage().value_or(nullptr))))))));

	if (target == nullptr) {
		// no target found: go to the enemy base
		const auto enemyBase = BWAPI::Position(Tools::getEnemyStartLocation());
		for (const auto& unit : squad.getTroops()) {
			
			if (BWAPI::Broodwar->getFrameCount() % 20 == 0 || unit->isIdle()) {
				if (unit->getDistance(squad.getCenter()) > squad.getMergeDistance())
					unit->move(squad.getCenter());
				else 
					unit->attack(enemyBase);
			}
		}
	}
	else {
		//BWAPI::Broodwar->setLocalSpeed(67);
		// if the target building is being repaired, switch to the unit repairing it
		if (target->getType().isBuilding() && target->isBeingHealed()) target = getHealingSCV(target).value_or(target);

		bool printed = false;
		for (const auto& unit : squad.getTroops()) {
			if (BWAPI::Broodwar->getFrameCount() % 20 == 0 || unit->isIdle() || !(squad.getPreviousUnit().has_value()) || target->getID() != squad.getPreviousUnit().value()->getID()) {
				unit->attack(target);
				if (!printed) {
					//printf("attacking %s\n", target->getType().getName().c_str());
					printed = true;
				}
			}
		}
	}
	squad.updatePreviousTarget(target == nullptr ? std::nullopt : std::optional<BWAPI::Unit>(target));
}

static bool isInRange(const BWAPI::Unit enemy, const BWAPI::Unit troop) {
	return enemy->getDistance(troop->getPosition()) < enemy->getType().seekRange();
}

static bool isLoadedBunker(const BWAPI::Unit unit) {
	return Tools::compareUnitTypes(unit->getType(), BWAPI::UnitTypes::Terran_Bunker) && unit->getLoadedUnits().size() > 0;
}

static bool isOffensive(const BWAPI::Unit unit) {
	return unit->exists() && (unit->getType().groundWeapon().targetsGround() || isLoadedBunker(unit)) && !(unit->getType().isWorker()) && !(unit->isFlying());
}

static bool isGarbage(const BWAPI::Unit unit) {
	return Tools::compareUnitTypes(BWAPI::UnitTypes::Zerg_Larva, unit->getType()) || Tools::compareUnitTypes(BWAPI::UnitTypes::Zerg_Egg, unit->getType());
}

std::optional<BWAPI::Unit> TroopManager::getClosestHealingUnit() const {
	BWAPI::Unitset healingUnits;
	for (const auto& enemy : BWAPI::Broodwar->enemy()->getUnits()) {
		if (enemy->getType().getID() == BWAPI::UnitTypes::Terran_Medic.getID())
			healingUnits.insert(enemy);
	}
	return Tools::getClosestUnitTo(squad.getCenter(), healingUnits);
}


std::optional<BWAPI::Unit> TroopManager::getDangerousEnemyUnit() const {
	// returns previous unit if it can hit a unit of the squad
	const auto previousUnit = squad.getPreviousUnit();
	if (previousUnit.has_value() && isOffensive(previousUnit.value())) {
		for (const auto& troop : squad.getTroops()) {
			if (isInRange(previousUnit.value(), troop))
				return previousUnit;
		}
	}

	// otherwise returns closest unit that can hit a unit of the squad
	BWAPI::Unitset dangerousUnits;
	for (const auto& enemy : BWAPI::Broodwar->enemy()->getUnits()) {
		if (!(isOffensive(enemy))) continue;
		for (const auto& troop : squad.getTroops())
			if (isInRange(enemy, troop)) dangerousUnits.insert(enemy);
	}
	return Tools::getClosestUnitTo(squad.getCenter(), dangerousUnits);
}

std::optional<BWAPI::Unit> TroopManager::getLowLifePreviousUnit() const {
	const auto previousUnit = squad.getPreviousUnit();
	if (!(previousUnit.has_value())) return std::nullopt;
	if (squad.getDPSEstimate() * 50 > previousUnit.value()->getHitPoints()) return previousUnit;
	return std::nullopt;
}

std::optional<BWAPI::Unit> TroopManager::getClosestOffensiveUnit() const {
	BWAPI::Unitset offensiveUnits;
	for (const auto& enemy : BWAPI::Broodwar->enemy()->getUnits())
		if (isOffensive(enemy)) offensiveUnits.insert(enemy);

	return Tools::getClosestUnitTo(squad.getCenter(), offensiveUnits);
}

std::optional<BWAPI::Unit> TroopManager::getPreviousUnit() const {
	return squad.getPreviousUnit();
}

std::optional<BWAPI::Unit> TroopManager::getClosetWorker() const {
	BWAPI::Unitset enemyWorkers;
	for (const auto& enemy : BWAPI::Broodwar->enemy()->getUnits())
		if (enemy->getType().isWorker()) enemyWorkers.insert(enemy);

	return Tools::getClosestUnitTo(squad.getCenter(), enemyWorkers);
}

std::optional<BWAPI::Unit> TroopManager::getClosetUnit() const {
	BWAPI::Unitset enemyUnits;
	for (const auto& enemy : BWAPI::Broodwar->enemy()->getUnits())
		if (!isGarbage(enemy) && enemy->exists() && !(enemy->isFlying()) && enemy->exists()) enemyUnits.insert(enemy);

	return Tools::getClosestUnitTo(squad.getCenter(), enemyUnits);
}

std::optional<BWAPI::Unit> TroopManager::getHealingSCV(BWAPI::Unit building) const {
	for (const auto& enemy : BWAPI::Broodwar->enemy()->getUnits()) {
		if (enemy->getType().getID() == BWAPI::UnitTypes::Terran_SCV && enemy->isRepairing() && enemy->getTarget()->getID() == building->getID())
			return enemy;
	}
	return std::nullopt;
}

std::optional<BWAPI::Unit> TroopManager::getClosestGarbage() const {
	BWAPI::Unitset larvae;
	for (const auto& enemy : BWAPI::Broodwar->enemy()->getUnits())
		if (isGarbage(enemy) && enemy->exists()) larvae.insert(enemy);

	return Tools::getClosestUnitTo(squad.getCenter(), larvae);
}

void TroopManager::addScout(BWAPI::Unit unit) {
	scouts.emplace_back<Scout>(unit);
}