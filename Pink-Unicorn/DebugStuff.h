#ifndef DEBUG_STUFF_H
#define DEBUG_STUFF_H

#define DEBUG
#ifdef DEBUG

#define DEBUG_CHECK(cond) assert(cond)
#define DEBUG_EXP(exp) exp


#else
#define DEBUG_CHECK(cond) 
#define DEBUG_EXP(exp) 


#endif 


#endif 