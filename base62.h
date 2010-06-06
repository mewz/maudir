/*
 *  base62.h
 *
 *  Created by Jason Hullinger on 5/27/10.
 *
 */

#ifndef __BASE_62_H_
#define __BASE_62_H_

#include <glib.h>

gboolean base62_encode_uint64 (guint64     val,
                               char       *str,
                               int         len);
gboolean base62_decode_uint64 (const char *str,
                               guint64    *val);

#endif
