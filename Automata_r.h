#ifndef _AUTOMATA_R_H__
#define _AUTOMATA_R_H__

#include "OOC\Object_r.h"
#include "OOC\Class_r.h"
#include "OOC\Set_r.h"
#include "OOC\HashTable_r.h"


struct Automata
{
	struct Object _;
	struct Set * grammar;
	struct Set * states;
};

struct AutomataClass
{
	struct Class _;
	void * (*transfor)(const void * automata, const void * state, const void * token);
	void * (*parse)(const void * automata, const void * input);
};

#endif