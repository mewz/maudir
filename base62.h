/*
 *  base62.h
 *
 *  Created by Jason Hullinger on 5/27/10.
 *
 */

#ifndef __BASE_62_H_
#define __BASE_62_H_

bool      base62_encode (u_int64_t   val, /* IN */
                         char       *str, /* OUT */
                         size_t      len); /* IN */
u_int64_t base62_decode (const char *str); /* IN */

#endif
