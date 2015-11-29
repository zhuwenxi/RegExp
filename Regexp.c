#include "OOC\OOC.h"
#include "Regexp.h"
#include "Production.h"
#include "LrAutomata.h"

void loadRegexp()
{
	loadOoc();
	loadProduction();
	loadLrAutomata();
}