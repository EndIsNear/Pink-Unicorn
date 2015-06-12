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

#endif //UTILS_H