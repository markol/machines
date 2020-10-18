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
 * $NCDId: @(#)wave.c,v 1.26 1996/04/29 21:48:08 greg Exp $
 */

/*
 * WAVE support library
 */

//#include "config.h"

#include	<stdio.h>

#if defined(HAVE_STDLIB_H)
# include <stdlib.h>
#endif

#if defined(HAVE_MALLOC_H)
# include <malloc.h>
#endif


#include	<audio/Aos.h>
#include	<audio/wave.h>
#include	<audio/fileutil.h>

#define Err()		{ WaveCloseFile(wi); return NULL; }
#define readFourcc(_f)	fread(_f, sizeof(RIFF_FOURCC), 1, wi->fp)
#define cmpID(_x, _y)							      \
    strncmp((const char *) (_x), (const char *) (_y), sizeof(RIFF_FOURCC))
#define PAD2(_x)	(((_x) + 1) & ~1)

static int
readChunk(
          RiffChunk      *c,
          FILE           *fp
          )
{
    int             status;
    char            n;

    if ((status = fread(c, sizeof(RiffChunk), 1, fp)))
	if (NAS_BIG_ENDIAN)
	    swapl(&c->ckSize, n);

    return status;
}

WaveInfo       *
WaveOpenFileForReading(const char *name)
{
    RiffChunk       ck;
    RIFF_FOURCC     fourcc;
    AuInt32            fileSize;
    WaveInfo       *wi;

    if (!(wi = (WaveInfo *) malloc(sizeof(WaveInfo))))
	return NULL;

    wi->comment = NULL;
    wi->dataOffset = wi->format = wi->writing = 0;

    if (!(wi->fp = fopen(name, AU_READ_BINARY)))
	Err();

    if (!readChunk(&ck, wi->fp) ||
	cmpID(&ck.ckID, RIFF_RiffID) ||
	!readFourcc(&fourcc) ||
	cmpID(&fourcc, RIFF_WaveID))
	Err();

    fileSize = PAD2(ck.ckSize) - sizeof(RIFF_FOURCC);

    while (fileSize >= (AuInt32)sizeof(RiffChunk))
    {
	if (!readChunk(&ck, wi->fp))
	    Err();

	fileSize -= sizeof(RiffChunk) + PAD2(ck.ckSize);

	/* LIST chunk */
	if (!cmpID(&ck.ckID, RIFF_ListID))
	{
	    if (!readFourcc(&fourcc))
		Err();

	    /* INFO chunk */
	    if (!cmpID(&fourcc, RIFF_ListInfoID))
	    {
		ck.ckSize -= sizeof(RIFF_FOURCC);

		while (ck.ckSize)
		{
		    RiffChunk       c;

		    if (!readChunk(&c, wi->fp))
			Err();

		    /* ICMT chunk */
		    if (!cmpID(&c.ckID, RIFF_InfoIcmtID))
		    {
			if (!(wi->comment = (char *) malloc(c.ckSize)) ||
			    !fread(wi->comment, c.ckSize, 1, wi->fp))
			    Err();

			if (c.ckSize & 1)
			    fgetc(wi->fp);	/* eat the pad byte */
		    }
		    else
			/* skip unknown chunk */
			fseek(wi->fp, PAD2(c.ckSize), 1);

		    ck.ckSize -= sizeof(RiffChunk) + PAD2(c.ckSize);
		}
	    }
	    else
		/* skip unknown chunk */
		fseek(wi->fp, PAD2(ck.ckSize) - sizeof(RIFF_FOURCC), 1);
	}
	/* wave format chunk */
	else if (!cmpID(&ck.ckID, RIFF_WaveFmtID) && !wi->format)
	{
	    AuInt32            dummy;

	    wi->format = FileReadS(wi->fp, NAS_BIG_ENDIAN);
	    wi->channels = FileReadS(wi->fp, NAS_BIG_ENDIAN);
	    wi->sampleRate = FileReadL(wi->fp, NAS_BIG_ENDIAN);

	    /* we don't care about the next two fields */
	    dummy = FileReadL(wi->fp, NAS_BIG_ENDIAN);
	    dummy = FileReadS(wi->fp, NAS_BIG_ENDIAN);

	    if (wi->format != RIFF_WAVE_FORMAT_PCM)
		Err();

	    wi->bitsPerSample = FileReadS(wi->fp, NAS_BIG_ENDIAN);

	    /* skip any other format specific fields */
	    fseek(wi->fp, PAD2(ck.ckSize - 16), 1);
	}
	/* wave data chunk */
	else if (!cmpID(&ck.ckID, RIFF_WaveDataID) && !wi->dataOffset)
	{
	    long endOfFile;

	    wi->dataOffset = ftell(wi->fp);
	    wi->dataSize = ck.ckSize;
	    fseek(wi->fp, 0, 2);
	    endOfFile = ftell(wi->fp);

	    /* seek past the data */
	    if (fseek(wi->fp, wi->dataOffset + PAD2(ck.ckSize), 0) ||
		ftell(wi->fp) > endOfFile)
	    {
		/* the seek failed, assume the size is bogus */
		fseek(wi->fp, 0, 2);
		wi->dataSize = ftell(wi->fp) - wi->dataOffset;
	    }

	    wi->dataOffset -= sizeof(long);
	}
	else
	    /* skip unknown chunk */
	    fseek(wi->fp, PAD2(ck.ckSize), 1);
    }

    if (!wi->dataOffset)
	Err();

    wi->numSamples = wi->dataSize / wi->channels / (wi->bitsPerSample >> 3);

    if (!wi->comment)
	wi->comment = FileCommentFromFilename(name);

    WaveRewindFile(wi);
    return wi;
}

WaveInfo       *
WaveOpenFileForWriting(
                       const char *name,
                       WaveInfo       *wi
                       )
{
    int             n;

    wi->writing = wi->dataSize = 0;

    if (!(wi->fp = fopen(name, AU_WRITE_BINARY)) ||
	!fwrite(RIFF_RiffID, sizeof(RIFF_FOURCC), 1, wi->fp))
	Err();

    wi->sizeOffset = ftell(wi->fp);

    if (!FileWriteL(0, wi->fp, NAS_BIG_ENDIAN) ||
	!fwrite(RIFF_WaveID, sizeof(RIFF_FOURCC), 1, wi->fp))
	Err();

    wi->fileSize = sizeof(RIFF_FOURCC);

    if ((n = strlen(wi->comment)))
    {
	int             size;

	n++;
	size = sizeof(RiffChunk) + sizeof(RIFF_FOURCC) + PAD2(n);

	if (!fwrite(RIFF_ListID, sizeof(RIFF_FOURCC), 1, wi->fp) ||
	    !FileWriteL(size, wi->fp, NAS_BIG_ENDIAN) ||
	    !fwrite(RIFF_ListInfoID, sizeof(RIFF_FOURCC), 1, wi->fp) ||
	    !fwrite(RIFF_InfoIcmtID, sizeof(RIFF_FOURCC), 1, wi->fp) ||
	    !FileWriteL(n, wi->fp, NAS_BIG_ENDIAN) ||
	    !fwrite(wi->comment, n, 1, wi->fp))
	    Err();

	if (n & 1)
	    fputc(0, wi->fp);	/* pad the comment */

	wi->fileSize += sizeof(RiffChunk) + size;
    }

    if (!fwrite(RIFF_WaveFmtID, sizeof(RIFF_FOURCC), 1, wi->fp) ||
	!FileWriteL(RIFF_WaveFmtSize, wi->fp, NAS_BIG_ENDIAN) ||
	!FileWriteS(RIFF_WAVE_FORMAT_PCM, wi->fp, NAS_BIG_ENDIAN) ||
	!FileWriteS(wi->channels, wi->fp, NAS_BIG_ENDIAN) ||
	!FileWriteL(wi->sampleRate, wi->fp, NAS_BIG_ENDIAN) ||
	!FileWriteL(wi->channels * wi->sampleRate * (wi->bitsPerSample >> 3),
		    wi->fp, NAS_BIG_ENDIAN) ||
	!FileWriteS(wi->channels * (wi->bitsPerSample >> 3), wi->fp,
		    NAS_BIG_ENDIAN) ||
	!FileWriteS(wi->bitsPerSample, wi->fp, NAS_BIG_ENDIAN))
	Err();

    wi->fileSize += sizeof(RiffChunk) + RIFF_WaveFmtSize;

    if (!fwrite(RIFF_WaveDataID, sizeof(RIFF_FOURCC), 1, wi->fp))
	Err();

    wi->dataOffset = ftell(wi->fp);

    if (!FileWriteL(0, wi->fp, NAS_BIG_ENDIAN))
	Err();

    wi->fileSize += sizeof(RiffChunk);

    wi->writing = 1;
    return wi;
}

int
WaveCloseFile(WaveInfo *wi)
{
    int             status = 0;

    if (wi->fp)
    {
	if (wi->writing)
	{
	    if (wi->dataSize & 1)
		fputc(0, wi->fp);	/* pad the data */

	    fseek(wi->fp, wi->sizeOffset, 0);
	    FileWriteL(wi->fileSize + PAD2(wi->dataSize), wi->fp, NAS_BIG_ENDIAN);
	    fseek(wi->fp, wi->dataOffset, 0);
	    FileWriteL(wi->dataSize, wi->fp, NAS_BIG_ENDIAN);
	}

	status = fclose(wi->fp);
    }

    if (wi->comment)
	free(wi->comment);

    free(wi);
    return status;
}

int
WaveReadFile(
             char           *p,
             int             n,
             WaveInfo       *wi
             )
{
    return fread(p, 1, n, wi->fp);
}

int
WaveWriteFile(
              char           *p,
              int             n,
              WaveInfo       *wi
              )
{
    int             num;

    num = fwrite(p, 1, n, wi->fp);
    wi->dataSize += num;
    return num;
}

int
WaveRewindFile(WaveInfo *wi)
{
    return wi->writing ? -1 : fseek(wi->fp, wi->dataOffset + sizeof(long), 0);
}

int
WaveSeekFile(
             int              n,
             WaveInfo       *wi
             )
{
    return wi->writing ? -1 :
	fseek(wi->fp, wi->dataOffset + sizeof(long) + n, 0);
}

int
WaveTellFile(WaveInfo *wi)
{
    return wi->writing ? -1 :
	ftell(wi->fp) - wi->dataOffset - sizeof(long);
}

int
WaveFlushFile(WaveInfo *wi)
{
    return fflush(wi->fp);
}
