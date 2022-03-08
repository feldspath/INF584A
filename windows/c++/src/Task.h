#pragma once

#include "Tools.h"
#include <BWAPI.h>


class Task {

protected:
	size_t requiredUnits;
	bool inProgress = false;

public:
	Task();
	Task(int requiredUnits_);
	virtual bool isDone() const = 0;
	virtual bool resolve() = 0;
	bool isInProgress() const { return inProgress; }
};

class ProductionTask : public Task {
private:
	BWAPI::UnitType unitType;
	size_t targetAmount;

public:
	ProductionTask(BWAPI::UnitType unitType_, int targetAmount_, int requiredUnits_) : unitType(unitType_), targetAmount(targetAmount_), Task(requiredUnits_) {}

	bool isDone() const override;
	bool resolve() override;
};

class UpgradeTask : public Task {
	BWAPI::UpgradeType upgrade;
};

class ResearchTask : public Task {
	BWAPI::TechType research;
};