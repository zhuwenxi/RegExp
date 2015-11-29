#include <stdarg.h>

#include "LrAutomata.h"
#include "LrAutomata_r.h"
#include "OOC\Class.h"
#include "OOC\Object.h"
#include "OOC\Set_r.h"

static LrAutomata_ctor(void * _self, va_list * args)
{
	struct LrAutomata * self = _self;
	struct Set * grammar = va_arg(*args, struct Set *);

	self->grammar = grammar;

	return self;
}

static LrAutomata_dtor(void * _self)
{
	struct LrAutomata * self = _self;
	struct Set * grammar = self->grammar;

	delete(grammar);

	return self;
}

const void * LrAutomata;

void loadLrAutomata()
{
	if (!LrAutomata)
	{
		LrAutomata = new (Class, "LrAutomata", Object, sizeof(struct LrAutomata), ctor, LrAutomata_ctor, dtor, LrAutomata_dtor);
	}
}