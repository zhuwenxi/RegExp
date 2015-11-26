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

	struct Production * production = new (Production, 0);

	delete(production);

	printf("%d\n", '[' == '\[');

	/*
	 * Pause here for Debug.
	 */
	system("pause");
}