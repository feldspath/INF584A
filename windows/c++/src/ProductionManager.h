#pragma once

#include "Task.h"
#include <vector>
#include <memory>

class ProductionManager {
private:
	std::vector<std::shared_ptr<Task>> buildOrder;

public:
	ProductionManager();

	void update();
	void addTask(std::shared_ptr<Task>& task);
};