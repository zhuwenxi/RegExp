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
#include "OOC\Queue.h"
#include "OOC\Queue_r.h"
#include "OOC\Stack.h"
#include "OOC\Stack_r.h"
#include "OOC\LinkList.h"
#include "OOC\LinkList_r.h"




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
static void * initInputQueue(const void * input);
static void * initStateStack(const void * states);
static bool reduceSymbols(const struct Production * production, struct Stack * stack);
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
	
	struct Set * s = ((struct Automata *)self)->states;
	int i;
	for (i = 0; i < s->length; i++)
	{
		printf("%s\n\n", toString(s->items[i])->text);
	}

	markStatesWithNumber(((struct Automata *)self)->states);
	// printf("GOTO:\n\n%s\n", toString(self->GOTO)->text);
	constructAction(self);
	// printf("ACTION:\n\n%s\n", toString(self->ACTION)->text);

	return _self;
}

static void * SlrAutomata_dtor(void * _self)
{
	struct SlrAutomata * self = cast(SlrAutomata, _self);

	assert(self);
	delete(self->GOTO);
	delete(self->ACTION);

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

static void * SlrAutomata_parse(const void * _automata, const void * input)
{
	struct SlrAutomata * automata = cast(SlrAutomata, _automata);

	if (automata && input)
	{
		struct Queue * inputQueue = initInputQueue(input);
		struct Stack * stateStack = initStateStack(((struct Automata *)automata)->states);
		struct Stack * symbolsStack = new (Stack, 0);

		// printf("%s\n", toString(inputQueue)->text);
		
		struct Set * state;

		struct ProductionToken * symbol = clone(dequeue(inputQueue));
		unshift(inputQueue, clone(symbol));
		//printf("origin input stream: %s\n", toString(inputQueue)->text);

		struct Action * nextAction;

		while (true)
		{
			// symbol = clone(dequeue(inputQueue));
			state = top(stateStack);

			printf("%s\n", toString(state)->text);
			printf("%s\n", toString(symbol)->text);

			////
			//// A little trial...
			////
			//struct Pair * hashBySymbolPair = search(automata->ACTION, state);
			//struct HashTable * hashBySymbol = hashBySymbolPair->value;
		 //   // printf("%s\n", toString(hashBySymbol->slots[0])->text);
			//struct Pair * bPair = search(hashBySymbol, symbol);

			//struct LinkList * ll = cast(LinkList, hashBySymbol->slots[0]);
			//struct LinkListItem * next = ll->head;

			//while (next)
			//{	
			//	struct Pair * p = cast(Pair, next->data);
			//	assert(p);

			//	struct ProductionToken * key = cast(ProductionToken, p->key);
			//	assert(key);
			//	

			//	if (!strcmp(key->text->text, "b"))
			//	{
			//		printf("%s\n", toString(key)->text);
			//	}

			//	next = next->next;
			//}

			nextAction = action(automata, state, symbol);
			
			assert(nextAction);

			if (nextAction->isShift)
			{
				struct Set * stateToShift = clone(nextAction->stateToShift);
				
				push(stateStack, stateToShift);
				
				push(symbolsStack, symbol);

				dequeue(inputQueue);
				symbol = dequeue(inputQueue);
				unshift(inputQueue, clone(symbol));

				printf("Shift: %s\n", toString(stateToShift)->text);
				printf("=================================\n");
			}
			else if (nextAction->isReduce)
			{
				//printf("before reduce, symbols stack: %s\n", toString(symbolsStack)->text);
				reduceSymbols(nextAction->productionToReduce, symbolsStack);

				pop(stateStack);
				state = top(stateStack);
				
				printf("******state: %s\n", toString(state)->text);
				printf("******symbol: %s\n", toString(nextAction->productionToReduce->head)->text);

				struct Set * nextState = transfor(automata, state, nextAction->productionToReduce->head);
				
				push(stateStack, clone(nextState));
				
				printf("Reduce: %s\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n", toString(symbolsStack)->text);

				// Output the production A -> beta.
			}
			else if (nextAction->isAccept)
			{
				// 
				// Parsing is done!
				//
				break;
			}
			else if (nextAction->isError)
			{
				//
				// Error!
				// TO DO: call error-recovery routine.
				//
				break;
			}
		}
		
	}
	else
	{
		return NULL;
	}
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
		struct SlrAutomata * automata = cast(SlrAutomata, _automata);

		return automata ? queryTwoStageHashTable(automata->ACTION, _state, symbol) : NULL;
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
					bool hasAction = false;

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

						hasAction = true;
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

							hasAction = true;
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

						hasAction = true;
					}

					// Error:
					if (!hasAction)
					{
						struct Action * action = new (Action, 0);
						action->isError = true;

						updateAction(slrAutomata, clone(state), clone(symbol), action);

						hasAction = true;
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
		struct ProductionToken * symbol = clone(_symbol);

		struct Pair * statePair = search(ACTION, state);
		struct HashTable * hashBySymbol;

		if (!statePair)
		{
			statePair = new (Pair, state, new (HashTable, 0));
			insert(ACTION, statePair);
		}

		hashBySymbol = cast(HashTable, statePair->value);

		assert(hashBySymbol);

		if (!symbol->isCollection)
		{
			struct Pair * symbolPair = search(hashBySymbol, symbol);

			if (!isOf(symbolPair, Pair))
			{
				symbolPair = new (Pair, symbol, action);

				insert(hashBySymbol, symbolPair);

				/*printf("%s\n", toString(symbolPair)->text);
				printf("==================\n");
				printf("%s\n", toString(_slrAutomata->ACTION)->text);
				printf("~~~~~~~~~~~~~~~~~~\n");*/
			}
			else
			{
				// Having conflict action here!
				struct Action * currentAction = cast(Action, symbolPair->value);
				assert(currentAction);

				assert(equals(currentAction, action));
			}
		}
		else
		{
			// symbol is a collection, e.g. [a-zA-Z0-9]

			// This is a dirty implementation, it could be improved later if we have time.

			const char * text = symbol->text->text;
			char * ch = text;

			while (*ch)
			{
				

				if (*ch == '-')
				{
					char start = *(ch - 1);
					char end = *(ch + 1);

					while (start <= end)
					{
						char str[2];
						str[0] = start;
						str[1] = '\0';

						struct ProductionToken * token = new (ProductionToken, new (String, str, 0), true, 0);

						struct Pair * symbolPair = search(hashBySymbol, token);
						if (!isOf(symbolPair, Pair))
						{
							symbolPair = new (Pair, token, clone(action));

							/*printf("%s\n", toString(state)->text);
							printf("%s\n", toString(token)->text);*/
						
							insert(hashBySymbol, symbolPair);
						}
						else
						{
							// Having conflict action here!
							struct Action * currentAction = cast(Action, symbolPair->value);
							assert(currentAction);

							assert(equals(currentAction, action));
						}

						start++;
					}
					
				}

				
				
				ch++;
			}
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

static void * initInputQueue(const void * input)
{
	struct Queue * inputQueue = new (Queue, 0);
	char str[2];
	char *current = input;

	while (*current)
	{
		str[0] = *current;
		str[1] = '\0';

		struct ProductionToken * queueElement = new (ProductionToken, new (String, str, 0), true, 0);
		enqueue(inputQueue, queueElement);

		current++;
	}

	struct ProductionToken * dollarSymbol = new (ProductionToken, new (String, "$", 0), false, 0);
	dollarSymbol->isFlag = true;
	
	return inputQueue;
}

static void * initStateStack(const void * _states)
{
	struct Stack * stateStack = new (Stack, 0);
	struct Set * states = cast(Set, _states);
	assert(states);

	push(stateStack, states->items[0]);

	return stateStack;
}

static bool reduceSymbols(const struct Production * production, struct Stack * stack)
{
	//printf("%s\n", toString(production)->text);
	/*printf("stack before: %s\n", toString(stack)->text);*/
	assert(production && stack);

	const struct Set * body = production->body;
	const struct ProductionToken * head = production->head;
	assert(body && head);

	// Pop production body. e.g. beta
	int i;
	for (i = body->length - 2; i >= 0; i--)
	{
		struct ProductionToken * symbol = cast(ProductionToken, body->items[i]);

		assert(symbol);

		/*if (equals(symbol, top(stack)))
		{*/
		pop(stack);
		//}
		//else
		//{
		//	// Error!
		//	assert(0);
		//	return false;
		//}
	}
	
	// Push production head. e.g. A
	push(stack, clone(head));
	/*printf("symbols stack: %s\n", toString(stack)->text);*/
	return true;
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
static bool Action_equals(void * _self, void * _another)
{
	struct Action * self = cast(Action, _self);
	struct Action * another = cast(Action, _another);
	
	assert(self && another);

	if (self->isShift != another->isShift)
	{
		return false;
	}

	if (self->isReduce != another->isReduce)
	{
		return false;
	}

	if (self->isAccept != another->isAccept)
	{
		return false;
	}

	if (self->isError != another->isError)
	{
		return false;
	}

	if (self->stateToShift)
	{
		if (!another->stateToShift)
		{
			return false;
		}
		else
		{
			if (!equals(self->stateToShift, another->stateToShift))
			{
				return false;
			}
		}
	}

	if (self->productionToReduce)
	{
		if (!another->productionToReduce)
		{
			return false;
		}
		else
		{
			if (!equals(self->productionToReduce, another->productionToReduce))
			{
				return false;
			}
		}
	}
	
	return true;
}

static struct String * Action_toString(const void * _self)
{
	struct Action * self = cast(Action, _self);

	assert(self);

	if (self->isShift)
	{
		return new (String, "SHIFT", 0);
	}

	if (self->isReduce)
	{
		return new (String, "REDUCE", 0);
	}

	if (self->isAccept)
	{
		return new (String, "ACCEPT", 0);
	}

	if (self->isError)
	{
		return new (String, "ERROR", 0);
	}

	return new (String, "Damn it!", 0);
}

static struct Action * Action_clone(const void * _self)
{
	struct Action * self = cast(Action, _self);
	assert(self);

	struct Action * copy = new (Action, 0);
	
	copy->isShift = self->isShift;
	copy->isReduce = self->isReduce;
	copy->isAccept = self->isAccept;
	copy->isError = self->isError;

	copy->productionToReduce = self->productionToReduce ? clone(self->productionToReduce) : NULL;
	copy->stateToShift = self->stateToShift ? clone(self->stateToShift) : NULL;

	return copy;
}





void loadSlrAutomata()
{
	if (!SlrAutomata)
	{
		SlrAutomata = new (AutomataClass, "SlrAutomata", Automata, sizeof(struct SlrAutomata), ctor, SlrAutomata_ctor, dtor, SlrAutomata_dtor, transfor, SlrAutomata_transfor, parse, SlrAutomata_parse, 0);
	}

	if (!Action)
	{
		Action = new (Class, "Action", Object, sizeof(struct Action), equals, Action_equals, toString, Action_toString, clone, Action_clone, 0);
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