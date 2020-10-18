/*
 * C O U N T P T R . H P P
 * (c) Charybdis Limited, 1996. All Rights Reserved.
 */

#include "ctl/countptr.hpp"

//////////////////////////////////////////////////////////////////////

CtlCountedObject::CtlCountedObject()
: refs_( 0 ) // ,
  // shareable_( true )
{
	/* Intentionally Empty	*/
}

CtlCountedObject::CtlCountedObject( const CtlCountedObject& )
: refs_( 0 ) // ,
  // shareable_( true )
{
	/* Intentionally Empty	*/
}

CtlCountedObject&
CtlCountedObject::operator =( const CtlCountedObject& )
{
	return *this;
}

CtlCountedObject::~CtlCountedObject()
{
	/* Intentionally Empty	*/
}

void CtlCountedObject::addReference()
{
	++refs_;
}

void CtlCountedObject::removeReference()
{
	if( --refs_ == 0 )
		_DELETE( this );
}

// void CtlCountedObject::markUnshareable()
// {
//	shareable_ = false;
// }

// bool CtlCountedObject::isShareable() const
// {
//	return shareable_;
// }

bool CtlCountedObject::isShared() const
{
	return refs_ > 1;
}

//////////////////////////////////////////////////////////////////////

/* End COUNTPTR.CPP *************************************************/
