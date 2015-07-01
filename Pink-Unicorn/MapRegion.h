#ifndef MAP_R_H
#define MAP_R_H

#include "DebugStuff.h"
#include "ManagerBase.h"
#include <BWAPI.h>
#include <BWAPI\Client.h>
#include <memory>
#include <vector>
#include <assert.h>

using namespace BWAPI;

class MapRegion;
class RegionSet;

class ConnectedComponent {
public:
	std::set<TilePosition> tiles;
	ConnectedComponent(){};
	bool contains(const TilePosition& tile) {
		return tiles.find(tile) != tiles.end();
	}
	void addTile(const TilePosition& tile) {
		tiles.insert(tile);
	}
	TilePosition getCenter() {
		auto result = TilePosition(0, 0);
		for (auto t : tiles) {
			result += t;
		}
		result /= tiles.size();
	}

	int getArea() {
		return tiles.size();
	}
};

class UnwalkableArea : public ConnectedComponent {
public:
	bool isSignificant() {
		return getArea() > 2;
	}
	void fillAreaAround(const TilePosition& tile);
};

class UnwalkableAreaSet : public std::vector<UnwalkableArea>{
public:
	int nextId;
	UnwalkableAreaSet() : nextId(0){};
	int addArea(const UnwalkableArea& area) {
		push_back(area);
		return nextId++;
	}

	bool isInSignificantArea(const TilePosition& tile) {
		for (auto a : *this) {
			if (a.isSignificant() && a.contains(tile))
				return true;
		}
		return false;
	}

	bool contains(const TilePosition& tile) {
		for (auto a : *this) {
			if (a.contains(tile))
				return true;
		}
		return false;
	}
};
#endif //MAP_R_H
