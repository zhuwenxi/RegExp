#ifndef _AUTOMATA_R_H__
#define _AUTOMATA_R_H__

#include "OOC\Set_r.h"

struct Automata
{
	struct Object _;
	struct Set * grammar;
	struct Set * states;
	struct Set * transitionGraphic;
};

struct State
{
	struct Object;
	struct Set * items;
};

#endif