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
 * $NCDId: @(#)Aosdefs.h,v 1.4 1995/12/28 19:48:15 greg Exp $
 */

/* Portions derived from */
/*
 * O/S-dependent (mis)feature macro definitions
 *
 * $XConsortium: Xosdefs.h,v 1.7 91/07/19 23:22:19 rws Exp $
 *
 * Copyright 1991 Massachusetts Institute of Technology
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of M.I.T. not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  M.I.T. makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * M.I.T. DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL M.I.T.
 * BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN 
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef _AOSDEFS_H_
#define _AOSDEFS_H_

/*
 * AU_NOT_STDC_ENV means does not have ANSI C header files.  Lack of this
 * symbol does NOT mean that the system has stdarg.h.
 *
 * AU_NOT_POSIX means does not have POSIX header files.  Lack of this
 * symbol does NOT mean that the POSIX environment is the default.
 * You may still have to define _POSIX_SOURCE to get it.
 */

#ifdef NOSTDHDRS
#define AU_NOT_POSIX
#define AU_NOT_STDC_ENV
#endif

#ifdef sony
#ifndef SYSTYPE_SYSV
#define AU_NOT_POSIX
#endif
#endif

#ifdef UTEK
#define AU_NOT_POSIX
#define AU_NOT_STDC_ENV
#endif

#ifdef CRAY
#define AU_NOT_POSIX
#endif

#ifdef vax
#ifndef ultrix			/* assume vanilla BSD */
#define AU_NOT_POSIX
#define AU_NOT_STDC_ENV
#endif
#endif

#ifdef luna
#define AU_NOT_POSIX
#define AU_NOT_STDC_ENV
#endif

#ifdef Mips
#define AU_NOT_POSIX
#define AU_NOT_STDC_ENV
#endif
  
#ifdef USL
#ifdef SYSV /* (release 3.2) */
#define AU_NOT_POSIX
#define AU_NOT_STDC_ENV
#endif
#endif

#ifdef SYSV386
#ifdef SYSV
#define AU_NOT_POSIX
#define AU_NOT_STDC_ENV
#endif
#endif

#ifdef MOTOROLA
#ifdef SYSV
#define AU_NOT_STDC_ENV
#endif
#endif

#ifdef NCD
#define AU_NOT_POSIX
#define AU_NOT_STDC_ENV
#endif

#ifndef WIN32
#define AUCDECL
#else /* WIN32 */
#define AU_NOT_POSIX
#define SYSV
#define _POSIX_
#define AUCDECL __cdecl
#endif /* WIN32 */

#endif /* _AOSDEFS_H_ */
