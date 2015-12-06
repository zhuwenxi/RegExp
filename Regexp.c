#include "OOC\OOC.h"
#include "Regexp.h"
#include "Production.h"
#include "Automata.h"
#include "SlrAutomata.h"




void loadRegexp()
{
	loadOoc();
	loadProduction();
	loadAutomata();
	loadSlrAutomata();
}