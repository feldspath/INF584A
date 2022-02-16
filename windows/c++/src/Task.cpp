#include "Task.h"

Task::Task() {}

Task::Task(int requiredUnits_) : requiredUnits(requiredUnits_) {}

bool ProductionTask::resolve() {
	const size_t currentUnits = BWAPI::Broodwar->self()->supplyUsed()/2;
	const int minerals = BWAPI::Broodwar->self()->minerals();
	const int gas = BWAPI::Broodwar->self()->gas();

	if (currentUnits < requiredUnits) {
		const BWAPI::Unit depot = Tools::GetDepot();
		const BWAPI::UnitType worker = BWAPI::Broodwar->self()->getRace().getWorker();
		if (depot && !depot->isTraining() && minerals >= worker.mineralPrice()) {
			depot->train(worker);
			BWAPI::Broodwar->printf("Started training new worker");
		}
		return false;
	}

	if (isDone() || isInProgress() || minerals < unitType.mineralPrice() || gas < unitType.gasPrice()) return false;

	if (unitType.isBuilding()) {
		const bool startedBuilding = Tools::BuildBuilding(unitType);
		if (startedBuilding)
		{
			inProgress = true;
			BWAPI::Broodwar->printf("Started Building %s", unitType.getName().c_str());
		}
		else {
			printf("error building %s\n", unitType.getName().c_str());
		}
		return startedBuilding;
	}
	else {
		// TODO : train troops
	}
	return false;
}

bool ProductionTask::isDone() const {
	size_t existingUnits = 0;
	for (auto& unit : BWAPI::Broodwar->self()->getUnits())
	{
		if (unit->getType() == unitType && unit->isCompleted()) ++existingUnits;
	}
	return existingUnits >= targetAmount;
}