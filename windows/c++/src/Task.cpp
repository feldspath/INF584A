#include "Task.h"
#include "Tools.h"

Task::Task() {}

Task::Task(int requiredUnits_) : requiredUnits(requiredUnits_) {}

bool ProductionTask::resolve() {
	const size_t currentUnits = BWAPI::Broodwar->self()->supplyUsed()/2;
	const int minerals = BWAPI::Broodwar->self()->minerals();
	const int gas = BWAPI::Broodwar->self()->gas();

	if (currentUnits < requiredUnits) {
		const auto depot = Tools::GetDepot();
		const BWAPI::UnitType worker = BWAPI::Broodwar->self()->getRace().getWorker();
		if (depot.has_value() && !depot.value()->isTraining() && minerals >= worker.mineralPrice()) {
			if (depot.value()->train(worker)) BWAPI::Broodwar->printf("Started training new worker");
		}
		return false;
	}

	if (isInProgress() || minerals < unitType.mineralPrice() || gas < unitType.gasPrice()) return false;

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
		if (BWAPI::Broodwar->self()->getRace().getID() == BWAPI::Races::Zerg.getID()) {
			const auto pool = BWAPI::UnitTypes::Enum::Zerg_Spawning_Pool;
			if (!(Tools::GetUnitOfType(pool).has_value())) return false;
		}

		const auto unitBuilder = unitType.whatBuilds().first;
		const auto buildUnit = Tools::GetUnitOfType(unitBuilder);
		if (!(buildUnit.has_value())) return false;
		else {
			if (buildUnit.value()->train(unitType)) {
				BWAPI::Broodwar->printf("Started training %s", unitType.getName().c_str());
				inProgress = true;
				return true;
			}
			return false;
		}
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