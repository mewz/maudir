/*
 *  base62.h
 *
 *  Created by Jason Hullinger on 5/27/10.
 *
 */

#ifndef __BASE_62_H_
#define __BASE_62_H_

#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

char* encode_base62(u_int64_t val);
u_int64_t decode_base62(char *str);

#endif
