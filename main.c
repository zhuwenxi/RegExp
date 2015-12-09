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

	void * set = new (Set, 0);
	struct SetIterator * delegate = new (SetIterator, set, 0);
	struct Iterator * iter;
	struct String * text;

	insert(set, new (String, "FILE"));
	insert(set, new (String, "EDIT"));
	insert(set, new (String, "VIEW"));

	struct Iterator * s = start(delegate);
	struct iterator * e = end(delegate);

	printf("%d\n", s==e);

	for (iter = start(delegate); iter != end(delegate); iter = next(iter))
	{
		text = toString(iter->data);

		printf("%s\n", text->text);

		delete(text);
		
	}
	

	/*void * slrAutomata = new (SlrAutomata, 0);

	delete(slrAutomata);*/

	
	/*
	 * Pause here for Debug.
	 */
	system("pause");
}