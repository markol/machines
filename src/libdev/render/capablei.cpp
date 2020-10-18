/*
 * C A P A B L E . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

#include "render/internal/ren_pch.hpp"	// NB: pre-compiled header must come 1st

#include "render/internal/pixelfmt.hpp"
#include "render/internal/capablei.hpp"
#include "render/device.hpp"
#include "render/internal/devicei.hpp"
#include <iostream>

#ifndef _INLINE
    #include "render/internal/capablei.ipp"
#endif

// Assembler which uses the CPUID instruction to retrieve the MMX
// capabilities (amoung others).
void processorFeatures( uint32* );

RenICapabilities::RenICapabilities(const RenDevice* dev, bool h):
	hardware_(h),
	maxAvailableTextureMemory_(0),
	maxAvailableTextureMemoryDirty_(true),
	maxAvailableDisplayMemoryAfterTextures_(0),
	maxAvailableDisplayMemoryAfterTexturesDirty_(true),
	supportsFlatAlpha_(false),
	supportsStippledAlpha_(false),
	supportsTextureAlpha_(false),
	supportsTableFog_(false),
	supportsVertexFog_(false),
	supportsColourKey_(false),
	supportsBilinear_(false),
	supportsEdgeAntiAliasing_(false),
	supportsMMX_(false),
	supportsZBias_(false),
	supportsGammaCorrection_(false),
	supportsSharedVideoMemory_(false),
	supportsTextureSysMemory_(false),
	supports8BitsTexture_(false)
{
	PRE(dev);

	maxAvailableTextureMemory_ = 256000000;
	supportsFlatAlpha_ = true;
	supportsStippledAlpha_ = true;
	supportsTextureAlpha_ = true;
	supportsTableFog_ = true;
	supportsVertexFog_ = true;
	supportsColourKey_ = true;
	supportsBilinear_ = true;
	supportsEdgeAntiAliasing_ = true;
	supportsMMX_ = false;
	supportsZBias_ = true;
	supportsGammaCorrection_ = true;
	supportsSharedVideoMemory_ = true;
	supportsTextureSysMemory_ = true;
	minDisplayMemory_ = 3 * dev->display()->lowestAllowedMode().memoryRequired();
	setTotalTextureAndVideoMemory( dev );
}

RenICapabilities::~RenICapabilities()
{
}


void RenICapabilities::setTotalTextureAndVideoMemory( const RenDevice* dev )
{
	PRE( dev );
	totalVideoMemory_ = 256000000;
	totalTextureMemory_ = 256000000;
}

inline void supportMessage(ostream& o, bool supported, const char* feature)
{
	o << "  " << ((supported)? "Yes: ": "No:  ") << feature << std::endl;
}

ostream& RenICapabilities::write(ostream& o)
{
	o << ((hardware_)? "hardware": "software") << " driver supporting:" << std::endl;
	supportMessage(o, supportsFlatAlpha_,        "flat alpha");
	supportMessage(o, supportsStippledAlpha_,    "stippled alpha");
	supportMessage(o, supportsTextureAlpha_,     "texture alpha");
	supportMessage(o, supportsColourKey_,        "colour-keying");
	supportMessage(o, supportsTableFog_,         "table fog");
	supportMessage(o, supportsVertexFog_,        "vertex fog");
	supportMessage(o, supportsBilinear_,         "bilinear texture filtering");
	supportMessage(o, supportsMMX(),		     "MMX acceleration");
	supportMessage(o, supportsEdgeAntiAliasing_, "edge anti-aliasing");
	supportMessage(o, supportsGammaCorrection_, "gamma correction");
	supportMessage(o, supports8BitsTexture_, 	 "supports 8 bits texture");
	supportMessage(o, supportsSharedVideoMemory_, "shared video memory");
	supportMessage(o, supportsTextureSysMemory_, "can load textures in system memory");
	o << "  total video memory: " << totalVideoMemory_<< std::endl;
	o << "  total texture memory: "  << totalTextureMemory_ << ( (supportsTextureSysMemory_) ? " including system memory" : "") << std::endl;
	o << "  minimum display memory: "  << minDisplayMemory_ <<  std::endl;

	o << std::endl;
    return o;
}

ulong RenICapabilities::memoryRequiredBy4MBytesTextureSet() const
{
	ulong result;
	if( supports8BitsTexture_ )
		result = 0x400000;
	else
		result = 2 * 0x400000;
	return result;
}

void RenICapabilities::setSupportsGammaCorrection()
{

}

/* End CAPABLE.CPP **************************************************/
