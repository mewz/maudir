/*
 *  base62.c
 *
 *  Created by Jason Hullinger on 5/27/10.
 *
 */

#include "base62.h"

static const char base_vals[] = "0123456789"
                                "abcdefghijklmnopqrstuvwxyz"
                                "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

/**
 * encode_base62:
 * @val: A #guint64.
 *
 * Creates a newly allocated string with @val encoded in base62.
 *
 * Returns: A newly allocated string which should be freed with g_free().
 * Side effects: None.
 */
char*
encode_base62 (guint64 val) /* IN */
{
	char *str = (char*)malloc(1);
	memset(str, '\0', 1);
	do{
		int i = val % 62;
		str = (char*)realloc(str, sizeof(char)*strlen(str)+2);
		str[strlen(str)+1] = '\0';
		str[strlen(str)] = base_vals[i];
		val = (val - i) / 62;
	}while(val > 0);
	g_strreverse(str);
	return str;	
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
guint64
decode_base62 (char *str) /* IN */
{
	int len = strlen(str);
	u_int64_t val = 0;
	for(int i = 0; i < len; i ++){
		char c = str[i];
		if(!isalnum(c)){
			return -1;
		}
		int j;
		int len = sizeof(base_vals);
		for(j = 0; j < len; j++){
			if(base_vals[j] == c) break;
		}
		val += j * pow(62, len-i-1);
	}
	return val;	
}
