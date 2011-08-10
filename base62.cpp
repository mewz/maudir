/*
 *  base62.c
 *
 *  Created by Jason Hullinger on 5/27/10.
 */

#include <assert.h>
#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "base62.h"
#include "string_utils.h"

static const char base62_vals[] = "0123456789"
                                  "abcdefghijklmnopqrstuvwxyz"
                                  "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
static const int base62_index[] = {
	   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0, 
	   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0, 
	   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0, 
	   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0, 
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,    0,    0, 
	   0,    0,    0,    0,    0, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 
	0x2b, 0x2c, 0x2d, 0x2e, 0x2f, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 
	0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d,    0,    0,    0,    0,    0, 
	   0, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11, 0x12, 0x13, 0x14, 
	0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20, 
	0x21, 0x22, 0x23, 
};

/*
 *----------------------------------------------------------------------------
 *
 * base62_encode --
 *
 *     Encodes an u_int64_t into a base62 encoded string.  str should be
 *     a pointer to an allocation to store the encoded string.  len should
 *     be the size of the buffer outStr in bytes.
 *
 *     If there is not enough space in str, then FALSE is returned.  However,
 *     a length of 12 should fit all possible results.
 *
 * Returns:
 *     true if successful; otherwise false.
 *
 * Side effects:
 *     None.
 *
 *----------------------------------------------------------------------------
 */

bool
base62_encode (u_int64_t  val, /* IN */
               char      *str, /* OUT */
               size_t     len) /* IN */
{
	int i = 0;
	int v;

	assert(str);
	assert(len > 0);

	do {
		if (i + 1 >= len)
			return false;
		v = val % 62;
		str[i++] = base62_vals[v];
		val = (val - v) / 62;
	} while (val > 0);
	str[i] = '\0';
	strreverse_inplace(str);
	return true;
}

/*
 *----------------------------------------------------------------------------
 *
 * base62_decode --
 *
 *     Decodes a base62 encoded string back into a 64-bit unsigned integer.
 *
 * Returns:
 *     The decoded 64-bit unsigned integer.
 *
 * Side effects:
 *     None.
 *
 *----------------------------------------------------------------------------
 */

u_int64_t
base62_decode (const char *str) /* IN */
{
	u_int64_t val = 0;
	char c;
	int len;
	int i;
	//int j;

	assert(str);

	len = strlen(str);
	for (i = 0; i < len; i++) {
		c = str[i];
		if (!isalnum(c)) {
			return -1;
		}
		val += base62_index[c] * powl(62, len - i - 1);
	}
	return val;	
}
