#include <stdarg.h>
#include <stddef.h>
#include <assert.h>

#include "Automata.h"
#include "Automata_r.h"
#include "OOC\Object.h"
#include "OOC\Object_r.h"
#include "OOC\Class.h"



const void * Automata;

const void * AutomataClass;

void * transfor(const void * _automata, const void * _originState, const void * _token)
{
	struct Object * automata = cast(Object, _automata);
	struct AutomataClass * class = cast(AutomataClass, automata->class);

	assert(automata && class);

	return class->transfor(_automata, _originState, _token);
}

void * parse(const void * _automata, const void * input)
{
	struct Object * automata = cast(Object, _automata);
	struct AutomataClass * class = cast(AutomataClass, automata->class);

	assert(automata && class);

	return class->parse(_automata, input);
}



static void * Automata_transfor(const void * automata, const void * state)
{
	return NULL;
}

static void * Automata_parse(const void * _automata, const void * input)
{
	return NULL;
}




static void * AutomataClass_ctor(const void * _self, va_list * _args)
{
	struct AutomataClass * self = super_ctor(AutomataClass, _self, _args);
	typedef void(*voidf)();
	voidf selector;
	voidf method;
	va_list args = *_args;

	assert(self);

	while ((selector = va_arg(args, voidf)) != 0)
	{
		method = va_arg(args, voidf);

		if (selector == transfor)
		{
			self->transfor = method;
		} 
		else if (selector == parse)
		{
			self->parse = method;
		}
	}

	return self;
}

void loadAutomata()
{
	if (!AutomataClass)
	{
		AutomataClass = new (Class, "AutomataClass", Class, sizeof(struct AutomataClass), ctor, AutomataClass_ctor, 0);
	}

	if (!Automata)
	{
		Automata = new (AutomataClass, "Automata", Object, sizeof(struct Automata), transfor, Automata_transfor, 0);
	}
}