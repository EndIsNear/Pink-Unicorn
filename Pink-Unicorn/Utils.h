#ifndef UTILS_H
#define UTILS_H


void drawStartPos();
void drawRegions(int minPriority = 0);
void drawStats();
void drawBullets();
void drawVisibilityData();
void showPlayers();
void showForces();


bool IsRoboticUnit(BWAPI::UnitType type);
bool IsStargateUnit(BWAPI::UnitType type);
bool IsGatewayUnit(BWAPI::UnitType type);

template <class Container>
BWAPI::TilePosition GetCenter(const Container& tiles) {
	BWAPI::TilePosition result(0, 0);
	for (auto t : tiles) {
		result += t;
	}
	return result / tiles.size();
}


#endif //UTILS_H