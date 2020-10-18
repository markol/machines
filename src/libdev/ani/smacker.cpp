/*
 * S M A C K E R . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved.
 */

#include "ani/smacker.hpp"
#include "ani/internal/smackei.hpp"

#include <stdio.h>

#include "render/device.hpp"
#include "render/display.hpp"
#include "render/surface.hpp"
#include "system/pathname.hpp"

//////////////////////////////////////////////////////////////////////

AniSmacker::AniSmacker( const SysPathName& path, size_t xCoordTo, size_t yCoordTo, bool fast)
:
pImpl_( NULL )
{
	PRE( path.existsAsFile() );

	pImpl_ = _NEW( AniSmackerImpl( path, xCoordTo, yCoordTo, fast ) );
}

AniSmacker::AniSmacker( const SysPathName& path, size_t xCoordTo, size_t yCoordTo )
{
	PRE( path.existsAsFile() );

	pImpl_ = _NEW( AniSmackerImpl( path, xCoordTo, yCoordTo ) );
}

AniSmacker::~AniSmacker()
{
	_DELETE( pImpl_ );
}

//////////////////////////////////////////////////////////////////////

unsigned AniSmacker::height() const
{
	return pImpl_->height_;
}

unsigned AniSmacker::width() const
{
	return pImpl_->width_;
}

void AniSmacker::rewind()
{
	pImpl_->rewind();
}

void AniSmacker::SynchronisationOn( void )
{
	pImpl_->synchronisation_ = true;
}

void AniSmacker::SynchronisationOff( void )
{
	pImpl_->synchronisation_ = false;
}

bool AniSmacker::isFinished() const
{
	return pImpl_->isFinished();
}

void AniSmacker::playNextFrame( RenDevice * pDevice )
{
	pImpl_->playNextFrame( pDevice );
}

void AniSmacker::displaySummaryInfo() const
{
	pImpl_->displaySummaryInfo();
}

void AniSmacker::useFrontBuffer( bool ufb )
{
	pImpl_->useFrontBuffer( ufb );
}

bool AniSmacker::useFrontBuffer() const
{
	return pImpl_->useFrontBuffer();
}

////////////////////////////////////////////////////////////////////
