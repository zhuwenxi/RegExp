#ifndef __STRING_R_H__
#define __STRING_R_H__

#include "OOC\Object_r.h"

struct String
{
	struct Object _;
	char * text;
};

void * String_ctor(void * _self, va_list * args);

void * String_dtor(void * _self);

#endif