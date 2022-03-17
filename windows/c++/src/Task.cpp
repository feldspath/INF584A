#include "Task.h"
#include "Tools.h"
#include <optional>

Task::Task() {}

Task::Task(int requiredUnits_) : requiredUnits(requiredUnits_) {}

static void createWorker() {
	const int minerals = BWAPI::Broodwar->self()->minerals();
	const BWAPI::UnitType worker = BWAPI::Broodwar->self()->getRace().getWorker();
	if (minerals < worker.mineralPrice()) return;

	const auto depot = Tools::getIdleDepot();
	if (depot.has_value() && depot.value()->train(worker)) {
		BWAPI::Broodwar->printf("Started training new worker");
	}
}

bool ProductionTask::resolve() {
	const size_t currentUnits = BWAPI::Broodwar->self()->supplyUsed()/2;

	// if supply requirements not met, train workers
	if (currentUnits < requiredUnits) {
		createWorker();
		return false;
	}

	// 2 cases: building or unit
	if (unitType.isBuilding()) inProgress = Tools::buildBuilding(unitType);
	else inProgress = Tools::trainTroop(unitType);

	return isInProgress();
}

bool ProductionTask::isDone() const {
	size_t existingUnits = 0;
	for (auto& unit : BWAPI::Broodwar->self()->getUnits()) {
		if (unit->getType() == unitType && unit->isCompleted()) ++existingUnits;
	}
	return existingUnits >= targetAmount;
}