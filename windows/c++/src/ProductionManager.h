#pragma once

#include "Task.h"
#include <vector>
#include <optional>
#include <memory>

class ProductionManager {
private:
	enum class State {
		PREPARE, ATTACK, DEFEND
	};

	std::vector<std::shared_ptr<Task>> buildOrder;
	int frameDelay = 0;
	State state;
	std::optional<BWAPI::UnitType> currentBuildingOrder = std::nullopt;
	bool lastOrderIsSupply = false;

	// utility functions
	std::optional<BWAPI::Unit> getExistingUnit(BWAPI::UnitType type);
	bool larvaExists();
	BWAPI::UnitType buildColony(BWAPI::UnitType colony);
	BWAPI::UnitType getNextBuilding();
	void trainTypeOrSupply(BWAPI::UnitType type);
	bool findBuildingBuilding();
	bool trainWorkerOrSupply();

public:
	ProductionManager();

	void update();
	void addTask(std::shared_ptr<Task>& task);
	void resetOrder(BWAPI::UnitType type);
};