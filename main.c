#include <stdio.h>
#include <stddef.h>
#include <string.h>

#include "OOC\OOC.h"
#include "Regexp.h"
#include "OOC\Set.h"
#include "OOC\Set_r.h"
#include "OOC\String.h"
#include "OOC\String_r.h"
#include "Production.h"
#include "Production_r.h"

int main()
{
	loadRegexp();

	struct Set * productionSet = new (Set, 0);
	int i;

	add(productionSet, new(Production, "regexp->{regexp}|{concat}", 0));
	add(productionSet, new(Production, "regexp->{concat}", 0));
	add(productionSet, new(Production, "concat->{concat}{repeat}", 0));
	add(productionSet, new(Production, "concat->{repeat}", 0));
	add(productionSet, new(Production, "repeat->{unit}"));
	add(productionSet, new(Production, "repeat->{unit}*"));

	for (i = 0; i < productionSet->length; i++)
	{
		printProduction(productionSet->items[i]);
		printf("======================================\n");
	}
	
	delete(productionSet);

	/*
	 * Pause here for Debug.
	 */
	system("pause");
}