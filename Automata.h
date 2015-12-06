#ifndef __AUTOMATA_H__
#define __AUTOMATA_H__

#include "Automata_r.h"
#include "Production_r.h"

extern const void * Automata;
extern const void * State;

extern const void * AutomataClass;

void * transfor(const void * automata, const void * state, const void * token);




void loadAutomata();

#endif