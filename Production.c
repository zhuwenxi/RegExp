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
#include "OOC\String_r.h"
#include "OOC\String.h"
#include "OOC\New.h"


static struct Set * createBody(char *body);
static bool isLetter(char * c);
static int initTrans(int trans[][256]);
static void setTrans(int trans[][256], int source, int target);


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

static bool ProductionToken_equals(const void * _self, const void * _another)
{
	struct ProductionToken * self = cast(ProductionToken, _self);
	struct ProductionToken * another = cast(ProductionToken, _another);

	return self && another && equals(self->text, another->text) && self->isTerminal == another->isTerminal && self->isFlag == another->isFlag;
}

static void * ProductionToken_clone(const void * self)
{
	struct ProductionToken * token = cast(ProductionToken, self);
	struct String * text;

	assert(token);

	text = new(String, token->text->text, 0);

	struct ProductionToken * copy = new (ProductionToken, text, token->isTerminal, 0);
	copy->isFlag = token->isFlag;

	return copy;
}

static struct String * ProductionToken_toString(const void * _self)
{
	struct ProductionToken * self = cast(ProductionToken, _self);

	if (self)
	{
		return new (String, self->text->text, 0);
	}
	else
	{
		return NULL;
	}
}

static void * Production_ctor(void * _self, va_list * args)
{
	struct Production * self = cast(Production, _self);
	char * productionText = va_arg(*args, char *);

	if (!productionText)
	{
		return self;
	}

	struct ProductionToken * head;
	struct Set * body = new (Set, 0);
	char * separator = "->";
	char * arrow = strstr(productionText, separator);
	int headLength;
	int bodyLength;

	assert(self);

	if (arrow != NULL)
	{
		headLength = arrow - productionText;
		self->head = new (ProductionToken, new (String, productionText, headLength, 0), false, 0);

		// production = head + separtor + body
		bodyLength = strlen(productionText) - headLength - strlen(separator);
		self->body = createBody(arrow + strlen(separator));

		return self;
	}
	else
	{
		return self;
	}

}

static void * Production_dtor(void * _self)
{
	struct Production * self = cast(Production, _self);

	delete(self->head);
	delete(self->body);

	return self;
}

static void * Production_clone(const struct Production * self)
{
	assert(self);

	struct Production * prodCopy = new (Production, 0);

	prodCopy->body = clone(self->body);
	prodCopy->head = clone(self->head);

	return prodCopy;
}

static bool Production_equals(const void * _self, const void * _another)
{
	struct Production * self = cast(Production, _self);
	struct Production * another = cast(Production, _another);

	assert(self && another);

	return equals(self->head, another->head) && equals(self->body, another->body);
}

static struct String * Production_toString(const void * _self)
{
	struct Production * self = cast(Production, _self);
	struct String * retVal;
	struct String * head;
	struct String * body;
	struct String * arrow;
	struct String * temp;

	if (self)
	{
		head = toString(self->head);
		body = toString(self->body);

		assert(head && body);

		retVal = new (String, "", 0);
		arrow = new (String, "->", 0);

		temp = retVal;
		retVal = add(retVal, head, arrow, body, 0);
		free(temp);

		free(arrow);

		return retVal;
	}
	else
	{
		return NULL;
	}
}

static struct Set * createBody(char *bodyText)
{
	struct Set * body = new (Set, 0);

	int state = 0;
	char * start = 0;
	char * end = 0;
	bool accept = false;

	char * current = bodyText;

	// Transform diagram of the automata.
	int trans[100][256];
	
	initTrans(trans);

	while (*current != '\0')
	{
		// Init the state of the automata.
		state = 0;

		// Automata: 
		while (true/*state != 9 && state != 4 && state != 5 && state != 8*/)
		{
			if (state == 0)
			{
				accept = false;
				start = current;
			}
			else if (state == 1)
			{
				accept = true;
				end = current;
			}
			else if (state == 4 || state == 5 || state == 8)
			{
				accept = true;
				end = current;
				break;
			}
			else if (state == 6 || state == 7)
			{
				end = current;
			}
			else if (state == 9)
			{
				break;
			}

			/*switch (state)
			{
			case 0:
				accept = false;
				start = current;
				break;
			case 1:
				accept = true;
				end = current;
				break;
			case 2:
				break;
			case 3:
				break;
			case 4:
				accept = true;
				end = current;
				break;
			case 5:
				accept = true;
				end = current;
				break;
			case 6:
				break;
			case 7:
				break;
			case 8:
				accept = true;
				end = current;
				break;
			case 9:
				break;
			default:
				break;
			}*/

			state = trans[state][*current];

			current++;
		}

		/*if (accept == true)
		{*/
		if (state == 5)
		{
				// add(body, new(Set, 0), 0);
		}
		else if (state == 4)
		{
			void * text = new(String, start + 1 , end - start - 2, 0);
			bool isTerminal = false;

			void * token = new (ProductionToken, text, isTerminal, 0);

			insert(body, token);

		} 
		else if (state == 8)
		{
			void * text = new (String, start + 1, end - start - 2, 0);
			bool isTerminal = true;

			void * token = new (ProductionToken, text, isTerminal, 0);

			insert(body, token);

		}
		else if (state == 9)
		{
			if (end > start)
			{
				void * text = new (String, start, end - start, 0);
				bool isTerminal = true;

				void * token = new (ProductionToken, text, isTerminal, 0);

				insert(body, token);

				current--;
			}
			else
			{
				assert(0);
			}
		}
		else
		{
			assert(0);
		}
		// }
	}

	
	return body;
}

static bool isLetter(char c)
{
	// Letter: {a-z, A-Z, 0-9}
	return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '*' || c == '|' || c == '(' || c == ')' || c == '-' || c == '\'' || c == '+';
}

static int initTrans(int trans[][256])
{
	int i, j;

	for (i = 0; i < 100; i++)
	{
		for (j = 0; j < 256; j++)
		{
			trans[i][j] = 9;
		}
	}

	setTrans(trans, 0, 1);
	setTrans(trans, 1, 1);
	trans[0]['{'] = 2;
	setTrans(trans, 2, 3);
	setTrans(trans, 3, 3);
	trans[3]['}'] = 4;
	// trans[0]['|'] = 5;
	trans[0]['['] = 6;
	// trans[6]['|'] = 7;
	setTrans(trans, 6, 7);
	trans[7][']'] = 8;
	setTrans(trans, 7, 7);
}

static void setTrans(int trans[][256], int source, int target)
{
	int i;

	for (i = 0; i < 256; i++)
	{
		if (isLetter(i))
		{
			trans[source][i] = target;
		}
	}
}




const void * Production;

const void * ProductionToken;

void loadProduction()
{
	if (!ProductionToken)
	{
		ProductionToken = new (Class, "ProductionToken", Class, sizeof(struct ProductionToken), ctor, ProductionToken_ctor, dtor, ProductionToken_dtor, toString, ProductionToken_toString, equals, ProductionToken_equals, clone, ProductionToken_clone, 0);
	}

	if (!Production)
	{
		Production = new (Class, "Production", Object, sizeof(struct Production), ctor, Production_ctor, dtor, Production_dtor, toString, Production_toString, clone, Production_clone, equals, Production_equals, 0);
	}
}