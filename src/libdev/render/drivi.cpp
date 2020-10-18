/*
 * D R I V . C P P
 * (c) Charybdis Limited, 1999. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "system/registry.hpp"
#include "ctl/countptr.hpp"
#include "render/internal/drivi.hpp"
#include "render/driv.hpp"

//
// class RenIDriverImpl
//
RenIDriverImpl::RenIDriverImpl( const RenIDriverImpl& copy )
:
  name_( copy.name_ ),
  description_( copy.description_ ),
  hw_( copy.hw_ ),
  isAutomatic_( copy.isAutomatic_ )
{
}

RenIDriverImpl::~RenIDriverImpl()
{
}

bool RenIDriverImpl::hasGuid() const
{
	return false;
}

void RenIDriverImpl::isAutomatic( bool isAuto )
{
	isAutomatic_ = isAuto;
}

bool RenIDriverImpl::isAutomatic() const
{
	return isAutomatic_;
}

const std::string& RenIDriverImpl::name() const
{
	return name_;
}

void RenIDriverImpl::name( const std::string& newName )
{
	name_ = newName;
}

const std::string& RenIDriverImpl::description() const
{
	 return description_;
}

bool RenIDriverImpl::isHardware() const
{
	return hw_;
}

bool RenIDriverImpl::isBetterChoiceThan(  const RenIDDrawDriverImpl& than ) const
{

	std::string assertOut = 	name() + " and " + than.name() ;
	ASSERT_INFO( assertOut );
	ASSERT( false, "Can't compare incompatible drivers" );
	return false;
}

bool RenIDriverImpl::isBetterChoiceThan(  const RenID3dDeviceImpl& than ) const
{

	std::string assertOut = 	name() + " and " + than.name() ;
	ASSERT_INFO( assertOut );
	ASSERT( false, "Can't compare incompatible drivers" );
	return false;
}

// return an invalid value
int RenIDriverImpl::zBufferBitDepth() const
{
	return -1;
}

//
/* RenIDDrawDriverImpl **************************************************/
//

RenIDDrawDriverImpl::RenIDDrawDriverImpl( const RenIDDrawDriverImpl& copy )
: RenIDriverImpl( copy )

{
}

RenIDDrawDriverImpl::~RenIDDrawDriverImpl()
{
}

RenIDriverImpl* RenIDDrawDriverImpl::clone() const
{
	return _NEW( RenIDDrawDriverImpl( *this ) );
}

bool RenIDDrawDriverImpl::isBetterChoiceThan( const RenIDriverImpl& than ) const
{
	return not than.isBetterChoiceThan( *this );
}

bool RenIDDrawDriverImpl::isBetterChoiceThan( const RenIDDrawDriverImpl& than ) const
{
	bool result = true;
	// we prefer the non primary drivers which have guids
    if( not hasGuid() and than.hasGuid() )
		result=false;
	return result;
}

void RenIDDrawDriverImpl::writeToRegistry()
{
	SysRegistry::KeyHandle handle;
	SysRegistry::instance().setIntegerValue( "Options\\Video Drivers\\DirectDraw", "Automatic", isAutomatic(), SysRegistry::CURRENT_USER );
	SysRegistry::instance().setIntegerValue( "Options\\Video Drivers\\DirectDraw", "Primary Driver",  not hasGuid(), SysRegistry::CURRENT_USER );
	if( hasGuid() )
	{
		SysRegistry::instance().closeKey( handle );
	}
}

RenID3dDeviceImpl::RenID3dDeviceImpl( const RenID3dDeviceImpl& copy )
: RenIDriverImpl( copy )
{
}

RenID3dDeviceImpl::~RenID3dDeviceImpl()
{
}

RenIDriverImpl* RenID3dDeviceImpl::clone() const
{
	return _NEW( RenID3dDeviceImpl( *this ) );
}

bool RenID3dDeviceImpl::isBetterChoiceThan( const RenIDriverImpl& than ) const
{
	hasGuid();
	return not than.isBetterChoiceThan( *this );
}

bool RenID3dDeviceImpl::isBetterChoiceThan( const RenID3dDeviceImpl& than ) const
{
	bool result = true;
		result = false;
	return result;
}


void RenID3dDeviceImpl::writeToRegistry()
{
	SysRegistry::KeyHandle handle;
	SysRegistry::instance().setIntegerValue( "Options\\Video Drivers\\Direct3d", "Automatic", isAutomatic(), SysRegistry::CURRENT_USER );
	if( hasGuid() )
	{
		SysRegistry::instance().openKey( "Options\\Video Drivers\\Direct3d", &handle, SysRegistry::CURRENT_USER );
		SysRegistry::instance().closeKey( handle );
	}
}

int RenID3dDeviceImpl::zBufferBitDepth() const
{
	// Prefer smallest first, except for 8-bit which would be pathetic.
	    return 0;
}

/* End DRIV.CPP **************************************************/
