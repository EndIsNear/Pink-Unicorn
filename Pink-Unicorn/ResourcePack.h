#ifndef PINK_UNICORN_RESOURCE_PACK
#define PINK_UNICORN_RESOURCE_PACK

#include <BWAPI.h>
#include <BWAPI/Client.h>

struct ResourcePack
{
	int minerals;
	int gas;
	int supply; // this will represent Total - Used supply

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