/*
 * Copyright 1993 Network Computing Devices, Inc.
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name Network Computing Devices, Inc. not be
 * used in advertising or publicity pertaining to distribution of this
 * software without specific, written prior permission.
 *
 * THIS SOFTWARE IS PROVIDED 'AS-IS'.  NETWORK COMPUTING DEVICES, INC.,
 * DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING WITHOUT
 * LIMITATION ALL IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE, OR NONINFRINGEMENT.  IN NO EVENT SHALL NETWORK
 * COMPUTING DEVICES, INC., BE LIABLE FOR ANY DAMAGES WHATSOEVER, INCLUDING
 * SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES, INCLUDING LOSS OF USE, DATA,
 * OR PROFITS, EVEN IF ADVISED OF THE POSSIBILITY THEREOF, AND REGARDLESS OF
 * WHETHER IN AN ACTION IN CONTRACT, TORT OR NEGLIGENCE, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * $NCDId: @(#)fileutil.c,v 1.9 1994/04/07 20:29:58 greg Exp $
 */

/*
 * sound file reading util functions
 */

//#include "config.h"

#include	<stdio.h>
#include	<audio/Aos.h>
#include	<audio/fileutil.h>

#if defined(HAVE_STDLIB_H)
# include <stdlib.h>
#endif

#if defined(HAVE_MALLOC_H)
# include <malloc.h>
#endif



unsigned short FileSwapS (unsigned short us)
{
    return ((us >> 8) | (us << 8)) & 0xffff;
}

AuUint32
FileSwapL(AuUint32 ul)
{
    return (ul >> 24) | ((ul >> 8) & 0xff00) | ((ul << 8) & 0xff0000) |
	(ul << 24);
}

unsigned short
FileReadS(
          FILE       *fp,
          int         swapit
          )
{
    unsigned short us;

    fread(&us, 2, 1, fp);
    if (swapit)
	us = FileSwapS(us);
    return us;
}

AuUint32
FileReadL(
          FILE       *fp,
          int         swapit
          )
{
    AuUint32 ul;

    fread(&ul, 4, 1, fp);
    if (swapit)
	ul = FileSwapL(ul);
    return ul;
}

int
FileWriteS (unsigned short us, FILE *fp, int swapit)
{
    if (swapit)
	us = FileSwapS(us);
    return fwrite(&us, 2, 1, fp);
}

int
FileWriteL(
           AuUint32 ul,
           FILE       *fp,
           int         swapit
           )
{
    if (swapit)
	ul = FileSwapL(ul);
    return fwrite(&ul, 4, 1, fp);
}

char       *
FileCommentFromFilename(const char *fname)
{
    const char *f;
    char       *t;

    /* ignore piped files */
    if (strcmp(fname, "-")) {
#ifdef m88k
	f = strrchr(fname, '/');
#else
	f = rindex(fname, '/');
#endif
	if (!f)
	    f = fname;
	else
	    f++;
	t = (char *) malloc(strlen(f) + 1);
	if (t) {
	    strcpy(t, f);
	}
    } else {
	t = (char *) malloc(1);
	if (t) {
	    *t = '\0';
	}
    }
    return t;
}
