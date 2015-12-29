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
#include "OOC\Integer.h"
#include "OOC\Integer_r.h"




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
static void updateAction(struct SlrAutomata * _slrAutomata, struct Set * _state, struct ProductionToken * _symbol, struct Action * action);
static struct Set * extractGrammarSymbol(const struct Production * prod);
static void addDotPrefix(struct Set * grammar);
static struct ProductionToken * tokenNext(struct Production * _production, struct ProductionToken * _token);
static struct ProductionToken * tokenNextToDot(struct Production * production);
static struct Set * items(const void * _automata, const void * _state, const void * _token);
static struct Set * closure(struct Set * state, struct Set * grammar);
static struct Production * getInitialItem(struct Set * grammar);
static void * queryTwoStageHashTable(const void * _hashTable, const void * _state, const void * _symbol);
static struct Production * searchFinishedProduction(const struct Set * _state);
static struct Set * follow(const struct Set * grammar, const struct ProductionToken * symbol);
static struct Set * first(struct Set * grammar, struct ProductionToken * symbol);
static void markStatesWithNumber(struct Set * states);
static struct HashTable * stateNumberMap;




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
	markStatesWithNumber(((struct Automata *)self)->states);
	// printf("GOTO:\n\n%s\n", toString(self->GOTO)->text);
	constructAction(self);

	return _self;
}

static void * SlrAutomata_dtor(void * _self)
{
	return _self;
}

static void * SlrAutomata_transfor(const void * _automata, const void * _state, const void * _token)
{
	struct SlrAutomata * automata = cast(SlrAutomata, _automata);

	// Log:
	/*struct Set * state = cast(Set, _state);
	struct ProductionToken * token = cast(ProductionToken, _token);
	printf("state:\n\n");
	printf("%s\n", toString(state)->text);
	printf("\nsymbol:\n\n");
	printf("%s\n\n", toString(token)->text);
	printf("=======================================");*/
	
	return automata ? queryTwoStageHashTable(automata->GOTO, _state, _token) : NULL;
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

static struct Set * follow(const struct Set * _grammar, const struct ProductionToken * _symbol)
{
	struct Set * grammar = cast(Set, _grammar);
	struct ProductionToken * symbol = cast(ProductionToken, _symbol);

	if (grammar && symbol)
	{
		struct Set * followSet = new (Set, 0);

		// 1. Place $ in FOLLOW(S), where S is a start symbol;
		if (symbol->isNonterminal)
		{
			// Cosntruct the "$" symbol.
			struct ProductionToken * dollarSymbol = new (ProductionToken, new (String, "$", 0), 0);
			dollarSymbol->isFlag = true;

			insert(followSet, dollarSymbol);
		}

		int i;
		for (i = 0; i < grammar->length; i++)
		{
			struct Production * production = cast(Production, grammar->items[i]);

			assert(production);

			if (search(production->body, symbol))
			{
				struct ProductionToken * nextToken = tokenNext(production, symbol);

				// The "epsilon" token:
				struct ProductionToken * epsilon = new (ProductionToken, new (String, "epsilon", 0), false, 0);
				epsilon->isFlag = true;

				// FIRST(beta):
				struct Set * firstBeta = first(grammar, nextToken);

				if (nextToken)
				{
					int indexFirstBeta;
					for (indexFirstBeta = 0; indexFirstBeta < firstBeta->length; indexFirstBeta++)
					{
						struct ProductionToken * followBetaItem = cast(ProductionToken, firstBeta->items[indexFirstBeta]);

						assert(followBetaItem);

						if (!equals(epsilon, followBetaItem))
						{
							// If there is a production A->alpha B beta, then everything in FIRST(beta) except epsilon is in FOLLOW(B);
							if (!search(followSet, followBetaItem))
							{
								insert(followSet, clone(followBetaItem));
							}
						}
						else
						{
							// If there is a production A -> alpha B beta, where FIRST(beta) contains epsilon, then add everything in FOLLOW(A) is in FOLLOW(B);
							struct Set * followA = follow(grammar, production->head);

							int indexFollowA;
							for (indexFollowA = 0; indexFollowA < followA->length; indexFollowA ++)
							{
								if (!search(followSet, followA->items[indexFollowA]))
								{
									insert(followSet, clone(followA->items[indexFollowA]));
								}
							}

							delete(followA);
						}
					}

					
				}
				else
				{
					// If there is a production A -> alpha B, where FIRST(beta) contains epsilon, then add everything in FOLLOW(A) is in FOLLOW(B).
					struct Set * followA = follow(grammar, production->head);

					int indexFollowA;
					for (indexFollowA = 0; indexFollowA < followA->length; indexFollowA++)
					{
						if (!search(followSet, followA->items[indexFollowA]))
						{
							insert(followSet, clone(followA->items[indexFollowA]));
						}
					}

					delete(followA);
				}

				delete(firstBeta);
				delete(epsilon);
			}
		}
		
		/*if (!strcmp(symbol->text->text, "E"))
		{
			printf("+++++++++++++++++++++++++++++++\n");
			printf("FOLLOW(%s): ", toString(symbol)->text);
			printf("%s\n", toString(followSet)->text);
			printf("-------------------------------\n");
		}*/
		

		return followSet;
	}
	else
	{
		return NULL;
	}	
}

static struct Set * first(struct Set * _grammar, struct ProductionToken * _symbol)
{
	struct Set * grammar = cast(Set, _grammar);
	struct ProductionToken * symbol = cast(ProductionToken, _symbol);

	if (grammar && symbol)
	{
		//printf("symbol: %s\n", symbol->text->text);

		struct Set * firstSet = new (Set, 0);

		if (symbol->isTerminal)
		{
			// If X is terminal, then FIRST(X) = {X}:
			insert(firstSet, clone(symbol));
		}
		else
		{
			int productionIndex;

			for (productionIndex = 0; productionIndex < grammar->length; productionIndex++)
			{
				struct Production * production = cast(Production, grammar->items[productionIndex]);
				assert(production);
				//printf("production: %s\n", toString(production)->text);

				if (equals(production->head, symbol))
				{
					int tokenIndex;
					for (tokenIndex = 0; tokenIndex < production->body->length; tokenIndex ++)
					{
						struct ProductionToken * token = cast(ProductionToken, production->body->items[tokenIndex]);

						assert(token);


						// if X -> epsilon is a production, then add epsilon to FIRST(X):

						struct ProductionToken * epsilon = new (ProductionToken, new (String, "epsilon", 0), false, 0);
						epsilon->isFlag = true;

						if (equals(token, epsilon) && production->body->length == 1)
						{
							if (!search(firstSet, epsilon))
							{
								insert(firstSet, clone(epsilon));
							}
						} 

						/* Hack here: don't call first resursively when "token" equals to "symbol", to avoid infinite recursion.
						 * For example: E -> E + T.
						 */
						if (!equals(token, symbol))
						{
							struct Set * firstSetOfCurrentToken = first(grammar, token);

							int i;
							for (i = 0; i < firstSetOfCurrentToken->length; i++)
							{
								if (!search(firstSet, firstSetOfCurrentToken->items[i]) && !equals(firstSetOfCurrentToken->items[i], epsilon))
								{
									insert(firstSet, clone(firstSetOfCurrentToken->items[i]));
								}
							}

							if (!search(firstSetOfCurrentToken, epsilon))
							{
								delete(firstSetOfCurrentToken);
								break;
							}
							else
							{
								delete(firstSetOfCurrentToken);
							}
						}
						else
						{
							break;
						}


						delete(epsilon);
						
					}
				}
			}
		}

		
		//printf("return set: %s\n", toString(firstSet)->text);
		return firstSet;
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
					}

					// Log:
					/*printf("state:\n\n%s\n\n", toString(state)->text);
					printf("symbol:\n\n%s\n\n", toString(symbol)->text);
					printf("next state:\n\n%s\n\n", toString(newState)->text);
					printf("=====================================\n\n");*/

					// Update the "goto" hash table.
					updateGoto(automata, state, symbol, newState);
					
				}
				else
				{
					delete(newState);
				}
			}
		}
		

	} while (statesLength != states->length);

	
	/*int i;

	for (i = 0; i < states->length; i++)
	{
		void * state = states->items[i];
		struct String * str = toString(state);
		printf("state %d:\n\n%s\n==========================\n", i, str->text);
		delete(str);
	}*/

	return states;
}

static void updateGoto(struct SlrAutomata * slrAutomata, struct Set * _state, struct ProductionToken * _symbol, struct Set * _targetState)
{
	struct HashTable * hashByState = slrAutomata->GOTO;

	assert(hashByState);

	struct Set * state = clone(_state);
	struct ProductionToken * symbol = clone(_symbol);
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
		insert(hashBySymbol, symbolPair);
	}
}

static void constructAction(struct SlrAutomata * _slrAutomata)
{
	struct SlrAutomata * slrAutomata = cast(SlrAutomata, _slrAutomata);

	if (slrAutomata)
	{
		struct HashTable * GOTO = cast(HashTable, slrAutomata->GOTO);
		struct Set * states = cast(Set, ((struct Automata *)slrAutomata)->states);
		struct Set * symbols = initGrammarSymbol(((struct Automata *)slrAutomata)->grammar);

		// Add "$" as the end of the input stream.
		struct ProductionToken * dollarSymbol = new (ProductionToken, new (String, "$", 0), false, 0);
		dollarSymbol->isFlag = true;

		insert(symbols, dollarSymbol);

		if (GOTO && states && symbols)
		{
			int stateIndex;
			int symbolIndex;

			for (stateIndex = 0; stateIndex < states->length; stateIndex ++)
			{
				struct Set * state = states->items[stateIndex];

				for (symbolIndex = 0; symbolIndex < symbols->length; symbolIndex ++)
				{
					struct ProductionToken * symbol = symbols->items[symbolIndex];

					/*printf("state:\n%s\n\n", toString(state)->text);
					printf("symbol:\n%s\n\n", toString(symbol)->text);*/

					struct Set * nextState = transfor(slrAutomata, state, symbol);

					// Shift:
					if (nextState)
					{
						// Set ACTION[state, symbol] to "shift" GOTO(state, symbol).
						struct Action * action = new (Action, 0);
						action->isShift = true;
						action->stateToShift = clone(nextState);

						// log:
						/*struct Pair * pair = cast(Pair, search(stateNumberMap, state));
						assert(pair);
						struct Integer * intObj = cast(Integer, pair->value);
						assert(intObj);
						int sn = intObj->value;
						printf("state:\n%d\n\n", sn);
						printf("symbol:\n%s\n\n", toString(symbol)->text);
						 printf("next state:\n%s\n\n", toString(nextState)->text);
						struct Pair * nextPair = cast(Pair, search(stateNumberMap, nextState));
						assert(nextPair);
						struct Integer * intObjNext = cast(Integer, nextPair->value);
						assert(intObjNext);
						int snNext = intObjNext->value;
						printf("next state:\n%d\n\n", snNext);
						printf("action: shift\n~~~~~~~~~~~~~~~~~~~~~~\n");*/

						updateAction(slrAutomata, clone(state), clone(symbol), action);
					}

					
					struct Production * finishedProduction = searchFinishedProduction(state);

					struct Production * initialProduction = getInitialItem(((struct Automata *)slrAutomata)->grammar);
					assert(initialProduction && initialProduction->body);

					struct ProductionToken * dot = new (ProductionToken, new (String, "DOT", 0), false, 0);
					dot->isFlag = true;
					insert(initialProduction->body, dot);

					// Reduce:
					if (finishedProduction)
					{
						//printf("~~~~~~~~~~~~~\n%s\n~~~~~~~~~~~~~~~\n", toString(initialProduction)->text);
						if (!equals(initialProduction, finishedProduction) && search(follow(((struct Automata *)slrAutomata)->grammar, finishedProduction->head), symbol))
						{
							// "Reduce A->alpha".
							struct Action * action = new (Action, 0);
							action->isReduce = true;
							action->productionToReduce = clone(finishedProduction);

							/*printf("state:\n%s\n\n", toString(state)->text);
							printf("symbol:\n%s\n\n", toString(symbol)->text);
							printf("action: reduce\n~~~~~~~~~~~~~~~~~~~~~~\n");*/

							// log:
							/*struct Pair * pair = cast(Pair, search(stateNumberMap, state));
							assert(pair);
							struct Integer * intObj = cast(Integer, pair->value);
							assert(intObj);
							int sn = intObj->value;
							printf("state:\n%d\n", sn);
							printf("symbol:\n%s\n", toString(symbol)->text);
							printf("reduce:\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");*/

							updateAction(slrAutomata, clone(state), clone(symbol), action);
						}
					}
					
					// Accept:
					if (search(state, initialProduction))
					{
						// "Set ACTION[state, $] to accpet
						/*struct ProductionToken * dollarSymbol = new (ProductionToken, new (String, "$", 0), 0);
						dollarSymbol->isFlag = true;*/

						struct Action * action = new (Action, 0);
						action->isAccept = true;

						/*printf("state:\n%s\n\n", toString(state)->text);
						printf("symbol:\n%s\n\n", toString(symbol)->text);
						printf("action: accept\n~~~~~~~~~~~~~~~~~~~~~~\n");*/

						updateAction(slrAutomata, clone(state), clone(dollarSymbol), action);
					}
					
				}
			}
		}
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
		else
		{
			// Having conflict action here!
			assert(0);
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

static struct ProductionToken * tokenNext(struct Production * _production, struct ProductionToken * _token)
{
	struct Production * production = cast(Production, _production);
	struct ProductionToken * token = cast(ProductionToken, _token);

	if (production && token)
	{
		int i;
		for (i = 0; i < production->body->length - 1; i++)
		{
			struct ProductionToken * t = production->body->items[i];

			if (equals(t, token))
			{
				return production->body->items[i + 1];
			}
		}

		return NULL;
	}
	else
	{
		return NULL;
	}
}

static struct ProductionToken * tokenNextToDot(struct Production * production)
{
	/*assert(production);

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

	return NULL;*/

	struct ProductionToken * dot = new (ProductionToken, new (String, "DOT", 0), false, 0);
	dot->isFlag = true;

	struct ProductionToken * tokenNextDot = tokenNext(production, dot);

	delete(dot);

	return tokenNextDot;
	
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

static void * queryTwoStageHashTable(const void * _hashTable, const void * key1, const void * key2)
{
	struct HashTable * hashTable = cast(HashTable, _hashTable);

	if (hashTable)
	{
		struct Pair * pair = cast(Pair, search(hashTable, key1));

		if (pair)
		{
			struct HashTable * secondStageHashTable = pair->value;

			/*printf("key2:\n\n%s\n", toString(key2)->text);
			printf("\n2nd HashTable: \n\n%s\n", toString(secondStageHashTable)->text);*/
			
			struct Pair * secondPair = cast(Pair, search(secondStageHashTable, key2));

			return secondPair ? secondPair->value : NULL;
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

static struct Production * searchFinishedProduction(const struct Set * _state)
{
	const struct Set * state = cast(Set, _state);

	if (state)
	{
		int i;
		for (i = 0; i < state->length; i++)
		{
			struct Production * production = state->items[i];
			struct ProductionToken * lastTokenInBody = production->body->items[production->body->length - 1];

			struct ProductionToken * dot = new (ProductionToken, new (String, "DOT", 0), false, 0);
			dot->isFlag = true;

			if (equals(lastTokenInBody, dot))
			{
				return production;
			}
		}

		return NULL;
	}
	else
	{
		return NULL;
	}
}



static void markStatesWithNumber(struct Set * states)
{
	if (!stateNumberMap)
	{
		stateNumberMap = new (HashTable, 0);
	}

	int i;
	for (i = 0; i < states->length; i++)
	{
		struct Set * state = cast(Set, states->items[i]);
		assert(state);

		struct Pair * pair = new (Pair, clone(state), new (Integer, i, 0));

		insert(stateNumberMap, pair);
	}
}



//static void * Action_ctor(void * self, va_list * args)
//{
//	return self;
//}
//
//static void * Action_dtor(void * _self)
//{
//	struct Action * self = cast(Action, _self);
//
//	assert(self);
//
//	delete(self->stateToShift);
//	delete(self->productionToReduce);
//
//	return self;
//}





void loadSlrAutomata()
{
	if (!SlrAutomata)
	{
		SlrAutomata = new (AutomataClass, "SlrAutomata", Automata, sizeof(struct SlrAutomata), ctor, SlrAutomata_ctor, dtor, SlrAutomata_dtor, transfor, SlrAutomata_transfor,0);
	}

	if (!Action)
	{
		Action = new (Class, "Action", Object, sizeof(struct Action), 0);
	}
}




/*
 * Test suites:
 */

struct Set * test_initGrammarSymbol(struct Set * grammar)
{
	return initGrammarSymbol(grammar);
}

struct Set * test_follow(struct Set * grammar, struct ProductionToken * symbol)
{
	return follow(grammar, symbol);
}

struct Set * test_first(struct Set * grammar, struct ProductionToken * symbol)
{
	return first(grammar, symbol);
}