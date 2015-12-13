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
#include "OOC\String.h"




/*
 * Declarations:
 */
const void * SlrAutomata;

const void * State;

static void * initStates(struct SlrAutomata * automata);
static struct Set * initGrammar();
static struct Set * initGrammarSymbol(struct Set * grammar);
static struct Set * extractGrammarSymbol(const struct Production * prod);
static void addDotPrefix(struct Set * grammar);
static struct ProductionToken * tokenNextToDot(struct Production * production);




static void * SlrAutomata_ctor(void * _self, va_list * args)
{
	struct Set * states = initStates(_self);


	/*struct Set * grammar = initGrammar();
	struct Set * grammarSymbol = initGrammarSymbol(grammar);
	struct String * str;
	struct SetIterator * iter = new (SetIterator, grammarSymbol, 0);
	struct ProductionToken * data;*/

	/*for (data = start(iter); data != end(iter); data = next(iter))
	{
		str = toString(data);

		printf("%s\n", str->text);

		delete(str);
	}*/


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

static struct Set * closure(struct Set * state, struct Set * grammar)
{
	int prevLength;

	

	struct Production * item;
	struct Production * prod;

	do
	{
		prevLength = state->length;

		struct SetIterator * stateIter = new (SetIterator, state, 0);

		for (item = start(stateIter); item != end(stateIter); item = next(stateIter))
		{
			struct ProductionToken * tokenNext = tokenNextToDot(item);

			/*struct String * str = toString(state);

			printf("%s\n================================================\n", str->text);*/

			if (tokenNext)
			{
				struct SetIterator * grammarIter = new (SetIterator, grammar, 0);

				for (prod = start(grammarIter); prod != end(grammarIter); prod = next(grammarIter))
				{
					if (equals(prod->head, tokenNext))
					{
						struct Production * newItem = clone(prod);

						addDotPrefix(newItem);

						if (!search(state, newItem))
						{
							insert(state, newItem);
						}
						
					}
				}

				delete(grammarIter);
			}
		}

		delete(stateIter);
	} 
	while (state->length != prevLength);

	return state;
}

static void * initStates(struct SlrAutomata * automata)
{
	struct Set * states = new (Set, 0);
	struct Set * state0 = new (Set, 0);
	struct Set * grammar = initGrammar();

	struct Set * grammarSymbol = initGrammarSymbol(grammar);

	struct Production * initialItem = new (Production, "regexp'->{regexp}", 0);
	
	int stateIndex;
	int productionIndex;
	int symbolIndex;
	
	int statesLength = 0;


	insert(state0, initialItem);
	insert(states, state0);


	addDotPrefix(initialItem);

	/*struct String * grammarTxt = toString(state0);

	printf("%s\n", grammarTxt->text);*/


	state0 = closure(state0, grammar);

	struct String * stateTxt = toString(state0);

	printf("%s\n", stateTxt->text);

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

				if (newState && !search(states, newState))
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

static void addDotPrefix(struct Production * production)
{
	struct Set * body = production->body;
	struct ProductionToken * token = new (ProductionToken, new (String, "DOT", 0), false, 0);

	token->isFlag = true;

	insertAt(body, token, 0);
}

static struct ProductionToken * tokenNextToDot(struct Production * production)
{
	assert(production);

	struct ProductionToken * dot = new (ProductionToken, new (String, "DOT", 0), false, 0);
	struct Set * body = production->body;
	int i;

	dot->isFlag = true;

	for (i = 0; i < body->length - 1; i ++)
	{
		struct ProductionToken * token = body->items[i];

		if (equals(dot, token))
		{
			delete(dot);

			return body->items[i + 1];
		}
	}

	delete(dot);

	return NULL;
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