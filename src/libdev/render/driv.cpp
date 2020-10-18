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
// class RenDriver
//

RenDriver::RenDriver( const RenDriver& copy )
: pImpl_( copy.pImpl_->clone() )
{
  TEST_INVARIANT;
}

RenDriver::RenDriver( RenIDriverImpl *pImpl )
{
  PRE( pImpl );
  pImpl_ = pImpl;

  TEST_INVARIANT;
}

RenDriver::~RenDriver()
{
	_DELETE( pImpl_ );
}

RenIDriverImpl&	RenDriver::driverImpl()
{
	PRE( pImpl_ );
	return *pImpl_;
}

const RenIDriverImpl& RenDriver::driverImpl() const
{
	PRE( pImpl_ );
	return *pImpl_;
}

const std::string& RenDriver::name() const
{
	return pImpl_->name_;
}

const std::string& RenDriver::description() const
{
	return pImpl_->description_;
}

bool RenDriver::isBetterChoiceThan( const RenDriver& driver ) const
{
	return driverImpl().isBetterChoiceThan( driver.driverImpl() );
}

void RenDriver::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const RenDriver& t )
{
	o << t.name() << ", " << t.description();
    return o;
}

/* End DRIV.CPP **************************************************/
