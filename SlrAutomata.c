#include <stddef.h>
#include <stdarg.h>
#include <assert.h>

#include "SlrAutomata.h"
#include "SlrAutomata_r.h"
#include "Automata.h"
#include "Automata_r.h"
#include "OOC\Set.h"
#include "OOC\Set_r.h"




static void initGrammar();

const void * SlrAutomata;

static struct Set * grammar;

static void * SlrAutomata_ctor(void * _self, va_list * args)
{
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

static void initGrammar()
{
	
}

void loadSlrAutomata()
{
	if (!SlrAutomata)
	{
		SlrAutomata = new (AutomataClass, "SlrAutomata", Automata, sizeof(struct SlrAutomata), ctor, SlrAutomata_ctor, dtor, SlrAutomata_dtor, transfor, SlrAutomata_transfor,0);
	}
}