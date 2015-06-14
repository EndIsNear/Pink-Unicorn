#ifndef PINK_UNICORN_PRODUCE_MANAGER_H
#define PINK_UNICORN_PRODUCE_MANAGER_H

#include "ManagerBase.h"

class ProduceManager : public ManagerBase
{
public:
	virtual void OnFrame() override;
	virtual void OnUnitComplete(BWAPI::Unit unit) override;
};


#endif