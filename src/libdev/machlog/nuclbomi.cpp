/*
 * N U C L B O M I . C P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machlog/internal/nuclbomi.hpp"

#include "machphys/machphys.hpp"
#include "machphys/mphydata.hpp"


MachLogNuclearBombImpl::MachLogNuclearBombImpl( const MachPhysWeaponData* const pWeaponData )
:	pWeaponData_( pWeaponData ),
	pPhysNuclearBomb_( NULL )
{
    TEST_INVARIANT;
}

MachLogNuclearBombImpl::~MachLogNuclearBombImpl()
{
    TEST_INVARIANT;

}

void MachLogNuclearBombImpl::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachLogNuclearBombImpl& t )
{

    o << "MachLogNuclearBombImpl " << (void*)&t << " start" << std::endl;
    o << "MachLogNuclearBombImpl " << (void*)&t << " end" << std::endl;

    return o;
}




/* End NuclearBombI.CPP *************************************************/
