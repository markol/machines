/*
 * C A P A B L E . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

#ifndef _RENDER_CAPABLE_HPP
#define _RENDER_CAPABLE_HPP

#include "base/base.hpp"

class RenICapabilities;
class SysPathName;
class RenDevice;

// A fairly dumb class which describes what rendering capabilities a
// particular driver has.
class RenCapabilities
{
public:
    ~RenCapabilities();

	// Is the driver hardware assisted or software only?
	bool hardware() const;

	// Does the driver support these various nice features?
	bool supportsFlatAlpha() const;
	bool supportsTextureAlpha() const;
	bool supportsColourKey() const;
	bool supportsStippledAlpha() const;
	bool supportsFog() const;
	bool supportsMMX() const;
	bool supportsBilinear() const;
	bool supportsGammaCorrection()	const;
	bool supports8BitsTexture()	const;

	// The driver supports anti-aliasing.  More specifically, this returns true
	// only if the type of anti-aliasing does not impose contraints on the
	// rendering (some of them require the polygons rendered in order): modes having
	// a higher memory requriement than the highest allowed mode can be ranked below
	// the highest allowed mode (for instance a 16 bit depth mode below a 8 bits mode)
	bool supportsEdgeAntiAliasing() const;

	// Memory required by the 4 Mbytes texture set if it is supported,
	// by the 2 Mbytes texture set if not
	ulong memoryRequiredByTextureSet() const;
	// yes if we have more texture memory than required by the 4Mbytes texture set
	bool supports4MBytesTextureSet() const;
	// maximum texture memory available (account for system memory)
	ulong maxAvailableTextureMemory() const;
	// total display memory available once the texture set has been loaded
	ulong maxAvailableDisplayMemoryAfterTextures() const;

	// The internal capabilities (which provide a super-set of the above capabilities).
	const RenICapabilities* internal() const;
	RenICapabilities* internal();

    friend ostream& operator <<( ostream& o, const RenCapabilities& t );

private:
	friend class RenDevice;
    RenCapabilities(const RenDevice* dev, bool hardwareDriver);		// PRE(dev);

	void parseCardSpecificationFile( const SysPathName& );
	void updateMaxAvailableTextureMemory();
	void updateMaxAvailableDisplayMemoryAfterTextures();

	// These capabilities are cached to avoid repeatedly querying Direct3D.
	RenICapabilities* pImpl_;

    // Operations deliberately revoked
    RenCapabilities( const RenCapabilities& );
    RenCapabilities& operator =( const RenCapabilities& );
    bool operator ==( const RenCapabilities& );
};

#endif

/* End CAPABLE.HPP **************************************************/
