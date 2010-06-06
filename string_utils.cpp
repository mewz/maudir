/*
 *  string_utils.cpp
 *  maudir
 *
 *  Created by Jason Hullinger on 6/2/10.
 *
 */

#include <assert.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#include "string_utils.h"

/*
 *----------------------------------------------------------------------------
 *
 * strtolower --
 *
 *     Convert an ASCII string to its lowercase equivalent.  It is an error to
 *     call this method with an UTF-8 or any non-ASCII string.
 *
 * Returns:
 *      A newly allocated string which should be freed with free().
 *
 * Side effects:
 *      None.
 *
 *----------------------------------------------------------------------------
 */
char*
strtolower(const char *inStr) /* IN */
{
	char *outStr;
	int i;
	int len;

	assert(inStr);

	len = strlen(inStr);
	outStr = (char *)malloc(sizeof(char) * (len + 1));
	for (i = 0; i < len; i++) {
		outStr[i] = tolower(inStr[i]);
	}
	outStr[len + 1] = '\0';
	return outStr;
}

/*
 *----------------------------------------------------------------------------
 *
 * strreverse_inplace --
 *
 *     Reverses a string in place.  This method is not UTF-8 safe.  It is an
 *     error to call it with anything than an ASCII encoded string.
 *
 * Returns:
 *      None.
 *
 * Side effects:
 *      None.
 *
 *----------------------------------------------------------------------------
 */

void
strreverse_inplace (char *str) /* IN */
{
	char c;
	int half;
	int len;
	int i;

	assert(str);

	len = strlen(str);
	half = len >> 1;
	for (i = 0; i < half; i++) {
		c = str[i];
		str[i] = str[len - i - 1];
		str[len - i - 1] = c;
	}
}
