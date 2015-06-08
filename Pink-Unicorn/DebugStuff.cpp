#include "DebugStuff.h"
#include <windows.h>

void DoDebugThrow(bool cond, int &ignor)
{
	
	if (!cond && !ignor) // make ignor check true if you want to continue program
	{
		DebugBreak(); // here you have chance to change "ignor"
		if (!ignor)
		assert(cond);
	}
}