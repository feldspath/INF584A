#pragma once

#include "Worker.h"
#include <vector>
#include <BWAPI.h>

class WorkManager {
private:
	std::vector<Worker> workers;

	std::vector<Worker> miners;


public:
	void updateWorkersState();
	void updateWorkersJob();
	void assignNewWorker(BWAPI::Unit unit);
};