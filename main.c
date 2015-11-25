#include <stdio.h>
#include <stddef.h>

#include "OOC\OOC.h"
#include "Regexp.h"
#include "OOC\Set.h"
#include "OOC\Set_r.h"

int main()
{
	loadOoc();
	loadRegexp();
	

	struct Set * set = new (Set);
	delete(Set);

	 

	/*
	 * Pause here for Debug.
	 */
	system("pause");
}