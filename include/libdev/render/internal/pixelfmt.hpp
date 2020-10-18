/*
 * P I X E L F M T . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

#ifndef _RENDER_PIXELFMT_HPP
#define _RENDER_PIXELFMT_HPP

#include "base/base.hpp"

// A wrapper for one of the DirectX structures which adds some handy fns.
class RenIPixelFormat //: public DDPIXELFORMAT
{
public:
	// Create an empty, invalid pixel format.
	// POST(!isValid());
	RenIPixelFormat();

	// Determine the pixel format of the given surface.
	// PRE(surf); POST(isValid());

	//explicit RenIPixelFormat(const COMPTR(IDirectDrawSurface2)& surf);

	// Initialise with the given DirectDraw structure.  No statement about
	// validity can be made; it depends on the state of the argument.
	//explicit RenIPixelFormat(const DDPIXELFORMAT&);

	// Copy this to the given DirectX structure.
	//void copyTo(DDPIXELFORMAT*) const;

	size_t totalDepth() const;		// RGBDepth() + alphaDepth()
	size_t RGBDepth  () const;
	size_t redDepth  () const;
	size_t greenDepth() const;
	size_t blueDepth () const;
	size_t alphaDepth() const;

	// The RGB bit count always seems to be set for valid formats.
	//bool isValid() const		{ return dwRGBBitCount > 0; }
	//bool hasAlpha() const		{ return dwFlags & DDPF_ALPHAPIXELS; }
	//bool isRGB() const			{ return dwFlags & DDPF_RGB; }
	//bool isPalette() const		{ return dwFlags & (DDPF_PALETTEINDEXED8 | DDPF_PALETTEINDEXED4); }

	bool operator==(const RenIPixelFormat& pf) const;

private:
	size_t computeBitDepth(unsigned long bitMask) const;
};

//class ostream;
ostream& operator<<(ostream& os, const RenIPixelFormat& fmt);

#endif

/* End PIXELFMT.HPP *************************************************/
