/*
 * T E X S E T . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "render/internal/texseti.hpp"
#include "render/texset.hpp"

RenTextureSet::RenTextureSet()
: pImpl_( _NEW( RenTextureSetImpl( ) ) )
{
    TEST_INVARIANT;
	POST( not isLoaded() );
}

RenTextureSet::RenTextureSet( const SysPathName& pathName )
: pImpl_( _NEW( RenTextureSetImpl( pathName ) ) )
{
    TEST_INVARIANT;
	POST( isLoaded() );
}

RenTextureSet::RenTextureSet( const SysPathName& pathName, BaseProgressReporter* pReporter )
: pImpl_( _NEW( RenTextureSetImpl( pathName, pReporter ) ) )
{
    TEST_INVARIANT;
	POST( isLoaded() );
}

RenTextureSet::~RenTextureSet()
{
    TEST_INVARIANT;
	_DELETE( pImpl_ );
}

void RenTextureSet::load(const SysPathName& pathName )
{
	PRE(not isLoaded());
	pImpl_->load( pathName );
}

void RenTextureSet::load(const SysPathName& pathName, BaseProgressReporter* pReporter )
{
	PRE(not isLoaded());
	PRE(pReporter);

	pImpl_->load( pathName, pReporter );
}

bool RenTextureSet::isLoaded() const
{
	return pImpl_->isLoaded_;
}


void RenTextureSet::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const RenTextureSet& t )
{

    o << "RenTextureSet " << (void*)&t << " start" << std::endl;
    o << "RenTextureSet " << (void*)&t << " end" << std::endl;

    return o;
}

/* End TEXSET.CPP ***************************************************/
