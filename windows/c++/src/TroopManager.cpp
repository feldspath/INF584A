#include "TroopManager.h"

#include <BWAPI.h>
#include <optional>
#include "Tools.h"


TroopManager::TroopManager() {
	previousTarget = std::nullopt;
}

static int targetUnitPriority(BWAPI::Unit unit) {
	if ((!BWAPI::Filter::IsWorker && !BWAPI::Filter::IsBuilding)(unit)) return 2;
	if ((BWAPI::Filter::IsWorker)(unit)) return 1;
	else return 0;
}

BWAPI::Unit getClosest(const BWAPI::Unitset& set, BWAPI::Position pos) {
	if (set.size() == 0) return nullptr;
	auto it = set.begin();
	BWAPI::Unit closest = *it;
	float minDist = pos.getDistance(closest->getPosition());
	for (; it != set.end(); ++it) {
		if ((*it)->getType().getID() == BWAPI::UnitTypes::Resource_Vespene_Geyser.getID()) continue;
		if (minDist > pos.getDistance((*it)->getPosition())) {
			minDist = pos.getDistance((*it)->getPosition());
			closest = *it;
		}
	}
	return closest;
}

BWAPI::Unitset getTroops(BWAPI::Unitset& alls) {
	BWAPI::Unitset result;
	for (const auto& unit : alls) {
		if (unit->getType().canAttack() && unit->getPlayer()->getID() == BWAPI::Broodwar->enemy()->getID() && !(unit->getType().isWorker()) && !(unit->getType().isBuilding()) && !(unit->getType().isFlyer()) && unit->getType().getID() != BWAPI::UnitTypes::Resource_Vespene_Geyser.getID())
			result.insert(unit);
	}
	return result;
}

BWAPI::Unitset getWorkers(BWAPI::Unitset& alls) {
	BWAPI::Unitset result;
	for (const auto& unit : alls) {
		if (unit->getPlayer()->getID() == BWAPI::Broodwar->enemy()->getID() && unit->getType().isWorker() && !(unit->getType().isFlyer()) && unit->getType().getID() != BWAPI::UnitTypes::Resource_Vespene_Geyser.getID())
			result.insert(unit);
	}
	return result;
}

BWAPI::Unitset getBuildings(BWAPI::Unitset& alls) {
	BWAPI::Unitset result;
	for (const auto& unit : alls) {
		if (unit->getPlayer()->getID() == BWAPI::Broodwar->enemy()->getID() && unit->getType().isBuilding() && !(unit->getType().isFlyer()) && unit->getType().getID() != BWAPI::UnitTypes::Resource_Vespene_Geyser.getID())
			result.insert(unit);
	}
	return result;
}

void TroopManager::update() {
	--squad.frameCount;
	squad.removeDeadTroops();
	squad.computeCenter();
	for (const auto& unit : BWAPI::Broodwar->self()->getUnits()) {
		if ((!BWAPI::Filter::CanAttack || BWAPI::Filter::IsWorker || !BWAPI::Filter::IsCompleted)(unit)) continue;
		if (squad.troops.size() == 0) {
			squad.addUnit(unit);
			squad.center = unit->getPosition();
			continue;
		}
		if (BWAPI::Position(unit->getPosition()).getDistance(squad.center) < squad.mergeDistance) {
			squad.addUnit(unit);
		}
		else {
			unit->move(squad.center);
		}
	}

	std::optional<BWAPI::Unit> previousTargetUnit;
	if (previousTarget.has_value()) {
		for (const auto& unit : BWAPI::Broodwar->getAllUnits()) {
			if (unit->getID() == previousTarget.value()) {
				previousTargetUnit = unit;
				break;
			}
		}
	}

	std::optional<BWAPI::Unit> newTargetUnit;
	//const auto enemyTroops = BWAPI::Broodwar->getUnitsInRadius(squad.center, squad.hostileDistance, BWAPI::Filter::IsEnemy && !(BWAPI::Filter::IsWorker) && !(BWAPI::Filter::IsBuilding) && !(BWAPI::Filter::IsFlyer));
	//const auto enemyWorkers = BWAPI::Broodwar->getUnitsInRadius(squad.center, squad.hostileDistance, BWAPI::Filter::IsEnemy && BWAPI::Filter::IsWorker);
	//const auto enemyBuildings = BWAPI::Broodwar->getUnitsInRadius(squad.center, squad.hostileDistance, BWAPI::Filter::IsEnemy && BWAPI::Filter::IsBuilding);
	const auto enemyTroops = getTroops(BWAPI::Broodwar->getUnitsInRadius(squad.center, squad.hostileDistance));
	const auto enemyWorkers = getWorkers(BWAPI::Broodwar->getUnitsInRadius(squad.center, squad.hostileDistance));
	const auto enemyBuildings = getBuildings(BWAPI::Broodwar->getUnitsInRadius(squad.center, squad.hostileDistance));

	if (enemyTroops.size() > 0) {
		newTargetUnit = getClosest(enemyTroops, squad.center);
	}
	else if (enemyWorkers.size() > 0) {
		newTargetUnit = getClosest(enemyWorkers, squad.center);
		printf("worker found\n");
	}
	else if (enemyBuildings.size() > 0) {
		newTargetUnit = getClosest(enemyBuildings, squad.center);
		printf("building found: %s\n", newTargetUnit.value()->getType().getName().c_str());
	}

	if (newTargetUnit.has_value() && previousTargetUnit.has_value() && targetUnitPriority(previousTargetUnit.value()) >= targetUnitPriority(newTargetUnit.value()))
	{
		newTargetUnit = previousTargetUnit.value();
	}

	if (!(newTargetUnit.has_value())) {
		squad.getTroops().move(BWAPI::Position(Tools::getEnemyStartLocation()));
	}
	else {
		BWAPI::Broodwar->setLocalSpeed(67);
		int attackingUnits = 0;
		for (const auto& unit : squad.getTroops()) {
			if (squad.frameCount == 0) {
				unit->rightClick(newTargetUnit.value()->getPosition() + BWAPI::Position(0, -64));
				continue;
			}
			if (unit->isAttacking()) ++attackingUnits;
			unit->rightClick(newTargetUnit.value());
			//unit->attack(newTargetUnit.value());
		}
		printf("not attacking: %d\n", squad.troops.size() - attackingUnits);
		previousTarget = newTargetUnit.value()->getID();
	}
	if (squad.frameCount == 0) {
		squad.frameCount = 30;
	}
}

void TroopManager::assignNewTroop(BWAPI::Unit unit) {
	//squad.addUnit(unit);
}