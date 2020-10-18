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
 * $NCDId: @(#)wave.h,v 1.10 1995/05/23 23:40:21 greg Exp $
 */

/*
 * WAVE support library
 */

#ifndef _WAVE_H_
#define _WAVE_H_

#include <audio/audio.h>	/* for AuInt32 and AuUint32 */

#ifndef _WaveConst
#define _WaveConst const
#endif						/* _WaveConst */

#ifndef _FUNCPROTOBEGIN
#ifdef __cplusplus			/* for C++ V2.0 */
#define _FUNCPROTOBEGIN extern "C" {	/* do not leave open across includes */
#define _FUNCPROTOEND }
#else
#define _FUNCPROTOBEGIN
#define _FUNCPROTOEND
#endif
#endif /* _FUNCPROTOBEGIN */

#define RIFF_RiffID			"RIFF"
#define RIFF_WaveID			"WAVE"
#define RIFF_ListID			"LIST"
#define RIFF_ListInfoID			"INFO"

#define RIFF_InfoIcmtID			"ICMT"

#define RIFF_WaveFmtID			"fmt "
#define RIFF_WaveFmtSize		16
#define RIFF_WaveDataID			"data"

#define	RIFF_WAVE_FORMAT_PCM		0x0001	/* Microsoft PCM */
#define	RIFF_IBM_FORMAT_MULAW		0x0101	/* IBM mu-law format */
#define	RIFF_IBM_FORMAT_ALAW		0x0102	/* IBM a-law format */
#define	RIFF_IBM_FORMAT_ADPCM		0x0103	/* IBM AVC Adaptive
						 * Differential Pulse Code
						 * Modulation format */
typedef AuUint32 RIFF_FOURCC;

typedef struct
{
    RIFF_FOURCC     ckID;			/* chunk ID */
    AuInt32            ckSize;			/* chunk size, in bytes */
}               RiffChunk;

typedef struct
{
    FILE           *fp;
    char           *comment;
    short           channels,
                    bitsPerSample;
    AuInt32            sampleRate;
    AuUint32   dataOffset,
                    numSamples;

    /* private data */
    AuUint32 fileSize, dataSize, sizeOffset;
    unsigned int writing;
    short format;
}               WaveInfo;

/*****************************************************************************
 *			       PUBLIC INTERFACES			     *
 *****************************************************************************/

_FUNCPROTOBEGIN

extern WaveInfo *
WaveOpenFileForReading(
		       const char *		/* file name */
);

extern WaveInfo *
WaveOpenFileForWriting(
		       const char *,		/* file name */
		       WaveInfo *		/* info */
);

extern int
WaveCloseFile(
	      WaveInfo *			/* info */
);

extern int
WaveReadFile(
	     char *,				/* buffer */
	     int,				/* num bytes */
	     WaveInfo *				/* info */
);

extern int
WaveWriteFile(
	      char *,				/* buffer */
	      int,				/* num bytes */
	      WaveInfo *			/* info */
);

extern int
WaveSeekFile(
	      int,                              /* number of audio bytes */
	      WaveInfo *			/* info */
);

extern int
WaveTellFile(
	      WaveInfo *			/* info */
);

extern int
WaveFlushFile(
	      WaveInfo *			/* info */
);

extern int
WaveRewindFile(
	       WaveInfo *			/* info */
);

_FUNCPROTOEND

#endif						/* _WAVE_H_ */
