/*
 * D E V I D I . C P P
 * (c) Charybdis Limited, 1999. All Rights Reserved
 */

#include "render/internal/ren_pch.hpp"	// NB: pre-compiled header must come 1st
#include "render/internal/devidi.hpp"

class RenIDeviceTag : public DDDEVICEIDENTIFIER
{
public:
	bool isVoodooGraphics() const;
	bool isVoodoo2() const;
	bool isVoodooBanshee() const;
	bool isVoodooRushAllianceAT25AT3DChip() const;
	bool isVoodooRushMacronixChip() const;
	bool isSTBVelocity128() const;

    friend ostream& operator <<( ostream& o, const RenIDeviceTag& t );
};

bool RenIDeviceTag::isVoodooGraphics() const
{
	return (dwVendorId == 0x121A) && (dwDeviceId ==0x0001);
}

bool RenIDeviceTag::isVoodoo2() const
{
	return (dwVendorId == 0x121A) && (dwDeviceId ==0x0002);
}

bool RenIDeviceTag::isVoodooBanshee() const
{
	return (dwVendorId == 0x121A) && (dwDeviceId ==0x0003);
}

bool RenIDeviceTag::isVoodooRushAllianceAT25AT3DChip() const
{
	return (dwVendorId == 0x1142) && (dwDeviceId ==0x643D);
}

bool RenIDeviceTag::isVoodooRushMacronixChip() const
{
	return (dwVendorId == 0x10D9) && (dwDeviceId ==0x8626);
}

bool RenIDeviceTag::isSTBVelocity128() const
{
	return (dwVendorId == 0x12D2) && (dwDeviceId ==0x0018);
}

ostream& operator <<( ostream& o, const RenIDeviceTag& t )
{
	o << std::endl;
    o << "dwVendorId " << t.dwVendorId << std::endl;
    o << "dwDeviceId " << t.dwDeviceId << std::endl;
    o << "dwSubSysId " << t.dwSubSysId << std::endl;
    o << "dwRevision " << t.dwRevision << std::endl;

    return o;
}

RenIDeviceIdentifier::RenIDeviceIdentifier(const COMPTR(IDirectDraw)& dd)
{
	PRE(dd);
	deviceType_ = getDeviceType(dd);
    TEST_INVARIANT;
}

//static
RenIDeviceIdentifier::DeviceType RenIDeviceIdentifier::getDeviceType(const COMPTR(IDirectDraw)& dd)
{
	PRE(dd);

	//COMPTR(IDirectDraw4) dd4 = dd;			// implicit query interface
	COMPTR(IDirectDraw4) dd4 ( dd );
	RenIDeviceTag tag;
	DWORD flags = 0;
	TRYRENDX( dd4->GetDeviceIdentifier( (LPDDDEVICEIDENTIFIER) &tag, flags ) );
	RENDER_STREAM("Device Identity card:" << tag <<  std::endl);

	if( tag.isVoodooGraphics() )
		return VOODOO_GRAPHICS;

	if( tag.isVoodoo2() )
		return VOODOO2;

	if( tag.isVoodooBanshee() )
		return VOODOO_BANSHEE;

	if( tag.isVoodooRushAllianceAT25AT3DChip() )
		return VOODOO_RUSH_ALLIANCE_AT25_AT3D_CHIP;

	if( tag.isVoodooRushMacronixChip() )
		return VOODOO_RUSH_MACRONIX_CHIP;

	if( tag.isSTBVelocity128() )
		return STB_VELOCITY_128;

	return UNKNOWN;
}


RenIDeviceIdentifier::DeviceType RenIDeviceIdentifier::getDeviceType() const
{
	return deviceType_;
}

RenIDeviceIdentifier::~RenIDeviceIdentifier()
{
    TEST_INVARIANT;

}

void RenIDeviceIdentifier::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const RenIDeviceIdentifier& t )
{

    o << "RenIDeviceIdentifier " << (void*)&t << " start" << std::endl;
    o << "RenIDeviceIdentifier " << (void*)&t << " end" << std::endl;

    return o;
}

/* End DEVIDI.CPP *************************************************/
