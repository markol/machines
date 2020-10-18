/*
 * D E V I D I . H P P
 * (c) Charybdis Limited, 1999. All Rights Reserved
 */

#ifndef _RENDER_DEVIDENT_HPP
#define _RENDER_DEVIDENT_HPP

#include "base/base.hpp"

// Identifies which specific 3D graphics card is being used by DirectDraw.
class RenIDeviceIdentifier
{
public:
    ~RenIDeviceIdentifier( void );

	typedef enum {
		VOODOO_GRAPHICS,
		VOODOO2,
		VOODOO_BANSHEE,
		VOODOO_RUSH_ALLIANCE_AT25_AT3D_CHIP,
		VOODOO_RUSH_MACRONIX_CHIP,
		STB_VELOCITY_128,
		UNKNOWN
	} DeviceType;

	DeviceType getDeviceType() const;

    void CLASS_INVARIANT;

private:
    friend ostream& operator <<( ostream& o, const RenIDeviceIdentifier& t );

    RenIDeviceIdentifier( const RenIDeviceIdentifier& );
    RenIDeviceIdentifier& operator =( const RenIDeviceIdentifier& );

	DeviceType deviceType_;
};

#endif

/* End DEVIDI.HPP *************************************************/
