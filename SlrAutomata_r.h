#ifndef __SLRAUTOMATA_R_H__
#define __SLRAUTOMATA_R_H__




#include "Automata_r.h"
#include "OOC\Object_r.h"
#include "OOC\Set_r.h"
#include "OOC\HashTable_r.h"

struct SlrAutomata
{
	struct Automata _;
	struct HashTable * GOTO;
	struct HashTable * ACTION;
};

#endif