#include <stdio.h>
#include <stddef.h>

#include "initOoc.h"
#include "String.h"
#include "String_r.h"

int main()
{
	initOoc();

	const struct String * string = new (String, "hello", 100, NULL);
	const struct String * string2 = new (String, "world", 5, NULL);

	printf("%s, %s\n", string->text, string2->text);

	delete(string);
	 

	/*
	 * Pause here for Debug.
	 */
	system("pause");
}