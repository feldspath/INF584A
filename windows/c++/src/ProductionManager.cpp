#include "ProductionManager.h"
#include "Squad.h"
#include "Tools.h"
#include <optional>

#define require(TYPE) if (!(getExistingUnit(TYPE).has_value())) return TYPE;

ProductionManager::ProductionManager() : state(State::PREPARE) {
	//buildOrder.push_back(std::make_shared<ProductionTask>(BWAPI::UnitTypes::Enum::Zerg_Spawning_Pool, 1, 4));

}

static std::optional<BWAPI::Unit> getExistingUnit(BWAPI::UnitType type) {
	for (const auto& unit : BWAPI::Broodwar->self()->getUnits()) {
		if (Tools::compareUnitTypes(unit->getType(), type) && unit->exists()) {
			return unit;
		}
	}
	return std::nullopt;
}

static bool larvaExists() {
	return getExistingUnit(BWAPI::UnitTypes::Zerg_Larva).has_value();
}

static BWAPI::UnitType buildColony(BWAPI::UnitType colony) {
	if (!(getExistingUnit(BWAPI::UnitTypes::Zerg_Creep_Colony).has_value())) return BWAPI::UnitTypes::Zerg_Creep_Colony;
	else return colony;
}


static BWAPI::UnitType getNextBuilding() {
	if (!(Tools::getUnitOfType(BWAPI::UnitTypes::Zerg_Hatchery).has_value())) return BWAPI::UnitTypes::Zerg_Hatchery;
	if (!(Tools::getUnitOfType(BWAPI::UnitTypes::Zerg_Spawning_Pool).has_value())) return BWAPI::UnitTypes::Zerg_Spawning_Pool;

	if (!(Tools::getUnitOfType(BWAPI::UnitTypes::Zerg_Sunken_Colony).has_value())) return buildColony(BWAPI::UnitTypes::Zerg_Sunken_Colony);

	if (!(Tools::getUnitOfType(BWAPI::UnitTypes::Zerg_Evolution_Chamber).has_value())) return BWAPI::UnitTypes::Zerg_Evolution_Chamber;

	const int hatcheryCount = Tools::countUnitsOfType(BWAPI::UnitTypes::Zerg_Hatchery, BWAPI::Broodwar->self()->getUnits());
	const int sunkenCount = Tools::countUnitsOfType(BWAPI::UnitTypes::Zerg_Sunken_Colony, BWAPI::Broodwar->self()->getUnits());
	const int sporeCount = Tools::countUnitsOfType(BWAPI::UnitTypes::Zerg_Spore_Colony, BWAPI::Broodwar->self()->getUnits());

	if (sunkenCount < hatcheryCount - 2) return buildColony(BWAPI::UnitTypes::Zerg_Sunken_Colony);

	if (sporeCount < hatcheryCount) return buildColony(BWAPI::UnitTypes::Zerg_Spore_Colony);

	return BWAPI::UnitTypes::Zerg_Hatchery;
}

static void trainTypeOrSupply(BWAPI::UnitType type) {
	const int supplyUsed = BWAPI::Broodwar->self()->supplyUsed();
	const int supplyTotal = BWAPI::Broodwar->self()->supplyTotal();
	if (supplyUsed + type.supplyRequired() >= supplyTotal) Tools::trainTroop(BWAPI::Broodwar->self()->getRace().getSupplyProvider());
	else Tools::trainTroop(type);
}

static bool findMorphingOverlord() {
	for (const auto& unit : BWAPI::Broodwar->self()->getUnits()) {
		if (Tools::compareUnitTypes(unit->getType(), BWAPI::UnitTypes::Zerg_Overlord) && unit->isMorphing()) {
			return true;
		}
	}
	return false;
}

static bool findBuildingBuilding() {
	for (const auto& unit : BWAPI::Broodwar->self()->getUnits()) {
		if (unit->getType().isBuilding() && unit->getRemainingBuildTime() > 0) {
			return true;
		}
	}
	return false;
}

void ProductionManager::trainWorkerOrSupply() {
	const int supplyUsed = BWAPI::Broodwar->self()->supplyUsed();
	const int supplyTotal = BWAPI::Broodwar->self()->supplyTotal();
	if (supplyUsed + BWAPI::Broodwar->self()->getRace().getWorker().supplyRequired() > supplyTotal) {
		if (buildingSupply) return;
		buildingSupply = Tools::trainTroop(BWAPI::Broodwar->self()->getRace().getSupplyProvider());
	}
	else if (!findMorphingOverlord()) {
		Tools::createWorker();
		buildingSupply = false;
	}
}

void ProductionManager::update() {
	if (frameDelay > 0) {
		--frameDelay;
		return;
	}

	if (state == State::ATTACK) {
		if (Squad::getSquadResetCount() > 5) {
			printf("switching to defend state\n");
			state = State::DEFEND;
			update();
			return;
		}
		trainTypeOrSupply(BWAPI::UnitTypes::Zerg_Zergling);
	}
	else if (state == State::PREPARE) {
		if (Tools::buildBuilding(BWAPI::UnitTypes::Zerg_Spawning_Pool, Tools::getPoolPlacement())) {
			state = State::ATTACK;
			printf("switching to attack state\n");
		}
	}
	else {
		auto task_it = buildOrder.begin();
		while (task_it != buildOrder.end() && ((*task_it)->isDone() || (*task_it)->isInProgress())) {
			(*task_it)->updateState();
			++task_it;
		}
		//printf("findBuildingBuilding():%d building: %d\n", findBuildingBuilding(), building);
		if (task_it == buildOrder.end()) {
			if (orderGiven) return;
			if (larvaExists()) {
				trainWorkerOrSupply();
				orderGiven = buildingSupply;
			}
			else {
				orderGiven = Tools::buildBuilding(getNextBuilding());
			}
		}
		else {
			(*task_it)->resolve();
		}
	}
	frameDelay = 5;
}

void ProductionManager::addTask(std::shared_ptr<Task>& task) {
	buildOrder.emplace_back(task);
}

void ProductionManager::resetOrder() {
	orderGiven = false;
}