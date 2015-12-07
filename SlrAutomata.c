#include <stddef.h>
#include <stdarg.h>
#include <assert.h>

#include "SlrAutomata.h"
#include "SlrAutomata_r.h"
#include "Automata.h"
#include "Automata_r.h"
#include "OOC\Set.h"
#include "OOC\Set_r.h"
#include "Production.h"
#include "Production_r.h"
#include "OOC\New.h"



/*
 * Declarations:
 */
const void * SlrAutomata;

static struct Set * initGrammar();



static void * SlrAutomata_ctor(void * _self, va_list * args)
{
	struct Set * grammar = initGrammar();
	struct String * str;
	int i;
	
	for (i = 0; i < grammar->length; i++)
	{
		str = toString(grammar->items[i]);
		
		printf("%s\n", str->text);

		delete(str);
	}
	

	return _self;
}

static void * SlrAutomata_dtor(void * _self)
{
	return _self;
}

static void * SlrAutomata_transfor(const void * _automata, const void * _state, const void * _token)
{
	return NULL;
}

static struct Set * initGrammar()
{
	struct Set * grammar = new (Set, 0);
	struct LrAutomata * automata;

	insert(grammar, new(Production, "regexp->{regexp}|{concat}", 0));
	insert(grammar, new(Production, "regexp->{concat}", 0));
	insert(grammar, new(Production, "concat->{concat}{repeat}", 0));
	insert(grammar, new(Production, "concat->{repeat}", 0));
	insert(grammar, new(Production, "repeat->{unit}", 0));
	insert(grammar, new(Production, "repeat->{unit}*", 0));
	insert(grammar, new(Production, "unit->({regexp})", 0));
	insert(grammar, new(Production, "unit->[a-zA-Z0-9]", 0));

	return grammar;
}

static void * initStates()
{

}

void loadSlrAutomata()
{
	if (!SlrAutomata)
	{
		SlrAutomata = new (AutomataClass, "SlrAutomata", Automata, sizeof(struct SlrAutomata), ctor, SlrAutomata_ctor, dtor, SlrAutomata_dtor, transfor, SlrAutomata_transfor,0);
	}
}