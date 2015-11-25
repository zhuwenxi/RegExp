#include <stdio.h>
#include <stddef.h>

#include "OOC\OOC.h"
#include "OOC\String.h"
#include "OOC\String_r.h"

int main()
{
	loadOoc();

	const struct String * string = new (String, "hello", 100, NULL);
	const struct String * string2 = new (String, "world", 5, NULL);

	printf("%s, %s\n", string->text, string2->text);

	delete(string);
	 

	/*
	 * Pause here for Debug.
	 */
	system("pause");
}