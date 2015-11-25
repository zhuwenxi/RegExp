#ifndef __PRODUCTION_R_H__
#define __PRODUCTION_R_H__

#include "OOC\Object_r.h"
#include "OOC\String_r.h"

struct Production
{
	struct Object _;
	struct String * head;
};

#endif