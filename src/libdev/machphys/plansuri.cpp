/*
 * P L A N S U R I . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machphys/internal/plansuri.hpp"
#include "world4d/root.hpp"

PER_DEFINE_PERSISTENT( MachPhysPlanetSurfaceImpl );

MachPhysPlanetSurfaceImpl::MachPhysPlanetSurfaceImpl()
: minMachineHeight_( -10000.0 ),
  pPersistenceRoot_( _NEW( W4dRoot( 133 ) ) )
{

    TEST_INVARIANT;
}

MachPhysPlanetSurfaceImpl::~MachPhysPlanetSurfaceImpl()
{
    TEST_INVARIANT;
    _DELETE( pPersistenceRoot_ );
}

void MachPhysPlanetSurfaceImpl::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachPhysPlanetSurfaceImpl& t )
{

    o << "MachPhysPlanetSurfaceImpl " << (void*)&t << " start" << std::endl;
    o << "MachPhysPlanetSurfaceImpl " << (void*)&t << " end" << std::endl;

    return o;
}

MachPhysPlanetSurfaceImpl::MachPhysPlanetSurfaceImpl( PerConstructor )
: minMachineHeight_( -10000.0 )
{
}

void perWrite( PerOstream& ostr, const MachPhysPlanetSurfaceImpl& t )
{
    ostr << t.minMachineHeight_;
    ostr << t.pPersistenceRoot_;
}

void perRead( PerIstream& istr, MachPhysPlanetSurfaceImpl& t )
{
    istr >> t.minMachineHeight_;
    istr >> t.pPersistenceRoot_;
}

/* End PLANSURI.CPP *************************************************/
