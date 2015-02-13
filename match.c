/* Search through string src for string arg, returns positive value for match */
/* cis = 0 case sensitive search, case insensitive = 1 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>

int match(char src[], char arg[], int cis)
{

int loop = 0, count = 0, ismatch = 0;
char a, s;

if (strlen(arg) > strlen(src))
	return 0;

while ((ismatch == 0) && (loop < strlen(src)))
{
if (cis)
{
	a = tolower(arg[count]);
        s = tolower(src[loop]);
	}
else
{
	a = arg[count];
        s = src[loop];
	}

if (a == s)
{
	count = count + 1;
        if (count == strlen(arg))
	{
		ismatch = loop;
		}
	}
else
{
	count = 0;
	}
loop = loop + 1;
}

return ismatch;
}
