#pragma once

#include "Task.h"
#include <vector>
#include <memory>

class ProductionManager {
private:
	enum class State {
		PREPARE, ATTACK, DEFEND
	};

	std::vector<std::shared_ptr<Task>> buildOrder;
	int frameDelay = 0;
	State state;
	bool building = false;
	bool buildingSupply = false;
	bool orderGiven = false;
	
public:
	ProductionManager();

	void update();
	void addTask(std::shared_ptr<Task>& task);
	void trainWorkerOrSupply();
	void resetOrder();
};