#include <BWAPI.h>
#include <BWAPI/Client.h>

using namespace BWAPI;


#include "Utils.h"

void drawStartPos()
{
	for (auto& SL : Broodwar->getStartLocations())
	{
		Broodwar->drawEllipseMap(SL.x * 32 + 64, SL.y * 32 + 56, 72, 44, Colors::Green);
		std::cout << SL << std::endl;
		Broodwar->drawTextMap(SL.x * 32 + 48, SL.y * 32 + 48, "Start Position");
	}
}

void drawRegions(int minPriority)
{
	for (auto &reg : Broodwar->getAllRegions())
	{
		if (reg->getDefensePriority() >= minPriority)
		{
			auto center = reg->getCenter();
			Broodwar->drawCircle(CoordinateType::Map, center.x, center.y, 12, Colors::Green);
			Broodwar->drawTextMap(center, "ID:%d GID: %d DP:%d IW:%d", reg->getID(), reg->getRegionGroupID(), reg->getDefensePriority(), reg->isAccessible());
		}
	}
}

void drawStats()
{
	int line = 1;
	Broodwar->drawTextScreen(5, 0, "I have %d units:", Broodwar->self()->allUnitCount());
	for (auto& unitType : UnitTypes::allUnitTypes())
	{
		int count = Broodwar->self()->allUnitCount(unitType);
		if (count)
		{
			Broodwar->drawTextScreen(5, 16 * line, "- %d %s%c", count, unitType.c_str(), count == 1 ? ' ' : 's');
			++line;
		}
	}
}

void drawBullets()
{
	for (auto &b : Broodwar->getBullets())
	{
		Position p = b->getPosition();
		double velocityX = b->getVelocityX();
		double velocityY = b->getVelocityY();
		Broodwar->drawLineMap(p, p + Position((int)velocityX, (int)velocityY), b->getPlayer() == Broodwar->self() ? Colors::Green : Colors::Red);
		Broodwar->drawTextMap(p, "%c%s", b->getPlayer() == Broodwar->self() ? Text::Green : Text::Red, b->getType().c_str());
	}
}

void drawVisibilityData()
{
	int wid = Broodwar->mapHeight(), hgt = Broodwar->mapWidth();
	for (int x = 0; x < wid; ++x)
	for (int y = 0; y < hgt; ++y)
	{
		if (Broodwar->isExplored(x, y))
			Broodwar->drawDotMap(x * 32 + 16, y * 32 + 16, Broodwar->isVisible(x, y) ? Colors::Green : Colors::Blue);
		else
			Broodwar->drawDotMap(x * 32 + 16, y * 32 + 16, Colors::Red);
	}
}

void showPlayers()
{
	Playerset players = Broodwar->getPlayers();
	for (auto p : players)
		Broodwar << "Player [" << p->getID() << "]: " << p->getName() << " is in force: " << p->getForce()->getName() << std::endl;
}

void showForces()
{
	Forceset forces = Broodwar->getForces();
	for (auto f : forces)
	{
		Playerset players = f->getPlayers();
		Broodwar << "Force " << f->getName() << " has the following players:" << std::endl;
		for (auto p : players)
			Broodwar << "  - Player [" << p->getID() << "]: " << p->getName() << std::endl;
	}
}


bool IsRoboticUnit(BWAPI::UnitType type)
{
	if (type == BWAPI::UnitTypes::Protoss_Zealot ||
		type == BWAPI::UnitTypes::Protoss_Dragoon ||
		type == BWAPI::UnitTypes::Protoss_High_Templar ||
		type == BWAPI::UnitTypes::Protoss_Dark_Templar)
		return true;
	return false;
}

bool IsStargateUnit(BWAPI::UnitType type)
{
	if (type == BWAPI::UnitTypes::Protoss_Scout ||
		type == BWAPI::UnitTypes::Protoss_Corsair ||
		type == BWAPI::UnitTypes::Protoss_Arbiter ||
		type == BWAPI::UnitTypes::Protoss_Carrier)
		return true;
	return false;
}

bool IsGatewayUnit(BWAPI::UnitType type)
{
	if (type == BWAPI::UnitTypes::Protoss_Reaver ||
		type == BWAPI::UnitTypes::Protoss_Shuttle ||
		type == BWAPI::UnitTypes::Protoss_Observer)
		return true;
	return false;
}



