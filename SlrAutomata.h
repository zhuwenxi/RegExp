#ifndef __SLRAUTOMATA_H__
#define __SLRAUTOMATA_H__



#include "Automata.h"




extern const void * SlrAutomata;

extern const void * Action;

// Unit tests:
struct Set * test_initGrammarSymbol(struct Set * grammar);
struct Set * test_follow(struct Set * grammar, struct ProductionToken * symbol);
struct Set * test_first(struct Set * grammar, struct ProductionToken * symbol);

#endif