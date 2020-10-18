/*
 * L I N E P R J I . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machphys/internal/lineprji.hpp"
#include "sim/manager.hpp"

PER_DEFINE_PERSISTENT(  MachPhysLinearProjectileImpl );

MachPhysLinearProjectileImpl::MachPhysLinearProjectileImpl()
:   flightStartTime_( SimManager::instance().currentTime() ),
    flightDuration_( 0.0 ),
    pPlanetSurface_( NULL ),
	lightsSuppressed_(false),
	launchControlType_(MachPhysMachine::OTHER)
{

    TEST_INVARIANT;
}

MachPhysLinearProjectileImpl::MachPhysLinearProjectileImpl( const MachPhysLinearProjectileImpl& rhs )
:   flightStartTime_( rhs.flightStartTime_ ),
    flightDuration_( rhs.flightDuration_ ),
    flightPath_( rhs.flightPath_ ),
    pPlanetSurface_( rhs.pPlanetSurface_ ),
	lightsSuppressed_(rhs.lightsSuppressed_),
	launchControlType_(MachPhysMachine::OTHER)
{

    TEST_INVARIANT;
}

MachPhysLinearProjectileImpl::~MachPhysLinearProjectileImpl()
{
    TEST_INVARIANT;

}


void MachPhysLinearProjectileImpl::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachPhysLinearProjectileImpl& t )
{

    o << "MachPhysLinearProjectileImpl " << (void*)&t << " start" << std::endl;
    o << "MachPhysLinearProjectileImpl " << (void*)&t << " end" << std::endl;

    return o;
}

MachPhysLinearProjectileImpl::MachPhysLinearProjectileImpl( PerConstructor )
{
}

void perWrite( PerOstream& ostr, const MachPhysLinearProjectileImpl& projectileImpl )
{
    ostr << projectileImpl.flightStartTime_;
    ostr << projectileImpl.flightDuration_;
    ostr << projectileImpl.flightPath_;
}

void perRead( PerIstream& istr, MachPhysLinearProjectileImpl& projectileImpl )
{
    istr >> projectileImpl.flightStartTime_;
    istr >> projectileImpl.flightDuration_;
    istr >> projectileImpl.flightPath_;

	// This is only used for optimization.  Correct persistence is unnecessary.
	projectileImpl.lightsSuppressed_ = false;
}

/* End LINEPRJI.CPP *************************************************/
