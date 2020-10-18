/*
 * P L A N E T I . C P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include <string>
#include "ctl/vector.hpp"
#include "machlog/internal/planeti.hpp"
#include "world4d/root.hpp"

MachLogPlanetImpl::MachLogPlanetImpl()
:   pSurface_( NULL ),
    pWorld_( NULL ),
    pConfigSpace_( NULL ),
    pPressurePads_( NULL ),
	pHiddenRootDomain_( NULL ),
	pHiddenConfigSpace_( NULL ),
	pHiddenRoot_( _NEW( W4dRoot( 100243 ) ) ),
	idsGenerated_( 0 )
{

    TEST_INVARIANT;
}

MachLogPlanetImpl::~MachLogPlanetImpl()
{
    TEST_INVARIANT;

}

void MachLogPlanetImpl::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachLogPlanetImpl& t )
{

    o << "MachLogPlanetImpl " << (void*)&t << " start" << std::endl;
    o << "MachLogPlanetImpl " << (void*)&t << " end" << std::endl;

    return o;
}

/* End PLANETI.CPP **************************************************/
