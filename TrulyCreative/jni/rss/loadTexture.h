/*
 * Copyright (C) 2002  Tugrul Galatali <tugrul@galatali.com>
 *
 * rsMath is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as 
 * published by the Free Software Foundation.
 *
 * rsMath is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef LOADTEXTURE_H
#define LOADTEXTURE_H

#include "config.h"

#ifndef HAVE_BZLIB_H
#undef HAVE_LIBBZ2
#endif

#ifdef HAVE_LIBBZ2

#include <bzlib.h>

#define LOAD_TEXTURE(dest, src, compressedSize, size) dest = (unsigned char *)malloc(size); BZ2_bzBuffToBuffDecompress((char *)dest, &size, (char *)src, compressedSize, 0, 0);

#define FREE_TEXTURE(tex) free(tex);

#else

#define LOAD_TEXTURE(dest, src, compressedSize, size) dest = src; 
#define FREE_TEXTURE(tex) tex = 0;

#endif

#endif
