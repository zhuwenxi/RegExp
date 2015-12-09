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

int main()
{
	loadRegexp();

	void * str = new (String, "test", 0);
	void * iter = new (Iterator, str, 0);

	printf("%p, %p, %p\n", start(iter), end(iter), next(iter));

	delete(iter);

	/*void * slrAutomata = new (SlrAutomata, 0);

	delete(slrAutomata);*/

	
	/*
	 * Pause here for Debug.
	 */
	system("pause");
}