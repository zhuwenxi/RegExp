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
#include "OOC\Class.h"
#include "OOC\Object.h"



/*
 * Declarations:
 */
const void * SlrAutomata;

const void * State;

static struct Set * initGrammar();



static void * SlrAutomata_ctor(void * _self, va_list * args)
{
	struct Set * grammar = initGrammar();
	struct Set * grammarSymbol;
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

	insert(grammar, new(Production, "regexp'->{regexp}", 0));
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

static struct Set * initGrammarSymbol(struct Set * grammar)
{

}

static struct Set * closure(struct Set * grammar)
{
	return NULL;
}

static void * initStates()
{
	struct Set * setOfItems = new (Set, 0);
	struct Production * initialItem = new (Production, "regexp'->{regexp}", 0);
	struct Set * states;
	struct Set * grammarSymbol;
	int statesLength = 0;

	insert(setOfItems, initialItem);

	states = closure(setOfItems);

	do
	{
		statesLength = states->length;

		

	} while (statesLength != states->length);
}

void loadSlrAutomata()
{
	if (!SlrAutomata)
	{
		SlrAutomata = new (AutomataClass, "SlrAutomata", Automata, sizeof(struct SlrAutomata), ctor, SlrAutomata_ctor, dtor, SlrAutomata_dtor, transfor, SlrAutomata_transfor,0);
	}

	if (!State)
	{
		State = new (Class, "State", Object, sizeof(struct State), 0);
	}
}