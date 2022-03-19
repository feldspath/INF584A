#include "Task.h"
#include "Tools.h"
#include <optional>

Task::Task() {}

Task::Task(int requiredUnits_) : requiredUnits(requiredUnits_) {}

bool ProductionTask::resolve() {
	const size_t currentUnits = BWAPI::Broodwar->self()->supplyUsed()/2;

	// if supply requirements not met, train workers
	if (currentUnits < requiredUnits) {
		Tools::createWorker();
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