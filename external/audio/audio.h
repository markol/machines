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
 * $NCDId: @(#)audio.h,v 1.37 1996/05/07 20:12:03 greg Exp $
 */

/*
 * 			     <audio/audio.h>
 * 
 * This file contains the constants and the primitive types (whose minimum
 * size can be determined and whose maximum size doesn't matter) that are 
 * used by clients and the server.
 */

#ifndef _NCD_AUDIO_H_
#define _NCD_AUDIO_H_

#include <audio/Amd.h>

/****************************************************************************
 *			      GENERIC DATA TYPES			    *
 ****************************************************************************/

/*
 * Data types used for resources.  They are designed to line up with X 
 * so that implementations may share code.
 */
typedef INT32			AuInt32; 	/* 32 bit integer */
typedef CARD32			AuUint32; 	/* 32 bit unsigned integer */
typedef INT16			AuInt16; 	/* 16 bit integer */
typedef CARD16			AuUint16; 	/* 16 bit unsigned integer */
typedef INT8			AuInt8;		/* 8 bit integer */
typedef CARD8			AuUint8;	/* 8 bit unsigned integer */

typedef AuUint32		AuID;		/* generic resource id */
typedef AuID			AuDeviceID;	/* physical or virtual */
typedef AuID			AuBucketID;	/* stored audio data */
typedef AuID			AuRadioID;	/* broadcast */
typedef AuID			AuFlowID;	/* how audio data moves */

typedef AuUint32		AuTime;		/* server time in millis */
typedef AuUint32		AuMask;		/* various value masks */
typedef int			AuBool;
typedef int			AuStatus;
typedef AuInt32 		AuFixedPoint;

#define AU_FIXED_POINT_SHIFT	(16)
#define AU_FIXED_POINT_SCALE	(1L << 16)
#define AU_FIXED_POINT_MASK	(0xffff)

/*
 * Fixed point numbers are simply signed integers that represent units of 
 * 65536.  Mathematically:
 * 
 *     fixedpoint = floor(floating * 65536)
 * 
 * It is important to note that although the value can be viewed as having
 * a easily identifiable integral and fractional portions, negative values
 * cannot simply be extracted and concatenated since the two pieces are 
 * actually related as:
 * 
 *     floating = (integerportion + fractionalportion)
 * 
 * rather than
 * 
 *     floating != (integerportion | fractionalportion)
 * 
 * To make manipulating the values easier, the following macros are provided:
 * 
 *     AuFixedPointFromSum		creates a fixed point from an
 * 					integer plus fraction.
 * 
 *     AuFixedPointFromFraction		creates a fixed point from a
 * 					numerator / denominator
 * 
 *     AuFixedPointRoundDown		rounds down to prev integer value
 * 
 *     AuFixedPointRoundUp		rounds up to next integer value
 * 
 *     AuFixedPointIntegralAddend	integer portion of sum
 * 
 *     AuFixedPointFractionalAddend	fractional portion of sum
 */

#define AuFixedPointFromSum(a,b) \
    ((AuInt32) (((AuInt32) (a) << AU_FIXED_POINT_SHIFT) + ((AuInt32) (b))))

#define AuFixedPointFromFraction(nnn,ddd) \
    ((((AuInt32) (nnn)) * AU_FIXED_POINT_SCALE) / ((AuInt32) (ddd)))

#define AuFixedPointRoundDown(fp) \
    ((fp) >> AU_FIXED_POINT_SHIFT)
#define AuFixedPointRoundUp(fp) \
    (AuFixedPointRoundDown((fp) + (AU_FIXED_POINT_SCALE - 1L)))

#define AuFixedPointIntegralAddend(fp) \
    (AuFixedPointRoundDown (fp))
#define AuFixedPointFractionalAddend(fp) \
    ((fp) & AU_FIXED_POINT_MASK)


#define AuNone			(0)		/* null resource */
#define AuFalse 		(0)
#define AuTrue			(1)



/*****************************************************************************
 *				   CONSTANTS				     *
 *****************************************************************************/

/*
 * Protocol Version number
 */
#define	AuProtocolMajorVersion			2
#define	AuProtocolMinorVersion			2


/*
 * Network types
 */
#define AuNetworkInternet			0
#define AuNetworkDECnet				1
#define AuNetworkChaos				2    /* reserved; match X */
#define AuNetworkNetname			254
#define AuNetworkLocal				256
#define AuNetworkWild				65535


/*
 * Access Control
 */
#define AuAccessNoChange		0
#define AuAccessEnableControls		1
#define AuAccessDisableControls		2

#define AuAccessAddUser			0
#define AuAccessRemoveUser		1


/*
 * Close Down Mode
 */
#define AuCloseDownDestroy			0
#define AuCloseDownRetainPermanent		1
#define AuCloseDownRetainTemporary		2



/*
 * Access Codes - these control what people are allowed to do with resources
 * that are created by a client.
 */
#define AuAccessImportMask		(1L << 0)
#define AuAccessExportMask		(1L << 1)
#define AuAccessDestroyMask		(1L << 2)
#define AuAccessListMask		(1L << 3)
#define AuAccessAllMasks	       ((1L << 4) - 1)


/*
 * String Types - how strings are encoded
 */
#define AuStringLatin1			(1L)
#define AuStringCompoundText		(2L)	/* later */


/*
 * Event Codes
 */
#define AuEventTypeElementNotify	2
#define AuEventTypeGrabNotify		3
#define AuEventTypeMonitorNotify	4
#define AuEventTypeBucketNotify		5    /* not yet */
#define AuEventTypeDeviceNotify		6    /* not yet */

#define AuFirstEventType		AuEventTypeElementNotify
#define AuLastEventType			AuEventTypeMonitorNotify



/*
 * Error Codes - note that they are designed to match up with core X protocol.
 */
#define AuSuccess			0    /* no problems */
#define AuBadRequest			1    /* bad request code */
#define AuBadValue			2    /* int value out of range */
#define AuBadDevice			3    /* bad physical device ID */
#define AuBadBucket			4    /* bad bucket ID */
#define AuBadFlow			5    /* bad flow ID */
#define AuBadElement			6    /* bad portal ID */

#define AuBadMatch			8    /* parameter mismatch */

#define AuBadAccess			10   /* permission denied */
#define AuBadAlloc			11   /* insufficient resources */
#define AuBadConnection			13   /* problem with network */
#define AuBadIDChoice			14   /* bad ID provided for Create */
#define AuBadName			15   /* named object doesn't exist */
#define AuBadLength			16   /* bad request length */
#define AuBadImplementation		17   /* bad server, no cookie */

#define	AuLastError			AuBadImplementation
#define AuFirstExtensionError		128
#define AuLastExtensionError		255


/*****************************************************************************
 *				 DATA FORMATS				     *
 *****************************************************************************/

/*
 * External Data Formats; taken from most common disk formats.  
 * the specific codes used do not match up exactly with the SND headers.
 * These map -1..+1 values into 8 or 16 bit fixed point representations of
 * the fractions.  The unsigned variants typically just offset by 128 or 32768.
 * 
 * We're going to assume that bit-order isn't a problem here.
 */
#define	AuFormatULAW8			1    /* logarithmic encoding */
#define AuFormatLinearUnsigned8		2    /* common PCM, offset */
#define AuFormatLinearSigned8		3    /* less common PCM */
#define AuFormatLinearSigned16MSB	4    /* common PCM */
#define AuFormatLinearUnsigned16MSB	5    /* PCM, offset */
#define AuFormatLinearSigned16LSB	6    /* common DEC & PC */
#define AuFormatLinearUnsigned16LSB	7    /* DEC & PC offset */

#define AuSizeofFormat(fff) \
    (((fff) < AuFormatULAW8 || (fff) > AuFormatLinearUnsigned16LSB) ? 0 : \
     (((fff) < AuFormatLinearSigned16MSB) ? 1 : 2))


#define AuUnlimitedSamples		0	/* an unlimited number of
						 * samples */

/****************************************************************************
 *				    DEVICES				    *
 ****************************************************************************/

/*
 * Device Attributes.  In addition to the ValueMask and ChangeMask, these are
 * the fields returned by the server to describe each device.  They are also
 * used by the client to change various fields.
 */
#define AuCompCommonIDMask		(1L << 0)
#define AuCompCommonKindMask		(1L << 1)
#define AuCompCommonUseMask		(1L << 2)
#define AuCompCommonFormatMask		(1L << 3)
#define AuCompCommonNumTracksMask	(1L << 4)
#define AuCompCommonAccessMask		(1L << 5)
#define AuCompCommonDescriptionMask	(1L << 6)
#define AuCompCommonMasks	       ((1L << 7) - 1)
#define AuCompCommonAllMasks		AuCompCommonMasks

#define AuCompDeviceMinSampleRateMask	(1L << 16)
#define AuCompDeviceMaxSampleRateMask	(1L << 17)
#define AuCompDeviceLocationMask	(1L << 18)
#define AuCompDeviceGainMask		(1L << 19)
#define AuCompDeviceLineModeMask	(1L << 20)
#define AuCompDeviceInputModeMask	AuCompDeviceLineModeMask
#define AuCompDeviceOutputModeMask	AuCompDeviceLineModeMask
#define AuCompDeviceChildrenMask	(1L << 21)
#define AuCompDeviceMasks	      (((1L << 22) - 1) & ~0xffff)
#define AuCompDeviceAllMasks		(AuCompDeviceMasks | \
					 AuCompCommonAllMasks)

#define AuCompBucketSampleRateMask	(1L << 16)
#define AuCompBucketNumSamplesMask	(1L << 17)
#define AuCompBucketMasks	      (((1L << 18) - 1) & ~0xffff)
#define AuCompBucketAllMasks	        (AuCompBucketMasks | \
					 AuCompCommonAllMasks)

#define AuCompRadioStationMask		(1L << 16)
#define AuCompRadioMasks	       ((1L << 17) - 1)
#define AuCompRadioAllMasks		(AuCompRadioMasks | \
					 AuCompCommonAllMasks)


/*
 * Component Kinds.
 */
#define AuComponentKindOther		0
#define AuComponentKindPhysicalInput	1
#define AuComponentKindPhysicalOutput	2
#define AuComponentKindBucket		3
#define AuComponentKindRadio		4
#define AuComponentKindPhysicalFeedback 5

/*
 * Component Use.  These describe whether or not the device can be used 
 * with ImportDevice and/or ExportDevice.
 * The ExclusiveMask for a device means that that device will override
 * other devices when used.  For example an ExclusiveMask on an input device
 * means that using that device will override the use of the output devices.
 * This usually means that the audio hardware cannot record and play
 * simultaneously.
 */
#define AuComponentUseImportMask	(1L << 0)
#define AuComponentUseExportMask	(1L << 1)
#define AuComponentUseExclusiveMask	(1L << 2)
#define AuComponentUseAllMasks	       ((1L << 3) - 1)

/*
 * Component Location Hints.  These describe where the device is likely to be,
 * relative to the user.  A device may be in more than one position.
 * The number of tracks determines whether or not the device is mono or stereo.
 */
#define	AuDeviceLocationLeftMask		(1L << 0)
#define	AuDeviceLocationCenterMask		(1L << 1)
#define	AuDeviceLocationRightMask		(1L << 2)
#define AuDeviceLocationTopMask			(1L << 3)
#define AuDeviceLocationMiddleMask		(1L << 4)
#define AuDeviceLocationBottomMask		(1L << 5)
#define AuDeviceLocationBackMask		(1L << 6)
#define AuDeviceLocationFrontMask		(1L << 7)
#define	AuDeviceLocationInternalMask		(1L << 8)
#define	AuDeviceLocationExternalMask		(1L << 9)
#define AuDeviceLocationAllMasks	       ((1L << 10) - 1)


/*
 * Component input device selection.
 *
 * These determine which amplifier circuits are used, typically to select 
 * between low gain pathways for CD players and other line-level devices and 
 * high gain pathways for microphones and record players. 
 */
#define AuDeviceInputModeNone			(0L)
#define AuDeviceInputModeLineIn			(1L)
#define AuDeviceInputModeMicrophone		(2L)

/* compatability... */
#define AuDeviceLineModeNone			AuDeviceInputModeNone
#define AuDeviceLineModeLow			AuDeviceInputModeLineIn
#define AuDeviceLineModeHigh			AuDeviceInputModeMicrophone

/*
 * Component output device selection.
 *
 * These may be or'd together on certain systems to select multiple 
 * devices simultaneously.
 */
#define AuDeviceOutputModeNone			(0L)
#define AuDeviceOutputModeSpeaker		(1L)
#define AuDeviceOutputModeHeadphone		(2L)
#define AuDeviceOutputModeLineOut		(4L)


/*****************************************************************************
 *				     FLOWS				     *
 *****************************************************************************/

/*
 * Elements in a Flow.  These are the primitives used to get data from
 * one place to another.
 */
#define AuElementTypeImportClient	0
#define AuElementTypeImportDevice	1
#define AuElementTypeImportBucket	2
#define AuElementTypeImportWaveForm	3
#define AuElementTypeImportRadio	4    /* not implemented yet */
#define AuElementTypeBundle		5
#define AuElementTypeMultiplyConstant	6
#define AuElementTypeAddConstant	7
#define AuElementTypeSum		8
#define AuElementTypeExportClient	9
#define AuElementTypeExportDevice	10
#define AuElementTypeExportBucket	11
#define AuElementTypeExportRadio	12    /* not implemented yet */
#define AuElementTypeExportMonitor	13


#define AU_MAX_ELEMENTS			254
#define AuElementAll			255

#define AU_MAX_PARMS			5
#define AuParmsImportClient		0
#define AuParmsImportDevice		0
#define AuParmsImportBucket		1    /* offset */
#define   AuParmsImportBucketOffset		0
#define AuParmsImportWaveForm		2    /* frequency, num samples */
#define   AuParmsImportWaveFormFrequency	0
#define   AuParmsImportWaveFormNumSamples	1
#define AuParmsImportRadio		0
#define AuParmsBundle			0
#define AuParmsMultiplyConstant		1    /* integer, fraction */
#define   AuParmsMultiplyConstantConstant	0
#define AuParmsAddConstant		1    /* integer, fraction */
#define   AuParmsAddConstantConstant		0
#define AuParmsSum			0
#define AuParmsExportClient		0
#define AuParmsExportDevice		0
#define AuParmsExportBucket		1    /* offset */
#define   AuParmsExportBucketOffset		0
#define AuParmsExportRadio		0
#define AuParmsExportMonitor		0

/*
 * Import and Export Element States.  These values are used explicitly
 * when setting the state, and in any state actions.
 */
#define AuStateStop			0
#define AuStateStart			1
#define AuStatePause			2
#define AuStateAny			255

#define AuElementActionChangeState	0
#define AuElementActionSendNotify	1
#define AuElementActionNoop		2


/*
 * Wave forms
 */
#define AuWaveFormSquare		0
#define AuWaveFormSine			1
#define AuWaveFormSaw			2
#define AuWaveFormConstant		3


/*
 * ElementNotify event kinds.
 */
#define AuElementNotifyKindLowWater	0
#define AuElementNotifyKindHighWater	1
#define AuElementNotifyKindState	2

/*
 * ElementNotify reasons
 */
#define AuReasonUser			0
#define AuReasonUnderrun		1
#define AuReasonOverrun			2
#define AuReasonEOF			3
#define AuReasonWatermark		4
#define AuReasonHardware		5
#define AuReasonAny			255

/*
 * GrabNotify event kinds.
 */
#define AuGrabNotifyKindRequested	0
#define AuGrabNotifyKindReleased	1



/*
 * Data transfer state
 */
#define AuTransferStateReady		0
#define AuTransferStatePending		1
#define AuTransferStateEnd		2


#endif /* _NCD_AUDIO_H_ */
