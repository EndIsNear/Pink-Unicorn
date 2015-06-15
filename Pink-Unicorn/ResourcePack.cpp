#include "ResourcePack.h"
#include "DebugStuff.h"
#include "assert.h"

bool ResourcePack::IsCorrectState()
{
	DEBUG_CHECK(minerals >= 0 && gas >= 0);
	if (minerals >= 0 && gas >= 0 && supply >= 0)
		return true;
	return false;
}

ResourcePack& ResourcePack::operator+=(const ResourcePack &rh)
{
	DEBUG_EXP(IsCorrectState());

	minerals += rh.minerals;
	gas += rh.gas;
	supply += rh.supply;

	DEBUG_EXP(IsCorrectState());
	return *this;
}

ResourcePack& ResourcePack::operator-=(const ResourcePack &rh)
{
	DEBUG_EXP(IsCorrectState());

	minerals -= rh.minerals;
	gas -= rh.gas;
	supply -= rh.supply;

	DEBUG_EXP(IsCorrectState());
	return *this;
}

ResourcePack& ResourcePack::operator/=(int frames)
{
	DEBUG_EXP(IsCorrectState());

	minerals /= frames;
	gas /= frames;
	supply /= frames;

	DEBUG_EXP(IsCorrectState());
	return *this;
}
