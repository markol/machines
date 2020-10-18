/*
 * P I X E L F M T . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

#include "render/internal/ren_pch.hpp"		// NB: pre-compiled header must come 1st

#include <iostream>
#include <iomanip>
#include "render/internal/pixelfmt.hpp"

RenIPixelFormat::RenIPixelFormat()
{

}

size_t RenIPixelFormat::computeBitDepth(unsigned long bitMask) const
{
	if (bitMask == 0)
		return 0;

	unsigned long m = bitMask;
    for (int s = 0; !(m & 1); ++s)
		m >>= 1;

	size_t d;
    for (d = 0; m & 1; ++d)
		m >>= 1;

	return d;
}

size_t RenIPixelFormat::redDepth() const
{
//	return computeBitDepth(dwRBitMask);
return 0;
}

size_t RenIPixelFormat::greenDepth() const
{
//	return computeBitDepth(dwGBitMask);
return 0;
}

size_t RenIPixelFormat::blueDepth() const
{
//	return computeBitDepth(dwBBitMask);
return 0;
}

size_t RenIPixelFormat::alphaDepth() const
{
	/*if ((dwFlags & DDPF_ALPHAPIXELS) && dwRGBAlphaBitMask)
		return computeBitDepth(dwRGBAlphaBitMask);
	else*/
		return 0;
}

size_t RenIPixelFormat::RGBDepth() const
{
	// It appears that the RGB bit count in the DirectX structure is a load of
	// bollocks.  The drivers don't fill out the contents with correct values:
	//   when alpha is present the RGB bit count member is actually RGB + alpha;
	// 	 on the Matrox Mystique there's a (5,5,5) format with a bit count of 16!
	// Computing the value from the mask seems fairly reliable, but we cannot
	// always use the masks because they appear to be zero when the structure
	// describes a palette format (which is fair enough).
	/*if (dwRBitMask || dwGBitMask || dwBBitMask)
		return redDepth() + greenDepth() + blueDepth();
	else
		return dwRGBBitCount;*/return 0;
}

size_t RenIPixelFormat::totalDepth() const
{
	return RGBDepth() + alphaDepth();
}

bool RenIPixelFormat::operator==(const RenIPixelFormat& pf) const
{
	PRE_INFO(*this);
	PRE_INFO(pf);

	// It's hard to say if this will actually work in all cases because the
	// naff MicroSoft implementation means that the flags determine which other
	// fields are valid.  We certainly can't do a memcmp because some bits of
	// the struct can presumably be uninitialised.
	/*return 	dwFlags       == pf.dwFlags &&
			dwRGBBitCount == pf.dwRGBBitCount &&
			dwRBitMask    == pf.dwRBitMask &&
			dwGBitMask    == pf.dwGBitMask &&
			dwBBitMask    == pf.dwBBitMask &&
			(hasAlpha() && pf.hasAlpha() || !hasAlpha() && !pf.hasAlpha());*/return 0;
}

ostream& operator<<(ostream& os, const RenIPixelFormat& fmt)
{
	/*if (fmt.hasAlpha())
	{
		os << "RGBA ("
		   << fmt.redDepth()   << ","
		   << fmt.greenDepth() << ","
		   << fmt.blueDepth()  << ","
		   << fmt.alphaDepth() << ") "
		   << std::hex << std::setw(8) << std::setfill('0') << fmt.dwRBitMask << " "
		   << std::hex << std::setw(8) << std::setfill('0') << fmt.dwGBitMask << " "
		   << std::hex << std::setw(8) << std::setfill('0') << fmt.dwBBitMask << " "
		   << std::hex << std::setw(8) << std::setfill('0') << fmt.dwRGBAlphaBitMask;
	}
	else if (fmt.isPalette())
	{
		os << "Palette " << fmt.dwRGBBitCount;
	}
	else
	{
		os << "RGB ("
		   << fmt.redDepth()   << ","
		   << fmt.greenDepth() << ","
		   << fmt.blueDepth()  << ") "
		   << std::hex << std::setw(8) << std::setfill('0') << fmt.dwRBitMask << " "
		   << std::hex << std::setw(8) << std::setfill('0') << fmt.dwGBitMask << " "
		   << std::hex << std::setw(8) << std::setfill('0') << fmt.dwBBitMask;
	}
*/
	os << std::dec;
	return os;
}

/* End PIXELFMT.CPP *************************************************/
