#include <stdio.h>
#include <stddef.h>
#include <string.h>

#include "OOC\OOC.h"
#include "OOC\New.h"
#include "Regexp.h"
#include "OOC\Set.h"
#include "OOC\Set_r.h"
#include "OOC\String.h"
#include "OOC\String_r.h"
#include "Production.h"
#include "Production_r.h"
#include "LrAutomata.h"
#include "LrAutomata_r.h"
#include "OOC\LinkList.h"
#include "OOC\LinkList_r.h"

int main()
{
	loadRegexp();

	struct Class * ll = LinkList;

	struct LinkList * list = new (LinkList, 0);
	
	struct LinkListItem * listItem1 = new (LinkListItem, new (String, "aaa", 0), 0);
	struct LinkListItem * listItem2 = new (LinkListItem, new (String, "bbb", 0), 0);
	struct LinkListItem * listItem3 = new (LinkListItem, new (String, "ccc", 0), 0);

	listInsert(list, listItem1);
	listInsert(list, listItem2);
	listInsert(list, listItem3);

	listDelete(list, listItem2);
	listDelete(list, listItem3);
	listDelete(list, listItem1);

	printf("%p\n", list->head);

	delete(list);
	
	/*struct Set * grammar = new (Set, 0);
	struct LrAutomata * automata;
	int i;

	add(grammar, new(Production, "regexp->{regexp}|{concat}", 0));
	add(grammar, new(Production, "regexp->{concat}", 0));
	add(grammar, new(Production, "concat->{concat}{repeat}", 0));
	add(grammar, new(Production, "concat->{repeat}", 0));
	add(grammar, new(Production, "repeat->{unit}", 0));
	add(grammar, new(Production, "repeat->{unit}*", 0));
	add(grammar, new(Production, "unit->({regexp})", 0));*/
	// add(grammar, new(Production, "unit->[a-zA-Z0-9]", 0));

	// add(grammar, new(Production, "", 0));

	/*for (i = 0; i < productionSet->length; i++)
	{
		printProduction(productionSet->items[i]);
		printf("======================================\n");
	}*/

	/*automata = new (LrAutomata, grammar, 0);

	delete(automata);*/
	
	/*
	 * Pause here for Debug.
	 */
	system("pause");
}