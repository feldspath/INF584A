#include "ProductionManager.h"

ProductionManager::ProductionManager() {
	buildOrder.reserve(6);
	buildOrder.push_back(std::make_shared<ProductionTask>(BWAPI::UnitTypes::Enum::Terran_Supply_Depot, 1, 9));
	buildOrder.push_back(std::make_shared<ProductionTask>(BWAPI::UnitTypes::Enum::Terran_Barracks, 1, 11));
	buildOrder.push_back(std::make_shared<ProductionTask>(BWAPI::UnitTypes::Enum::Terran_Barracks, 2, 13));
	buildOrder.push_back(std::make_shared<ProductionTask>(BWAPI::UnitTypes::Enum::Terran_Supply_Depot, 2, 15));
	buildOrder.push_back(std::make_shared<ProductionTask>(BWAPI::UnitTypes::Enum::Terran_Refinery, 1, 16));
	buildOrder.push_back(std::make_shared<ProductionTask>(BWAPI::UnitTypes::Enum::Terran_Supply_Depot, 3, 17));
}

void ProductionManager::update() {
	auto task_it = buildOrder.begin();
	while (task_it != buildOrder.end() && (*task_it)->isDone()) {
		buildOrder.erase(task_it);
		task_it = buildOrder.begin();
	}
	while (task_it != buildOrder.end() && (*task_it)->isInProgress()) ++task_it;
	if (task_it == buildOrder.end()) {
		BWAPI::Broodwar->printf("End of build order");
	}
	else {
		(*task_it)->resolve();
	}
}

void ProductionManager::addTask(std::shared_ptr<Task>& task) {
	buildOrder.emplace_back(task);
}