#include <stdbool.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <assert.h>

#include "Production.h"
#include "Production_r.h"
#include "OOC\Object.h"
#include "OOC\Class.h"
#include "OOC\String.h"
#include "OOC\Set.h"
#include "OOC\Set_r.h"



static struct Set * createBody(char *body);

static void * ProductionToken_ctor(void * _self, va_list * args)
{
	struct ProductionToken * self = cast(ProductionToken, _self);

	struct String * text = va_arg(*args, struct String *);
	bool isTerminal = va_arg(*args, bool);

	assert(text);

	self->text = text;
	self->isTerminal = isTerminal;
	self->isNonterminal = !isTerminal;

	return self;
}

static void * ProductionToken_dtor(void * _self)
{
	struct ProductionToken * self = cast(ProductionToken, _self);

	assert(self && self->text);

	delete(self->text);

	return self;
}

static void * Production_ctor(void * _self, va_list * args)
{
	struct Production * self = cast(Production, _self);
	char * productionText = va_arg(*args, char *);
	struct ProductionToken * head;
	struct Set * body;
	char * separator = "->";
	char * arrow = strstr(productionText, separator);
	int headLength;
	int bodyLength;

	assert(self);

	//if (arrow != NULL)
	//{
	//	headLength = arrow - productionText;
	//	self->head = new (ProductionToken, new (String, productionText, headLength, 0), false, 0);

	//	// production = head + separtor + body
	//	bodyLength = strlen(productionText) - headLength - strlen(separator);
	//	self->body = createBody(arrow + strlen(separator));

	//	return self;
	//}
	//else
	//{
	//	return self;
	//}
	return self;
}

static void * Production_dtor(void * _self)
{
	struct Production * self = cast(Production, _self);

	delete(self->head);
	delete(self->body);

	return self;
}

static struct Set * createBody(char *body)
{
	struct Set * set = new (Set, 0);
	bool isTerminal = true;
	char * current;

	while (current != '\0')
	{
		if (current == '{')
		{
			isTerminal = false;
		}
		else if (current == '[')
		{

		}
		else
		{
			if (isTerminal == true)
			{
				add(set, new (ProductionToken, new(String, current, 1), isTerminal, 0));
			}
			else
			{

			}
			
		}
		current++;
	}

	return set;
}




const void * Production;

const void * ProductionToken;

void loadProduction()
{
	if (!ProductionToken)
	{
		ProductionToken = new (Class, "ProductionToken", Class, sizeof(struct ProductionToken), ctor, ProductionToken_ctor, dtor, ProductionToken_dtor, 0);
	}

	if (!Production)
	{
		Production = new (Class, "Production", Object, sizeof(struct Production), ctor, Production_ctor, dtor, Production_dtor);
	}
}