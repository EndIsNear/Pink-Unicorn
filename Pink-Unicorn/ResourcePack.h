#ifndef PINK_UNICORN_RESOURCE_PACK
#define PINK_UNICORN_RESOURCE_PACK

#include <BWAPI.h>
#include <BWAPI/Client.h>

struct ResourcePack
{
	struct ZeroTag{};

	int minerals;
	int gas;
	int supply; // this will represent Total - Used supply
	

	ResourcePack(){}
	ResourcePack(ZeroTag &) : minerals(0), gas(0), supply(0){}
	ResourcePack(int m, int g, int s) : minerals(m), gas(g), supply(s){}

	bool IsCorrectState();
	ResourcePack& operator+=(const ResourcePack &rh);
	ResourcePack& operator-=(const ResourcePack &rh);
	ResourcePack& operator/=(int frames);

	static ResourcePack NeedFor(BWAPI::UnitType &type)
	{
		return ResourcePack{ type.mineralPrice(), type.gasPrice(), type.supplyRequired() };
	}
};

#endif