/*
 * C A P A B L E I . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    RenICapabilities

	Driver and hardware capabilities of interest to internal clients.
	RenCapabilites exports a sub-set of these for public clients.
*/

#ifndef _RENDER_CAPABLEI_HPP
#define _RENDER_CAPABLEI_HPP

#include "base/base.hpp"

class RenDevice;

class RenICapabilities
{
public:
	// Is the driver hardware assisted or software only?
	bool hardware() const;

	// Does the driver support these various nice features?
	bool supportsBilinear() const;
	bool supportsFlatAlpha() const;
	bool supportsTextureAlpha() const;
	bool supportsColourKey() const;
	bool supportsStippledAlpha() const;
	bool supportsTableFog() const;
	bool supportsVertexFog() const;
	bool supportsFog() const;			// the logical OR of the above two fns.
	bool supportsMMX() const;
	bool supportsZBias() const;
	bool supports8BitsTexture() const;
	bool supportsSharedVideoMemory() const;
	bool supportsTextureSysMemory() const;

	ulong totalVideoMemory() const;
	ulong totalTextureMemory() const;
	ulong memoryRequiredBy4MBytesTextureSet() const;
	void setTotalTextureAndVideoMemory(const RenDevice* dev);

	bool supportsGammaCorrection() const;
	void setSupportsGammaCorrection();

	// The driver supports anti-aliasing.  More specifically, this returns true
	// only if the type of anti-aliasing does not impose contraints on the
	// rendering (some of them require the polygons rendered in order).
	bool supportsEdgeAntiAliasing() const;

	// These values are derived from the Direct3D reference rather than any
	// Direct3D capability entries.
	short minZBias() const;
	short maxZBias() const;

    ostream& write(ostream& o);

private:
	friend class RenCapabilities;
	RenICapabilities(const RenDevice* dev, bool hardwareDriver);		// PRE(dev);
    ~RenICapabilities();

    friend ostream& operator <<( ostream& o, const RenICapabilities& t );

    RenICapabilities( const RenICapabilities& );
    RenICapabilities& operator =( const RenICapabilities& );

	// maxAvailableTextureMemory_ accounts for the minDisplayMemory_ in case the video
	// memory is shared
	bool maxAvailableTextureMemoryDirty_;
	ulong maxAvailableTextureMemory_;
	// maxAvailableDisplayMemoryAfterTextures_ accounts for the memory which is to be used by
	// the texture set in case the video memory is shared
	bool maxAvailableDisplayMemoryAfterTexturesDirty_;
	ulong maxAvailableDisplayMemoryAfterTextures_;

	bool hardware_, supportsMMX_, supportsBilinear_, supportsEdgeAntiAliasing_;
	bool supportsFlatAlpha_, supportsStippledAlpha_, supportsTextureAlpha_;
	bool supportsTableFog_, supportsVertexFog_, supportsColourKey_;
	bool supportsZBias_, supports8BitsTexture_;
	bool supportsSharedVideoMemory_, supportsTextureSysMemory_;
	bool supportsGammaCorrection_;
	ulong totalVideoMemory_, totalTextureMemory_;
	ulong minDisplayMemory_;
};

#ifdef _INLINE
    #include "internal/capablei.ipp"
#endif

#endif

/* End CAPABLEI.HPP *************************************************/
