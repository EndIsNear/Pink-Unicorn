#ifndef MAP_R_H
#define MAP_R_H

#include "DebugStuff.h"
#include "ManagerBase.h"
#include <BWAPI.h>
#include <BWAPI\Client.h>
#include <memory>
#include <cmath>
#include <vector>
#include <assert.h>
//#include "MapManager.h"
using namespace BWAPI;

static int maxDistance = 3;
struct tileNode;
typedef tileNode *pTileNode;
typedef std::function<bool(pTileNode)> pred;
typedef std::set<TilePosition> TPSet;
typedef std::vector<TPSet> TPSetGroup;

#define INVALID -1
#define REGION_MIN_SIZE 25
#define PI 3.14159

struct tileNode {
	TilePosition pos;
	int value; // 0 for unwalkable, positive int indicating the distance to unwalkable positions
	int regionId;
	int chokePointId;
	bool inChokepoint;
	bool closed; // used to indicate a square is between a chokepoint and it's sides, which will help to calculate the regions
	tileNode(const TilePosition& tile = TilePositions::None) :
		value(INVALID), inChokepoint(false), closed(false),
		pos(tile), chokePointId(INVALID), regionId(INVALID){};
};

class MapAnalyzer;

class MapRegion;
typedef std::shared_ptr<MapRegion> pRegion;
class ChokePoint;
typedef std::shared_ptr<ChokePoint> pChokePoint;
class RegionSet;

class ConnectedComponent;
class UnwalkableArea;
class UnwalkableAreaSet;

void addPoint(std::set<TilePosition>& points, const TilePosition& location);
std::set<TilePosition> getSurroundingTilesInSquare(const TilePosition& center, int widthFromCenter);
std::set<TilePosition> getNeighbors(const TilePosition& pos);

class MapAnalyzer {
public:
	static MapAnalyzer* GetInstance() {
		Broodwar->mapName();
		if (!insta || insta->map != Broodwar->mapName()) {
			if (insta)
				delete insta;
			insta = new MapAnalyzer();
		}
		return insta;
	}
	std::vector<tileNode> GetChokepoints() {
		return chokePoints;
	}

	std::vector<tileNode> GetClosed() {
		return closed;
	}
protected:
	pTileNode** nodeMap;
	int mw;
	int mh;
	void clear() {
		for (int i = 0; i < mw; ++i) {
			for (int j = 0; j < mh; ++j){
				delete nodeMap[i][j];
			}
			delete nodeMap[i];
		}
		delete nodeMap;
	}
	void init() {
		mw = Broodwar->mapWidth();
		mh = Broodwar->mapHeight();
		map = Broodwar->mapName();
		nodeMap = new tileNode**[mw];
		for (int i = 0; i < mw; ++i) {
			nodeMap[i] = new tileNode*[mh];
			for (int j = 0; j < mh; ++j){
				nodeMap[i][j] = new tileNode(TilePosition(i, j));
				allNodes.push_back(nodeMap[i][j]);
			}
		}
	}
	MapAnalyzer(const MapAnalyzer& m){};
	const MapAnalyzer& operator=(const MapAnalyzer& m){};
	std::string map;
	static MapAnalyzer * insta;
	std::vector<tileNode> chokePoints;
	std::set<pTileNode> cp;
	std::vector<pTileNode> allNodes;
	std::vector<tileNode> closed;

	std::map<int, pChokePoint> chokpts;
	std::map<int, pRegion> regions;

	MapAnalyzer() {
		init();
		CalculateChokepoints();
		CalculateRegions();
	}

	static void ReleaseInstance() {
		delete insta;
		insta = NULL;
	}
	~MapAnalyzer() {
		clear();
	}

	tileNode* getNode(const TilePosition& tile) {
		return nodeMap[tile.x][tile.y];
	}

	tileNode* getNode(int i, int j) {
		if (i > -1 && i < mw && j >= 0 && j < mh)
			return nodeMap[i][j];
		return nullptr;
	}
	std::vector<pTileNode> getNodes(pred p) {
		std::vector<pTileNode> result;
		for (int i = 0; i < mw; ++i) {
			for (int j = 0; j < mh; ++j){
				if (p(nodeMap[i][j])){
					result.push_back(nodeMap[i][j]); 
				}
			}
		}
		return result;
	}
	std::vector<pTileNode> getNeighborNodes(const TilePosition& node, pred p);
	std::vector<pTileNode> getNodesWithValue(int value);
	void addNode(std::set<pTileNode>& points, pTileNode node, pred p = nullptr);
	std::set<pTileNode> getSurroundingNodeInSquare(const TilePosition& center, int widthFromCenter, pred p = nullptr);
	std::set<pTileNode> getNeighborNodesWithDistInSquare(const std::set<pTileNode>& square, pTileNode node, int dist, pred p = nullptr);
	void CalculateDistancesToUnWalkables();
	void CalcChokePoints();
	void CalculateChokepoints();
	bool isCornerChokepoint(pTileNode pos);
	bool hasCornerChokepoint(const std::set<pTileNode>& cp);
	UnwalkableAreaSet getUnwalkableGroups();
	UnwalkableArea getUnwalkableGroup(std::set<TilePosition>& visited, const TilePosition& pos);
	void getGroup(std::set<pTileNode>& visited,
		const std::set<pTileNode>& container,
		pTileNode node,
		int density, pred p,
		std::set<pTileNode>& result)
	{
		result.insert(node);
		visited.insert(node);

		const auto& neighbors = getNeighborNodesWithDistInSquare(container, node, density, p);
		for (auto n : neighbors) {
			getGroup(visited, container, n, density, p, result);
		}
	}

	std::set<pTileNode> getNeighborNodeFour(pTileNode node, pred p) {
		std::set<pTileNode> result;
		addNode(result, getNode(node->pos.x - 1, node->pos.y), p);
		addNode(result, getNode(node->pos.x + 1, node->pos.y), p);
		addNode(result, getNode(node->pos.x, node->pos.y - 1), p);
		addNode(result, getNode(node->pos.x, node->pos.y + 1), p);
		return result;
	}
	void getRegionGroupInt(std::set<pTileNode>& visited,
		pTileNode node,
		pred p,
		int id,
		pRegion result);
	void getRegionGroup(std::set<pTileNode>& visited,
		pTileNode node,
		int id,
		pRegion result);
	void getRegionGroups();
	std::vector<std::set<pTileNode>> getGroups(const std::set<pTileNode>& initialContainer, int groupsDensity) {
		std::vector<std::set<pTileNode>> result;
		std::set<pTileNode> visited;
		auto notVisited = [&visited](pTileNode n){
			return visited.find(n) == visited.end();
		};
		for (auto it = initialContainer.begin(); it != initialContainer.end(); it++) {
			auto current = *it;
			if (visited.find(current) == visited.end()) {
				std::set<pTileNode> group;
				getGroup(visited, initialContainer, current, groupsDensity, notVisited, group);
				result.push_back(group);
			}
		}
		return result;
	}
	void getUnwalkableGroupInSquare(const std::set<pTileNode>& square, pTileNode node, std::set<pTileNode>& result, pred p);
	void closeChokepoint(std::set<pTileNode>& chokePoint);
	void closeLine(pTileNode f, pTileNode s);
	void CalculateRegions();
};

class ConnectedComponent {
public:
	std::set<TilePosition> tiles;
	int id;
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

};

class Group {
protected:
	std::vector<pTileNode> tiles;
	pTileNode ** nodeMap;
	int id;
	Group(pTileNode ** map) : nodeMap(map){};
public:
	pTileNode getNode(int i, int j) {
		if (i > -1 && i < Broodwar->mapWidth() && j >= 0 && j < Broodwar->mapHeight())
			return nodeMap[i][j];
		return nullptr;
	}
	int area() {
		return tiles.size();
	}

	int getId() {
		return id;
	}

	std::vector<pTileNode> getTiles() { // ugly shit
		return tiles;
	}
};

class ChokePoint : public Group{
	friend class MapAnalyzer;
	friend class MapRegion;
	std::vector<pTileNode> closedParts;
	std::vector<pRegion> adjacentRegions;
	void calcAdjacentRegions(std::map<int, pRegion>& regions) {
		std::set<int> tempIds;
		for (auto c : closedParts) {
			const auto& neighbors = getSurroundingTilesInSquare(c->pos, 2);
			for (auto n : neighbors) {
				auto node = nodeMap[n.x][n.y];
				if (node->regionId != INVALID)
					tempIds.insert(node->regionId);
			}
		}

		for (auto id : tempIds) {
			adjacentRegions.push_back(regions[id]);
		}
	}
	ChokePoint(std::set<pTileNode>& group, pTileNode ** nodeMap) : Group(nodeMap) { 
		for (auto t : group) {
			tiles.push_back(t);
			if (t->closed)
				closedParts.push_back(t);
		}
	}
	bool inAdjacentRegion(int id);
public:
	std::vector<pRegion> getAdjacentRegions() {
		return adjacentRegions;
	}

	TilePosition::list getDefencePointsAround() {
		TilePosition::list result;
		auto closed = *closedParts.begin();
		const auto& neighbours = getSurroundingTilesInSquare(closed->pos, 11);

		std::map<int, std::vector<TilePosition>> groups;
		for (auto p : neighbours) {
			auto node = getNode(p.x, p.y);
			if (node && node->value != 0 && inAdjacentRegion(node->regionId)) {
				//Broodwar->drawBoxMap(Position(p), Position(p + TilePosition(1, 1)), Colors::Green);
				groups[node->regionId].push_back(p);
			}
		}

		for (auto g : groups) {
			auto dp = TilePosition(0, 0);
			for (auto p : g.second) {
				dp += p;
			}
			dp /= g.second.size();
			//Broodwar->drawBoxMap(Position(dp), Position(dp + TilePosition(1, 1)), Colors::Green);
			result.push_back(dp);
		}
		return result;
	}
};

class MapRegion : public Group {
	friend class MapAnalyzer;
	std::set<pTileNode> border;
	std::vector<pChokePoint> adjacentChokePoints;
	std::vector<pRegion> adjacentRegions;
	MapRegion(pTileNode ** nodeMap) : Group(nodeMap){};
	void calcAdjacentChokepoints(std::map<int, pChokePoint>& cps) {
		std::set<int> visitedIds;
		std::vector<pChokePoint> result;
		for (auto t : border) {
			auto id = t->chokePointId;
			if (id != INVALID){
				if (visitedIds.find(id) == visitedIds.end()) {
					visitedIds.insert(id);
					result.push_back(cps[id]);
				}
			}
		}
		adjacentChokePoints = result;
	}
	void calcAdjacentRegions(std::map<int, pRegion>& regions) {
		std::set<int> visitedIds;
		std::vector<pRegion> result;
		for (auto cp : adjacentChokePoints) {
			auto adjRegionsInCP = cp->adjacentRegions;
			for (auto r : adjRegionsInCP) {
				if (r->id != this->id) {
					visitedIds.insert(r->id);
				}
			}
		}

		for (auto i : visitedIds) {
			result.push_back(regions[i]);
		}
		adjacentRegions = result;
	}

	void merge(const pRegion& other) {
		for (auto t : other->tiles) {
			tiles.push_back(t);
			t->regionId = id;
		}

		for (auto t : other->border) {
			if (border.find(t) != border.end()) {
				border.erase(t);
				t->regionId = id;
				tiles.push_back(t);
			}
			else {
				border.insert(t);
			}
		}
	}
public:
	std::vector<pChokePoint> getAdjacentChokepoints() {
		return adjacentChokePoints;
	}
	std::vector<pRegion> getAdjacentRegions() {
		return adjacentRegions;
	}
};
#endif //MAP_R_H
