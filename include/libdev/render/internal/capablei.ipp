/*
 * C A P A B L E I . I P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

#ifdef _INLINE
    #define _CODE_INLINE    inline
#else
    #define _CODE_INLINE
#endif

_CODE_INLINE
bool RenICapabilities::hardware() const
{
	return hardware_;
}	

_CODE_INLINE
bool RenICapabilities::supportsBilinear() const
{
	return supportsBilinear_;
}	

_CODE_INLINE
bool RenICapabilities::supportsFlatAlpha() const
{
	return supportsFlatAlpha_;
}

_CODE_INLINE
bool RenICapabilities::supportsTextureAlpha() const
{
	return supportsTextureAlpha_;
}

_CODE_INLINE
bool RenICapabilities::supportsStippledAlpha() const
{
	return supportsStippledAlpha_;
}

_CODE_INLINE
bool RenICapabilities::supportsColourKey() const
{
	return supportsColourKey_;
}

_CODE_INLINE
bool RenICapabilities::supportsTableFog() const
{
	return supportsTableFog_;
}

_CODE_INLINE
bool RenICapabilities::supportsVertexFog() const
{
	return supportsVertexFog_;
}

_CODE_INLINE
bool RenICapabilities::supportsFog() const
{
	return supportsTableFog_ || supportsVertexFog_;
}	

_CODE_INLINE
bool RenICapabilities::supportsEdgeAntiAliasing() const
{
	return supportsEdgeAntiAliasing_;
}	

_CODE_INLINE
bool RenICapabilities::supportsGammaCorrection() const
{
	return supportsGammaCorrection_;
}	

_CODE_INLINE
bool RenICapabilities::supportsMMX() const
{
	return supportsMMX_;
}	

_CODE_INLINE
bool RenICapabilities::supports8BitsTexture() const
{
	return supports8BitsTexture_;
}	

_CODE_INLINE
bool RenICapabilities::supportsZBias() const
{
	return supportsZBias_;
}	

_CODE_INLINE
short RenICapabilities::minZBias() const
{
	return 0;
}

_CODE_INLINE
short RenICapabilities::maxZBias() const
{
	return 16;
}

_CODE_INLINE
bool RenICapabilities::supportsSharedVideoMemory() const
{
	return supportsSharedVideoMemory_;
}	

_CODE_INLINE
bool RenICapabilities::supportsTextureSysMemory() const
{
	return supportsTextureSysMemory_;
}	

_CODE_INLINE
ulong RenICapabilities::totalVideoMemory() const
{
	return totalVideoMemory_;
}	

_CODE_INLINE
ulong RenICapabilities::totalTextureMemory() const
{
	return totalTextureMemory_;
}	

/* End CAPABLEI.IPP *************************************************/
