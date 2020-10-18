/*********************************************************

  L I N E D I A G . I P P
  Copyright (c) 1998 Charybdis Limited, All rights reserved.

***********************************************************/

#ifdef _INLINE
	#define _CODE_INLINE inline
#else
	#define _CODE_INLINE
#endif

_CODE_INLINE
RenILinesDiagnostic::DrawMethod RenILinesDiagnostic::horizontalResult( void ) const
{
	PRE(hasTestedLines());

	return horizontalResult_;
}

_CODE_INLINE
RenILinesDiagnostic::DrawMethod RenILinesDiagnostic::verticalResult( void ) const
{
	PRE(hasTestedLines());

	return verticalResult_;
}

_CODE_INLINE
bool RenILinesDiagnostic::hasTestedLines( void ) const
{
	return hasTestedLines_;
}

/* LINEDIAG.IPP *******************************************/

