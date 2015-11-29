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
	struct Production * production= new(Production, "concat->{concat}{repeat}|{repeat}", 0);
	int i;

	add(productionSet, new(Production, "regexp->{regexp}[|]{concat}|{concat}", 0));
	add(productionSet, production);
	add(productionSet, new(Production, "repeat->{unit}|{unit}*"));

	for (i = 0; i < productionSet->length; i++)
	{
		printProduction(productionSet->items[i]);
	}
	
	delete(productionSet);

	/*char * origin = "a|b|c";
	char str[100];
	strcpy_s(str, sizeof(char) * (strlen(origin) + 1), origin);
	char * context = NULL;
	char * p = strtok_s(str, "|", &context);

	while (p != NULL)
	{
		printf("%c\n", *p);
		p = strtok_s(NULL, "|", &context);
	}*/

	/*
	 * Pause here for Debug.
	 */
	system("pause");
}