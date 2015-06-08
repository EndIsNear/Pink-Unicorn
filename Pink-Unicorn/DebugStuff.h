#ifndef DEBUG_STUFF_H
#define DEBUG_STUFF_H
#pragma once
#include <assert.h>

#define DEBUG
#ifdef DEBUG

#define DEBUG_CHECK(cond) {static int ignor=false; DoDebugThrow(cond, ignor);}
#define DEBUG_EXP(exp) exp


void DoDebugThrow(bool cond, int &ignor);

#else
#define DEBUG_CHECK(cond) 
#define DEBUG_EXP(exp) 


#endif 


#endif 