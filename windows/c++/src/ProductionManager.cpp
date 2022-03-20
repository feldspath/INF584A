#include "ProductionManager.h"
#include "Squad.h"
#include "Tools.h"
#include <optional>

ProductionManager::ProductionManager() : state(State::PREPARE) {
	buildOrder.push_back(std::make_shared<ProductionTask>(BWAPI::UnitTypes::Enum::Zerg_Hatchery, 1, 0));
	buildOrder.push_back(std::make_shared<ProductionTask>(BWAPI::UnitTypes::Enum::Zerg_Spawning_Pool, 1, 5));
}

void ProductionManager::update() {
	if (frameDelay > 0) {
		--frameDelay;
		return;
	}
	frameDelay = 5;

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

		if (currentBuildingOrder.has_value()) return;
		if (task_it == buildOrder.end()) {
			if (larvaExists() && trainWorkerOrSupply()) {
				//printf("order given: build supply\n");
				lastOrderIsSupply = true;
				currentBuildingOrder = BWAPI::Broodwar->self()->getRace().getSupplyProvider();
			}
			if (currentBuildingOrder.has_value()) return;
			const auto nextBuilding = getNextBuilding();
			if (Tools::buildBuilding(nextBuilding)) {
				//printf("order given: build\n");
				currentBuildingOrder = nextBuilding;
			}
		}
		else {
			if ((*task_it)->resolve()) {
				//printf("order given: build\n");
				if (const auto productionTask = dynamic_cast<ProductionTask*>((*task_it).get())) {
					currentBuildingOrder = productionTask->getUnitType();
				}
			}
		}
	}
}

std::optional<BWAPI::Unit> ProductionManager::getExistingUnit(BWAPI::UnitType type) {
	for (const auto& unit : BWAPI::Broodwar->self()->getUnits()) {
		if (Tools::compareUnitTypes(unit->getType(), type) && unit->exists()) {
			return unit;
		}
	}
	return std::nullopt;
}

bool ProductionManager::larvaExists() {
	return getExistingUnit(BWAPI::UnitTypes::Zerg_Larva).has_value();
}

BWAPI::UnitType ProductionManager::buildColony(BWAPI::UnitType colony) {
	if (!(getExistingUnit(BWAPI::UnitTypes::Zerg_Creep_Colony).has_value())) return BWAPI::UnitTypes::Zerg_Creep_Colony;
	else return colony;
}

BWAPI::UnitType ProductionManager::getNextBuilding() {
	if (!(Tools::getUnitOfType(BWAPI::UnitTypes::Zerg_Sunken_Colony).has_value())) return buildColony(BWAPI::UnitTypes::Zerg_Sunken_Colony);

	if (!(Tools::getUnitOfType(BWAPI::UnitTypes::Zerg_Evolution_Chamber).has_value())) return BWAPI::UnitTypes::Zerg_Evolution_Chamber;

	const int hatcheryCount = Tools::countUnitsOfType(BWAPI::UnitTypes::Zerg_Hatchery, BWAPI::Broodwar->self()->getUnits());
	const int sunkenCount = Tools::countUnitsOfType(BWAPI::UnitTypes::Zerg_Sunken_Colony, BWAPI::Broodwar->self()->getUnits());
	const int sporeCount = Tools::countUnitsOfType(BWAPI::UnitTypes::Zerg_Spore_Colony, BWAPI::Broodwar->self()->getUnits());

	if (sunkenCount < hatcheryCount * hatcheryCount + 2) return buildColony(BWAPI::UnitTypes::Zerg_Sunken_Colony);

	if (sporeCount < (hatcheryCount * hatcheryCount + 2) / 2) return buildColony(BWAPI::UnitTypes::Zerg_Spore_Colony);

	return BWAPI::UnitTypes::Zerg_Hatchery;
}

void ProductionManager::trainTypeOrSupply(BWAPI::UnitType type) {
	const int supplyUsed = BWAPI::Broodwar->self()->supplyUsed();
	const int supplyTotal = BWAPI::Broodwar->self()->supplyTotal();
	if (supplyUsed + type.supplyRequired() >= supplyTotal) Tools::trainTroop(BWAPI::Broodwar->self()->getRace().getSupplyProvider());
	else Tools::trainTroop(type);
}

bool ProductionManager::findBuildingBuilding() {
	for (const auto& unit : BWAPI::Broodwar->self()->getUnits()) {
		if (unit->getType().isBuilding() && unit->getRemainingBuildTime() > 0) {
			return true;
		}
	}
	return false;
}

// returns true if building new supply, false otherwise
bool ProductionManager::trainWorkerOrSupply() {
	const int supplyUsed = BWAPI::Broodwar->self()->supplyUsed();
	const int supplyTotal = BWAPI::Broodwar->self()->supplyTotal();
	if (supplyUsed + BWAPI::Broodwar->self()->getRace().getWorker().supplyRequired() >= supplyTotal && !lastOrderIsSupply) {
		return Tools::trainTroop(BWAPI::Broodwar->self()->getRace().getSupplyProvider());
	}
	else {
		Tools::createWorker();
		return false;
	}
}

void ProductionManager::addTask(std::shared_ptr<Task>& task) {
	buildOrder.emplace_back(task);
}

void ProductionManager::resetOrder(BWAPI::UnitType type) {
	if (currentBuildingOrder.has_value() && Tools::compareUnitTypes(type, currentBuildingOrder.value())) {
		//printf("reset order\n");
		if (!(Tools::compareUnitTypes(type, BWAPI::Broodwar->self()->getRace().getSupplyProvider()))) {
			lastOrderIsSupply = false;
		}
		currentBuildingOrder = std::nullopt;
	}
}