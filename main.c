#include <stdio.h>
#include <stddef.h>

#include "initOoc.h"
#include "String.h"
#include "String_r.h"

int main()
{
	initOoc();

	const struct String * string = new (String, "hello", NULL);

	printf("%s\n", string->text);

	delete(string);
	 

	/*
	 * Pause here for Debug.
	 */
	system("pause");
}