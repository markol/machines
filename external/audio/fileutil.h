/*
 * Copyright 1993 Network Computing Devices, Inc.
 *
 * Permission to use, copy, modify, distribute, and sell this software and
 * its documentation for any purpose is hereby granted without fee, provided
 * that the above copyright notice appear in all copies and that both that
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
 * $NCDId: @(#)fileutil.h,v 1.7 1994/09/01 01:14:27 greg Exp $
 */

#ifndef	_FILEUTIL_H_
#define	_FILEUTIL_H_

#include <audio/audio.h>	/* for AuInt32 and AuUint32 */

#ifndef _FileConst
#define _FileConst const
#endif				/* _FileConst */

#ifndef _FUNCPROTOBEGIN
#ifdef __cplusplus			/* for C++ V2.0 */
#define _FUNCPROTOBEGIN extern "C" {	/* do not leave open across includes */
#define _FUNCPROTOEND }
#else
#define _FUNCPROTOBEGIN
#define _FUNCPROTOEND
#endif
#endif /* _FUNCPROTOBEGIN */

#define NAS_LITTLE_ENDIAN 		(*(char *) &endian)
#define NAS_BIG_ENDIAN 		(!NAS_LITTLE_ENDIAN)
static int      endian = 1;

/* byte swap a AuInt32 */
#define swapl(x, n) { n = ((char *) (x))[0];\
	     ((char *) (x))[0] = ((char *) (x))[3];\
	     ((char *) (x))[3] = n;\
	     n = ((char *) (x))[1];\
	     ((char *) (x))[1] = ((char *) (x))[2];\
	     ((char *) (x))[2] = n; }


/* byte swap a short */
#define swaps(x, n) { n = ((char *) (x))[0];\
		 ((char *) (x))[0] = ((char *) (x))[1];\
		 ((char *) (x))[1] = n; }

_FUNCPROTOBEGIN

#ifndef mips
extern unsigned short
FileSwapS(
			unsigned short
);
#endif	/* mips */

extern AuUint32
FileSwapL(
			AuUint32
);

extern unsigned short
FileReadS(
			FILE	*,
                        int			/* swap? */
);

extern AuUint32
FileReadL(
			FILE	*,
                        int			/* swap? */
);

#ifndef mips
extern int
FileWriteS(
                        unsigned short,
			FILE	*,
                        int			/* swap? */
);
#endif

extern int
FileWriteL(
                        AuUint32,
			FILE	*,
                        int			/* swap? */
);

extern char	*
FileCommentFromFilename(
                        const char	*		/* file name */
);

_FUNCPROTOEND

#endif				/* _FILEUTIL_H_ */
