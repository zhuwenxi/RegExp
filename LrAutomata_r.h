#ifndef __LRAUTOMATA_R_H__
#define __LRAUTOMATA_R_H__

#include "OOC\Object_r.h"

struct LrAutomata
{
	struct Object _;
	struct Set * grammar;
};

#endif