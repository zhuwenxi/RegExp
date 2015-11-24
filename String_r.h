#ifndef __STRING_R_H__
#define __STRING_R_H__

#include "OOC\Object_r.h"

struct String
{
	struct Object _;
	char * text;
};

#endif