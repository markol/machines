/*
 * V O R T B O M I . C P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machlog/internal/vortbomi.hpp"

#include "machphys/machphys.hpp"
#include "machphys/mphydata.hpp"
//#include "machlog/race.hpp"
//#include "machlog/races.hpp"
//#include "machlog/actor.hpp"


MachLogVortexBombImpl::MachLogVortexBombImpl( const MachPhysWeaponData* const pWeaponData )
:	pWeaponData_( pWeaponData ),
	pPhysVortexBomb_( NULL )
{
    TEST_INVARIANT;
}

MachLogVortexBombImpl::~MachLogVortexBombImpl()
{
    TEST_INVARIANT;

}

void MachLogVortexBombImpl::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachLogVortexBombImpl& t )
{

    o << "MachLogVortexBombImpl " << (void*)&t << " start" << std::endl;
    o << "MachLogVortexBombImpl " << (void*)&t << " end" << std::endl;

    return o;
}




/* End VORTBOMI.CPP *************************************************/
