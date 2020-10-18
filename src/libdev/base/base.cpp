/*
 * B A S E . C P P
 * (c) Charybdis Limited, 1996. All Rights Reserved.
 */

#include "base/base.hpp"

//////////////////////////////////////////////////////////////////////

#ifndef NDEBUG

//////////////////////////////////////////////////////////////////////

BaseAssertion::AssertionLock& BaseAssertion::AssertionLock::instance()
{
	static AssertionLock lock;
	return lock;
}

//////////////////////////////////////////////////////////////////////

void BaseAssertion::AssertionLock::lock()
{
	if( isLocked_ )
		BaseAssertion::preconditionFail( "not isLocked()", __FILE__, _STR( __LINE__ ) );

	isLocked_ = true;
}

void BaseAssertion::AssertionLock::unlock()
{
	if( not isLocked_ )
		BaseAssertion::preconditionFail( "isLocked()", __FILE__, _STR( __LINE__ ) );

	isLocked_ = false;
}

void BaseAssertion::AssertionLock::lockInvariant()
{
	if( invariantLocked_ )
		BaseAssertion::preconditionFail( "not invariantLocked()", __FILE__, _STR( __LINE__ ) );

	invariantLocked_ = true;
}

void BaseAssertion::AssertionLock::unlockInvariant()
{
	if( not invariantLocked_ )
		BaseAssertion::preconditionFail( "invariantLocked()", __FILE__, _STR( __LINE__ ) );

	invariantLocked_ = false;
}

bool BaseAssertion::AssertionLock::isLocked() const
{
	return isLocked_;
}

bool BaseAssertion::AssertionLock::invariantLocked() const
{
	return invariantLocked_;
}

BaseAssertion::AssertionLock::AssertionLock()
: isLocked_( false ),
  invariantLocked_( false )
{
	/* Intentionally Empty	*/
}

#endif

//////////////////////////////////////////////////////////////////////

/* End BASE.CPP *****************************************************/
