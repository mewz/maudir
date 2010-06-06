/*
 *  base62.c
 *
 *  Created by Jason Hullinger on 5/27/10.
 *
 */

#include "base62.h"

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

/**
 * encode_base62:
 * @val: A #guint64.
 * @str: A location to store the encoded string.
 * @len: The length of @str.
 *
 * Encodes @val into a base62 encoded string and stores it in @str.  @str
 * should be at least 12 bytes to store all possible encodings.
 *
 * Returns: %TRUE if successful; otherwise %FALSE.
 * Side effects: None.
 */
gboolean
base62_encode_uint64 (guint64  val, /* IN */
                      char    *str, /* OUT */
                      int      len) /* IN */
{

	int i = 0;
	int v;

	g_return_val_if_fail(str != NULL, FALSE);
	g_return_val_if_fail(len > 0, FALSE);

	do {
		if (i + 1 >= len)
			return FALSE;
		v = val % 62;
		str[i++] = base62_vals[v];
		val = (val - v) / 62;
	} while (val > 0);
	str[i] = '\0';
	g_strreverse(str);
	return TRUE;
}

/**
 * decode_base62:
 * @str: A base62 encoded string.
 * @v_uint64: A location for a #guint64. TODO
 *
 * Decodes a #guint64 from a base62 encoded string.
 *
 * Returns: %TRUE if successful; otherwise %FALSE.
 * Side effects: None.
 */
gboolean
base62_decode_uint64 (const char *str, /* IN */
                      guint64    *val) /* IN */
{
	guint64 v = 0;
	char c;
	int len;
	int i;

	g_return_val_if_fail(str != NULL, FALSE);
	g_return_val_if_fail(val != NULL, FALSE);

	len = strlen(str);
	for (i = 0; i < len; i++) {
		c = str[i];
		if (!g_ascii_isalnum(c)) {
			return FALSE;
		}
		v += base62_index[(int)c] * pow(62, len - i - 1);
	}
	*val = v;
	return TRUE;	
}
