/*
 * I O N B E A M I . C P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machlog/internal/ionbeami.hpp"

#include "machphys/machphys.hpp"
#include "machphys/mphydata.hpp"


MachLogIonBeamImpl::MachLogIonBeamImpl( const MachPhysWeaponData* const pWeaponData )
:	pWeaponData_( pWeaponData ),
	pPhysIonBeam_( NULL )
{
    TEST_INVARIANT;
}

MachLogIonBeamImpl::~MachLogIonBeamImpl()
{
    TEST_INVARIANT;

}

void MachLogIonBeamImpl::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachLogIonBeamImpl& t )
{

    o << "MachLogIonBeamImpl " << (void*)&t << " start" << std::endl;
    o << "MachLogIonBeamImpl " << (void*)&t << " end" << std::endl;

    return o;
}




/* End IONBEAMI.CPP *************************************************/
