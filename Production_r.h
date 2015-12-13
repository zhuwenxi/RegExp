#ifndef __PRODUCTION_R_H__
#define __PRODUCTION_R_H__

#include <stdbool.h>

#include "OOC\Object_r.h"
#include "OOC\String_r.h"

struct Production
{
	struct Object _;
	struct ProductionToken * head;
	struct Set * body;
};

struct ProductionToken
{
	struct Object _;
	struct String * text;
	bool isTerminal;
	bool isNonterminal;
	bool isFlag;
};


#endif