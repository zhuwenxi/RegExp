#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>

#include "String.h"
#include "String_r.h"
#include "OOC\Object.h"
#include "OOC\Object_r.h"
#include "OOC\Class.h"

void * String_ctor(void * _self, va_list * args)
{
	struct String * self = super_ctor(Object, _self, args);

	char * text = va_arg(*args, char *);
	int length = va_arg(*args, int);

	int i;

	assert(text != NULL);

	length = length == 0 ? strlen(text) : length;

	self->text = calloc(length + 1, sizeof(char));

	for (i = 0; i < length; i++)
	{
		self->text[i] = text[i];
	}

	self->text[i] = '\0';

	return self;
}

void * String_dtor(void * _self)
{
	struct String * self = super_dtor(Object, _self);
	
	free(self->text);

	return self;
}


const void * String;

void initString()
{
	if (!String)
	{
		String = new (Class, "String", Object, sizeof(struct String), ctor, String_ctor, dtor, String_dtor);
	}
}