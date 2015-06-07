#ifndef OUR_AI_MODULE_H
#define OUR_AI_MODULE_H
#include <BWAPI.h>

#include "Utils.h"
#include "MapManager.h"
class PinkUnicorn
{
public:
	PinkUnicorn(){}
	~PinkUnicorn(){}
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
private:
	//size_t			m_FrameCnt;
	BWAPI::UnitType m_SupplyProviderType;
};

#endif //OUR_AI_MODULE_H