#include "MapRegion.h"
#include "MapManager.h"
#include "Utils.h"
#include <memory>

MapAnalyzer * MapAnalyzer::insta = NULL;

template <class T>
static bool contains(std::set<T>& g, T pos) {
	return (g.find(pos) != g.end());
}

void addPoint(std::set<TilePosition>& points, const TilePosition& location) {
	if (location.isValid()) {
		points.insert(location);
	}
}

std::set<TilePosition> getSurroundingTilesInSquare(const TilePosition& center, int widthFromCenter) {
	std::set<TilePosition> result;
	for (int i = center.x - widthFromCenter; i <= center.x + widthFromCenter; ++i) {
		for (int j = center.y - widthFromCenter; j <= center.y + widthFromCenter; ++j) {
			if (i == center.x && j == center.y) 
				continue;
			addPoint(result, TilePosition(i, j));
		}
	}
	return result;
}

std::set<TilePosition> getNeighbors(const TilePosition& pos)
{
	return getSurroundingTilesInSquare(pos, 1);
}

std::vector<pTileNode> MapAnalyzer::getNeighborNodes(const TilePosition& tile, pred p) {
	std::vector<pTileNode> result;
	auto neighbors = getNeighbors(tile);
	//auto neighbors = get4Neighbours(node->pos);
	for (auto n : neighbors) {
		auto node = getNode(n);
		if (p(node))
			result.push_back(node);
	}
	return result;
}

std::vector<pTileNode> MapAnalyzer::getNodesWithValue(int value) {
	std::vector<pTileNode> result;
	for (int i = 0; i < mw; ++i) {
		for (int j = 0; j < mh; ++j) {
			auto p = nodeMap[i][j];
			if (p->value == value)
				result.push_back(p);
		}
	}
	return result;
}

void MapAnalyzer::addNode(std::set<pTileNode>& points, pTileNode node, pred p){
	if (node && node->pos.isValid() && (!p || p(node))) {
		points.insert(node);
	}
}

std::set<pTileNode> MapAnalyzer::getSurroundingNodeInSquare(const TilePosition& center, int widthFromCenter, pred p) {
	std::set<pTileNode> result;
	for (int i = center.x - widthFromCenter; i <= center.x + widthFromCenter; ++i) {
		for (int j = center.y - widthFromCenter; j <= center.y + widthFromCenter; ++j) {
			if (i == center.x && j == center.y)
				continue;
			addNode(result, getNode(i, j), p);
		}
	}
	return result;
}

std::set<pTileNode> MapAnalyzer::getNeighborNodesWithDistInSquare(const std::set<pTileNode>& square, pTileNode node, int dist, pred p) {
	const auto& neighbors = getSurroundingNodeInSquare(node->pos, dist, p);
	std::set<pTileNode> result;
	for (auto t : neighbors) {
		if (square.find(t) != square.end())
			result.insert(t);
	}
	return result; // TODO test
}
void MapAnalyzer::CalculateDistancesToUnWalkables() {
	auto isUnexplored = [](pTileNode pos) {
		return pos->value == -1;
	};

	for (int i = 0; i < maxDistance; ++i) {
		auto nodesWithValue = getNodesWithValue(i);
		for (auto node : nodesWithValue) {
			auto neighborNodes = getNeighborNodes(node->pos, isUnexplored);
			for (auto nn : neighborNodes) {
				nn->value = i + 1;
			}
		}
	}

	// the unexplored left are set to maxDistance
	auto nodesWithValue = getNodesWithValue(-1);
	for (auto node : nodesWithValue) {
		node->value = maxDistance;
	}
}

void MapAnalyzer::CalcChokePoints() {
	for (int i = 0; i < mw; ++i) {
		for (int j = 0; j < mh; ++j) {
			auto nodeData = nodeMap[i][j];
			const auto &neighbors = getNeighbors(nodeData->pos);
			if (nodeData->value == maxDistance || nodeData->value >2 || nodeData->value == 0) continue;

			nodeData->inChokepoint = true;
			for (auto n : neighbors) {
				auto posData = getNode(n);
				if (posData->value > nodeData->value) {
					nodeData->inChokepoint = false;
					break;
				}
			}
		}
	}
}

bool visited(std::set<TilePosition>& visited, const TilePosition& pos) {
	return visited.find(pos) != visited.end();
};

UnwalkableArea MapAnalyzer::getUnwalkableGroup(std::set<TilePosition>& visited, const TilePosition& pos) {
	UnwalkableArea area;
	area.fillAreaAround(pos);

	for (auto t : area.tiles) {
		visited.insert(t);
	}
	return area;
}

UnwalkableAreaSet MapAnalyzer::getUnwalkableGroups() {
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
					result.push_back(group);
					count++;
				}
			}
		}
	}
	std::cout << "getUnwalkableGroups done!" << count<<"\n";
	return result;
}

void MapAnalyzer::closeLine(pTileNode f, pTileNode s) {
	// brezenham algorithm for drawing lines
	int x0 = f->pos.x, y0 = f->pos.y, x1 = s->pos.x, y1 = s->pos.y;
	int dx = abs(x1 - x0), sx = x0<x1 ? 1 : -1;
	int dy = abs(y1 - y0), sy = y0<y1 ? 1 : -1;
	int err = (dx>dy ? dx : -dy) / 2, e2;

	for (;;){
		getNode(x0, y0)->closed = true;
		closed.push_back(*getNode(x0, y0));
		if (x0 == x1 && y0 == y1) break;
		e2 = err;
		if (e2 >-dx) { err -= dy; x0 += sx; }
		if (e2 < dy) { err += dx; y0 += sy; }
	}
}

void MapAnalyzer::closeChokepoint(std::set<pTileNode>& chokePoint) {
	auto el = *chokePoint.begin();
	pred unwalkable = [](pTileNode n) {
		return n->value == 0;
	};
	const auto& neighbors = getSurroundingNodeInSquare(el->pos, 4, unwalkable);
	auto groups = getGroups(neighbors, 1);

	for (auto g : groups) {
		auto pointFromGroup = *g.begin();
		closeLine(pointFromGroup, el);
	}
}

void MapAnalyzer::CalculateChokepoints() {
	auto mw = Broodwar->mapWidth();
	auto mh = Broodwar->mapHeight();
	const auto& groupedUnwalkables = getUnwalkableGroups();

	for (auto g : groupedUnwalkables) {
		if (g.isSignificant()) {
			for (auto t : g.tiles) {
				auto node = getNode(t);
				if (t.y != mh - 1)
					node->value = 0;
				else
					node->value = getNode(t.x, t.y - 1)->value;
			}
		}
	}

	CalculateDistancesToUnWalkables();
	CalcChokePoints();

	std::vector<pTileNode> tmp;
	tmp = getNodes([](pTileNode n){return n->inChokepoint; });

	for (auto it = tmp.begin(); it != tmp.end(); it++) {
		auto c = *it;
		if (!isCornerChokepoint(c)) {
			cp.insert(c);
		}
	}

	auto cps = getGroups(cp, 1);
	std::vector<ChokePoint> chokpoints;
	for (int i = 0; i < cps.size(); ++i) {
		closeChokepoint(cps[i]);
		for (auto c : cps[i]) {
			c->chokePointId = i;
		}
		auto cp = pChokePoint(new ChokePoint(cps[i], nodeMap));
		cp->id = i;
		chokpts[i] = cp;
	}
}

void MapAnalyzer::CalculateRegions() {
	getRegionGroups();

	for (auto cp : chokpts) {
		cp.second->calcAdjacentRegions(regions);
	}
	for (auto r : regions) {
		r.second->calcAdjacentChokepoints(chokpts);
		r.second->calcAdjacentRegions(regions);
	}

	for (auto r : regions) {
		auto adjacentRegions = r.second->adjacentRegions;
		if(r.second->area() > REGION_MIN_SIZE) {
			for (auto ar : adjacentRegions) {
				if (ar->area() < REGION_MIN_SIZE) {
					r.second->merge(ar);
					r.second->calcAdjacentChokepoints(chokpts);
					r.second->calcAdjacentRegions(regions);
				}
			}
		}
	}
}

void UnwalkableArea::fillAreaAround(const TilePosition& tile) {
	if (Broodwar->isWalkable(WalkPosition(tile)) || this->contains(tile)) return;

	tiles.insert(tile);
	const auto& neighbors = getNeighbors(tile);
	for (auto n : neighbors) {
		fillAreaAround(n);
	}
}

//corner "chokepoints" are to be removed from the chokepoint list

void MapAnalyzer::getUnwalkableGroupInSquare(const std::set<pTileNode>& square, pTileNode node, std::set<pTileNode>& result, pred p) {
	if (!node->value && !contains(result, node)) {
		result.insert(node);

		const auto& neighbors = getNeighborNodesWithDistInSquare(square, node, 2, p);
		for (auto n : neighbors) {
			getUnwalkableGroupInSquare(square, n, result, p);
		}
	}
}

bool MapAnalyzer::isCornerChokepoint(pTileNode node) {
	std::set<pTileNode> group;
	auto unwalkable = [](pTileNode n){return n->value == 0; };
	const auto& surroundingTile = getSurroundingNodeInSquare(node->pos, 4, unwalkable);
	//surroundingTile.insert(node);
	
	if (surroundingTile.size()) {
		auto first = *surroundingTile.begin();
		std::set<pTileNode> visited;
		auto notvisited = [&visited](pTileNode n) {
			return !contains(visited, n);
		};
		getGroup(visited, surroundingTile, first, 1, notvisited, group);
		//getUnwalkableGroupInSquare(surroundingTile, first, group, unwalkable);
		if (group.size() != surroundingTile.size())
			return false;
		return true;
	}
	
	return false;
}

void MapAnalyzer::getRegionGroup(std::set<pTileNode>& visited,
	pTileNode node,
	int id,
	pRegion result)
{
	std::set<pTileNode> lVisited;
	auto notVisited = [&lVisited](pTileNode n){
		return lVisited.find(n) == lVisited.end();
	};
	getRegionGroupInt(lVisited, node, notVisited, id, result);

	for (auto v : lVisited) {
		if (!v->closed && v->value != 0)
			visited.insert(v);
	}
}
void MapAnalyzer::getRegionGroupInt(std::set<pTileNode>& visited,
	pTileNode node,
	pred p,
	int id,
	pRegion result)
{
	result->tiles.push_back(node);
	node->regionId = id;
	visited.insert(node);

	const auto& neighbors = getNeighborNodeFour(node, p);
	for (auto n : neighbors) {
		if (n->closed || n->value == 0) {
			visited.insert(n);
			result->border.insert(n);
		}
		else {
			getRegionGroupInt(visited, n, p, id, result);
		}
	}
}

void MapAnalyzer::getRegionGroups() {
	std::set<pTileNode> visited;
	auto nextRegionId = 0;

	for (int i = 0; i < mw; ++i) {
		for (int j = 0; j < mh; ++j){
			auto current = nodeMap[i][j];
			if (current->value != 0 && !current->closed) {
				if (visited.find(current) == visited.end()) {
					pRegion group = pRegion(new MapRegion(nodeMap));
					getRegionGroup(visited, current, nextRegionId, group);
					if (group->area() != 0) {
						group->id = nextRegionId++;
						regions[group->id] = group;
					}
				}
			}
		}
	}
}

bool ChokePoint::inAdjacentRegion(int id) {
	for (auto r : adjacentRegions) {
		if (r->getId() == id)
			return true;
	}
	return false;
}
