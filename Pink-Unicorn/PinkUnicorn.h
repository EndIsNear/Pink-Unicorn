#ifndef OUR_AI_MODULE_H
#define OUR_AI_MODULE_H
#include <BWAPI.h>

#include "ManagerBase.h"
class PinkUnicorn
{
	ManagersCollection mManagers;
public:
	PinkUnicorn();
	~PinkUnicorn()
	{
		for (auto m : mManagers)
			m->ReleaseInst();

		//ugly hack to clear set events
		const auto &events = BWAPI::Broodwar->getEvents();
		const_cast<std::list<BWAPI::Event>&>(events).clear();
	}
	virtual void onStart();
	virtual void onEnd(bool isWinner);
	virtual void onFrame();
	virtual void onSendText(std::string text);
	virtual void onReceiveText(BWAPI::Player player, std::string text);
	virtual void onPlayerLeft(BWAPI::Player player);
	virtual void onNukeDetect(BWAPI::Position target);
	virtual void onUnitDiscover(BWAPI::Unit unit);
	virtual void onUnitEvade(BWAPI::Unit unit);
	virtual void onUnitShow(BWAPI::Unit unit);
	virtual void onUnitHide(BWAPI::Unit unit);
	virtual void onUnitCreate(BWAPI::Unit unit);
	virtual void onUnitDestroy(BWAPI::Unit unit);
	virtual void onUnitMorph(BWAPI::Unit unit);
	virtual void onUnitRenegade(BWAPI::Unit unit);
	virtual void onSaveGame(std::string gameName);
	virtual void onUnitComplete(BWAPI::Unit unit);
};

#endif //OUR_AI_MODULE_H