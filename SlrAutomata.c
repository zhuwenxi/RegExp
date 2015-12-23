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
#include "OOC\HashTable.h"
#include "OOC\HashTable_r.h"




/*
 * Declarations:
 */
const void * SlrAutomata;
const void * Action;




static struct Action * action(const void * _automata, const void * _state, const void * _action);
static void * constructStates(struct SlrAutomata * automata);
static struct Set * initGrammar();
static struct Set * initGrammarSymbol(struct Set * grammar);
static void updateGoto(struct SlrAutomata * slrAutomata, struct Set * state, struct ProductionToken * symbol, struct Set * targetState);
static void constructAction(struct SlrAutomata * slrAutomata);
static struct Set * extractGrammarSymbol(const struct Production * prod);
static void addDotPrefix(struct Set * grammar);
static struct ProductionToken * tokenNextToDot(struct Production * production);
static struct Set * items(const void * _automata, const void * _state, const void * _token);
static struct Set * closure(struct Set * state, struct Set * grammar);
static struct Production * getInitialItem(struct Set * grammar);
static void * queryTwoStageHashTable(const void * _hashTable, const void * _state, const void * _symbol);




static void * SlrAutomata_ctor(void * _self, va_list * args)
{
	struct SlrAutomata * self = cast(SlrAutomata, _self);
	assert(self);

	// Init self->GOTO and self->ACTION:
	self->GOTO = new(HashTable, 0);
	self->ACTION = new(HashTable, 0);


	// Init self->grammar:
	struct Set * grammar = va_arg(*args, struct Set *);
	if (grammar)
	{
		((struct Automata *)self)->grammar = grammar;
	}
	else
	{
		// If no grammar specificed, use default grammar.
		((struct Automata *)self)->grammar = initGrammar();
	}
	
	// Init self->states:
	((struct Automata *)self)->states = constructStates(_self);

	constructAction(self);

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

	// insert(grammar, new(Production, "regexp'->{regexp}", 0));
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

static struct Action * action(const void * _automata, const void * _state, const void * _symbol)
{
	const struct SlrAutomata * automata = cast(SlrAutomata, _automata);
	const struct Set * state = cast(Set, _state);
	const struct ProductionToken * symbol = cast(ProductionToken, _symbol);

	if (automata && state && symbol)
	{
		
	}
	else
	{
		return NULL;
	}
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

static struct Set * items(const void * _automata, const void * _state, const void * _token)
{
	struct Set * state = cast(Set, _state);
	struct ProductionToken * token = cast(ProductionToken, _token);

	assert(state && token);

	struct Set * targetState = new (Set, 0);

	struct SetIterator * iter = new (SetIterator, state, 0);
	struct Production * prod;

	for (prod = start(iter); prod != end(iter); prod = next(iter))
	{
		struct ProductionToken * nextToken = tokenNextToDot(prod);

		if (nextToken && equals(nextToken, token))
		{
			struct Production * nextProduction = clone(prod);

			int indexOfNextToken = indexOf(prod->body, nextToken);
			swap(nextProduction->body, indexOfNextToken, indexOfNextToken - 1);

			insert(targetState, nextProduction);
		}
	}

	delete(prod);

	if (targetState->length)
	{
		return targetState;
	}
	else
	{
		return NULL;
	}
}

static void * constructStates(struct SlrAutomata * automata)
{
	struct Set * states = new (Set, 0);
	struct Set * state0 = new (Set, 0);
	struct Set * grammar = ((struct Automata *)automata)->grammar;

	struct Set * grammarSymbol = initGrammarSymbol(grammar);

	struct Production * initialItem = getInitialItem(grammar);

	if (!initialItem)
	{
		return;
	}
	
	int stateIndex;
	int productionIndex;
	int symbolIndex;
	
	int statesLength = 0;


	insert(state0, initialItem);
	insert(states, state0);


	addDotPrefix(initialItem);


	state0 = closure(state0, grammar);

	do
	{
		statesLength = states->length;

		for (stateIndex = 0; stateIndex < states->length; stateIndex ++)
		{
			struct Set * state = states->items[stateIndex];

			for (symbolIndex = 0; symbolIndex < grammarSymbol->length; symbolIndex ++)
			{
				struct ProductionToken * symbol = grammarSymbol->items[symbolIndex];

				struct Set * newState = items(automata, state, symbol);
				
				if (newState)
				{
					newState = closure(newState, grammar);

					if (!search(states, newState))
					{
						// Add new state into states.
						insert(states, newState);

						// Update the "goto" hash table.
						updateGoto(automata, state, symbol, newState);
					}
					
				}
				else
				{
					delete(newState);
				}
			}
		}
		

	} while (statesLength != states->length);

	
	int i;

	for (i = 0; i < states->length; i++)
	{
		void * state = states->items[i];
		struct String * str = toString(state);
		printf("state %d:\n\n%s\n==========================\n", i, str->text);
		delete(str);
	}

	return states;
}

static void updateGoto(struct SlrAutomata * slrAutomata, struct Set * _state, struct ProductionToken * _symbol, struct Set * _targetState)
{
	struct HashTable * hashByState = slrAutomata->GOTO;

	assert(hashByState);

	struct Set * state = clone(_state);
	struct Set * symbol = clone(_symbol);
	struct Set * targetState = clone(_targetState);

	struct Pair * statePair = search(hashByState, state);
	struct HashTable * hashBySymbol;

	if (!statePair)
	{
		statePair = new (Pair, state, new (HashTable, 0));
		insert(hashByState, statePair);
	}

	hashBySymbol = cast(HashTable, statePair->value);

	assert(hashBySymbol);

	struct Pair * symbolPair = search(hashBySymbol, symbol);

	if (!isOf(symbolPair, Pair))
	{
		/*printf("origin:\n\n");
		printf("%s\n\n", toString(state)->text);
		printf("symbol: ");
		printf("\"%s\"\n\n", toString(symbol)->text);
		printf("target:\n\n");
		printf("%s\n======================================================\n", toString(targetState)->text);*/
		symbolPair = new (Pair, symbol, targetState);
	}
}

static void constructAction(struct SlrAutomata * _slrAutomata)
{
	struct SlrAutomata * slrAutomata = cast(SlrAutomata, _slrAutomata);


	if (slrAutomata)
	{
		struct HashTable * GOTO = cast(HashTable, slrAutomata->GOTO);
		
		if (GOTO)
		{
			
		}
		else
		{
			return;
		}
	}
	else
	{
		return;
	}
}

static void updateAction(struct SlrAutomata * _slrAutomata, struct Set * _state, struct ProductionToken * _symbol, struct Action * action)
{
	struct HashTable * ACTION = cast(HashTable, _slrAutomata->ACTION);
	
	if (ACTION)
	{
		struct Set * state = clone(_state);
		struct Set * symbol = clone(_symbol);

		struct Pair * statePair = search(ACTION, state);
		struct HashTable * hashBySymbol;

		if (!statePair)
		{
			statePair = new (Pair, state, new (HashTable, 0));
			insert(ACTION, statePair);
		}

		hashBySymbol = cast(HashTable, statePair->value);

		assert(hashBySymbol);

		struct Pair * symbolPair = search(hashBySymbol, symbol);

		if (!isOf(symbolPair, Pair))
		{
			symbolPair = new (Pair, symbol, action);
		}
	}
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

static struct Production * getInitialItem(struct Set * grammar)
{
	if (grammar && grammar->length)
	{
		struct Production * firstItem = cast(Production, grammar->items[0]);

		if (firstItem)
		{
			struct String * head = new (String, firstItem->head->text->text, 0);
			struct String * singleQuote = new (String, "'", 0);
			struct String * arrow = new (String, "->", 0);
			struct String * leftBrace = new (String, "{", 0);
			struct String * body = new (String, firstItem->head->text->text);
			struct String * rightBrace = new (String, "}", 0);

			struct String * productionString = add(head, singleQuote, arrow, leftBrace, body, rightBrace,0);

			delete(head);
			delete(singleQuote);
			delete(arrow);
			delete(body);
			delete(leftBrace);
			delete(rightBrace);

			return new (Production, productionString->text, 0);
		}
		else
		{
			return NULL;
		}
	}
	else
	{
		return NULL;
	}
}

static void * queryTwoStageHashTable(const void * _hashTable, const void * _state, const void * _symbol)
{
	return NULL;
}





void loadSlrAutomata()
{
	if (!SlrAutomata)
	{
		SlrAutomata = new (AutomataClass, "SlrAutomata", Automata, sizeof(struct SlrAutomata), ctor, SlrAutomata_ctor, dtor, SlrAutomata_dtor, transfor, SlrAutomata_transfor,0);
	}

	if (!Action)
	{
		Action = NULL;
	}
}