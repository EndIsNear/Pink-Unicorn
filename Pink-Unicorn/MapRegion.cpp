#include "MapRegion.h"
#include "MapManager.h"
#include <memory>

static void addPoint(std::vector<TilePosition>& points, const TilePosition& location) {
	if (location.isValid()) {
		points.push_back(location);
	}
}

static std::vector<TilePosition> getSurroundingTilesInSquare(const TilePosition& center, int widthFromCenter) {
	std::vector<TilePosition> result;
	for (int i = center.x - widthFromCenter; i <= center.x + widthFromCenter; ++i) {
		for (int j = center.y - widthFromCenter; j <= center.y + widthFromCenter; ++j) {
			if (i == center.x && j == center.y) 
				continue;
			addPoint(result, TilePosition(i, j));
		}
	}
	return result;
}

static std::vector<TilePosition> getNeighbours(const TilePosition& pos)
{
	return getSurroundingTilesInSquare(pos, 1);
}

static std::vector<pTileNode> getNeighborNodes(std::map<TilePosition, pTileNode>& map, pTileNode node, pred p) {
	std::vector<pTileNode> result;
	auto neighbors = getNeighbours(node->pos);
	//auto neighbors = get4Neighbours(node->pos);
	for (auto n : neighbors) {
		auto node = map[n];
		if (p(node))
			result.push_back(node);
	}
	return result;
}

std::vector<pTileNode> getNodesWithValue(std::map < TilePosition, pTileNode>& map, int value) {
	std::vector<pTileNode> result;
	for (auto p : map) {
		if (p.second->value == value)
			result.push_back(p.second);
	}
	return result;
}

void CalculateDistancesToUnWalkables(std::map < TilePosition, pTileNode>& map) {
	auto isUnexplored = [](pTileNode pos) {
		return pos->value == -1;
	};

	for (int i = 0; i < maxDistance; ++i) {
		auto nodesWithValue = getNodesWithValue(map, i);
		for (auto node : nodesWithValue) {
			auto neighborNodes = getNeighborNodes(map, node, isUnexplored);
			for (auto nn : neighborNodes) {
				nn->value = i + 1;
			}
		}
	}

	// the unexplored left are set to maxDistance
	auto nodesWithValue = getNodesWithValue(map, -1);
	for (auto node : nodesWithValue) {
		node->value = maxDistance;
	}
}

void CalcChokePoints(std::map<TilePosition, pTileNode>& map) {
	for (auto p : map) {
		auto neighbors = getNeighbours(p.first);
		auto nodeData = p.second;
		if (nodeData->value == maxDistance || nodeData->value >2 || nodeData->value == 0) continue;

		nodeData->inChokepoint = true;
		for (auto n : neighbors) {
			auto posData = map[n];
			if (posData->value > nodeData->value) {
				nodeData->inChokepoint = false;
				break;
			}
		}
	}
}

bool visited(std::set<TilePosition>& visited, const TilePosition& pos) {
	return visited.find(pos) != visited.end();
};

UnwalkableArea getUnwalkableGroup(std::set<TilePosition>& visited, const TilePosition& pos) {
	UnwalkableArea area;
	area.fillAreaAround(pos);

	for (auto t : area.tiles) {
		visited.insert(t);
	}
	return area;
}

UnwalkableAreaSet getUnwalkableGroups() {
	auto mw = Broodwar->mapWidth();
	auto mh = Broodwar->mapHeight();
	UnwalkableAreaSet result;
	std::set<TilePosition> v;
	int count = 0;

	for (int i = 0; i < mw; ++i) {
		for (int j = 0; j < mh; ++j) {
			auto current = TilePosition(i, j);
			if (!visited(v, current)) {
				v.insert(current);
				auto group = getUnwalkableGroup(v, current);
				if (group.tiles.size() != 0) {
					result.addArea(group);
					count++;
				}
					
				
			}
		}
	}
	std::cout << "getUnwalkableGroups done!" << count<<"\n";
	return result;
}

void MapManager::CalculateChokepoints() {
	auto mw = Broodwar->mapWidth();
	auto mh = Broodwar->mapHeight();
	std::map<TilePosition, pTileNode> map;
	auto groupedUnwalkables = getUnwalkableGroups();

	for (int i = 0; i < mw; ++i) {
		for (int j = 0; j < mh; ++j) {
			TilePosition current(i, j);
			//auto node = calcClosestUnwalkable(current, mw, mh);
			auto node = pTileNode(new tileNode);
			node->inChokepoint = false;
			node->pos = current;
			if (!Broodwar->isWalkable(WalkPosition(current))) {
				if (groupedUnwalkables.isInSignificantArea(current))
					node->value = 0;
			}

			map.insert (std::make_pair(current, node));
		}
	}

	//std::cout << "removing unsignificant groups done!\n";

	CalculateDistancesToUnWalkables(map);
	CalcChokePoints(map);
	for (auto m : map) {
		if (m.second->inChokepoint)
			chokepoints.push_back(m.second);
		allnodes.push_back(m.second);
	}
}

void UnwalkableArea::fillAreaAround(const TilePosition& tile) {
	if (Broodwar->isWalkable(WalkPosition(tile)) || contains(tile)) return;

	tiles.insert(tile);
	auto neighbors = getNeighbours(tile);
	for (auto n : neighbors) {
		fillAreaAround(n);
	}
}