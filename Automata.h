#ifndef __AUTOMATA_H__
#define __AUTOMATA_H__

#include "Automata_r.h"
#include "Production_r.h"

extern const void * Automata;
extern const void * State;

struct State * transfor(const struct Automata * automata, const struct State * state);

#endif