#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <assert.h>

#include "OOC\OOC.h"
#include "OOC\New.h"
#include "Regexp.h"
#include "OOC\Set.h"
#include "OOC\Set_r.h"
#include "OOC\String.h"
#include "OOC\String_r.h"
#include "Production.h"
#include "Production_r.h"
#include "OOC\LinkList.h"
#include "OOC\LinkList_r.h"
#include "Automata.h"
#include "SlrAutomata.h"
#include "OOC\Iterator.h"
#include "OOC\Iterator_r.h"

int main()
{
	loadRegexp();

	struct Set * grammar = new (Set, 0);

	/*insert(grammar, new (Production, "E->{E}+{T}"));
	insert(grammar, new (Production, "E->{T}"));
	insert(grammar, new (Production, "T->{T}*{F}"));
	insert(grammar, new (Production, "T->{F}"));
	insert(grammar, new (Production, "F->({E})"));
	insert(grammar, new (Production, "F->id"));*/

	insert(grammar, new(Production, "regexp->{regexp}|{concat}", 0));
	insert(grammar, new(Production, "regexp->{concat}", 0));
	insert(grammar, new(Production, "concat->{concat}{repeat}", 0));
	insert(grammar, new(Production, "concat->{repeat}", 0));
	insert(grammar, new(Production, "repeat->{unit}", 0));
	insert(grammar, new(Production, "repeat->{unit}*", 0));
	insert(grammar, new(Production, "unit->({regexp})", 0));
	insert(grammar, new(Production, "unit->[a-zA-Z0-9]", 0));
	

	void * slrAutomata = new (SlrAutomata, grammar, 0);

	delete(slrAutomata);

	
	/*
	 * Pause here for Debug.
	 */
	system("pause");
}