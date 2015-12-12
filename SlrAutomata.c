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

static void * initStates(struct SlrAutomata * automata);
static struct Set * initGrammar();
static struct Set * initGrammarSymbol(struct Set * grammar);
static struct Set * extractGrammarSymbol(const struct Production * prod);




static void * SlrAutomata_ctor(void * _self, va_list * args)
{
	struct Set * grammar = initGrammar();
	struct Set * grammarSymbol = initGrammarSymbol(grammar);
	struct String * str;
	struct SetIterator * iter = new (SetIterator, grammarSymbol, 0);
	struct ProductionToken * data;

	for (data = start(iter); data != end(iter); data = next(iter))
	{
		str = toString(data);

		printf("%s\n", str->text);

		delete(str);
	}


	/*int i;
	
	for (i = 0; i < grammar->length; i++)
	{
		str = toString(grammar->items[i]);
		
		printf("%s\n", str->text);

		delete(str);
	}*/
	

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
	struct SetIterator * iterator = new (SetIterator, grammar, 0);
	struct Set * grammarSymbol = new (Set, 0);
	struct Set * symbolsInProduction;
	struct Production * prod;
	
	for (prod = start(iterator); prod != end(iterator); prod = next(iterator))
	{
		symbolsInProduction = extractGrammarSymbol(prod);

		merge(grammarSymbol, symbolsInProduction, 0);

		delete(symbolsInProduction);
	}

	delete(iterator);

	return grammarSymbol;
}

static struct Set * closure(struct Set * grammar)
{
	return NULL;
}

static void * initStates(struct SlrAutomata * automata)
{
	struct Set * states;
	struct Set * state0 = new (Set, 0);
	struct Set * grammarSymbol;

	struct Production * initialItem = new (Production, "regexp'->{regexp}", 0);
	
	int stateIndex;
	int productionIndex;
	int symbolIndex;
	

	int statesLength = 0;

	insert(state0, initialItem);
	insert(states, state0);

	state0 = closure(state0);

	do
	{
		statesLength = states->length;

		for (stateIndex = 0; stateIndex < states->length; stateIndex ++)
		{
			struct Set * state = states->items[stateIndex];

			for (symbolIndex = 0; symbolIndex < grammarSymbol->length; symbolIndex ++)
			{
				struct ProductionToken * symbol = grammarSymbol->items[symbolIndex];
				struct Set * newState = transfor(automata, state, symbol);

				if (!search(states, newState))
				{
					insert(states, newState);
				}
			}
		}
		

	} while (statesLength != states->length);

	return states;
}

static struct Set * extractGrammarSymbol(const struct Production * prod)
{
	struct Set * body = prod->body;
	struct SetIterator * iterator = new (SetIterator, body, 0);
	struct Set * grammarSymbol;
	struct ProductionToken * token;

	if (body)
	{
		grammarSymbol = new (Set, 0);

		for (token = start(iterator); token != end(iterator); token = next(iterator))
		{
			insert(grammarSymbol, clone(token));
		}

		delete(iterator);

		return grammarSymbol;
	}
	else
	{
		delete(iterator);

		return NULL;
	}
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