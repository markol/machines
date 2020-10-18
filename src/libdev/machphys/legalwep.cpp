/*
 * L E G A L W E P . C P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machphys/legalwep.hpp"

#include "ctl/vector.hpp"

// static
MachPhysLegalWeapons& MachPhysLegalWeapons::instance()
{
    static MachPhysLegalWeapons instance_;
    return instance_;
}

MachPhysLegalWeapons::MachPhysLegalWeapons()
{

    TEST_INVARIANT;
}

MachPhysLegalWeapons::~MachPhysLegalWeapons()
{
    TEST_INVARIANT;

}

void MachPhysLegalWeapons::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

MachPhysLegalWeapons::Mountings MachPhysLegalWeapons::machineMountings
(
    MachPhys::MachineType type, uint subType, uint hwLevel
)
{
    //Set up vector for returning by value
    Mountings result;
    result.reserve( 2 );

    switch( type )
    {
        case MachPhys::AGGRESSOR:
        {
            switch( subType )
            {
                case MachPhys::GRUNT:
                {
                    switch( hwLevel )
                    {
                        case 1:
                            result.push_back( MachPhys::RIGHT );
                            break;
                        case 2:
                            result.push_back( MachPhys::LEFT );
                            break;
                        case 3:
                            result.push_back( MachPhys::RIGHT );
                            result.push_back( MachPhys::LEFT );
                            break;
                    }
                    break;
                }
            }
            break;
        }
    }

    return result;
}

MachPhysLegalWeapons::Mountings MachPhysLegalWeapons::emplacementMountings
(
    MachPhys::MissileEmplacementSubType, uint
)
{
    //Set up vector for returning by value
    Mountings result;
    result.reserve( 2 );

    return result;
}

MachPhysLegalWeapons::WeaponTypes MachPhysLegalWeapons::machineWeaponTypes
(
    MachPhys::MachineType type, uint subType, uint hwLevel
)
{
    //Set up vector for returning by value
    WeaponTypes result;
    result.reserve( 4 );

    switch( type )
    {
        case MachPhys::AGGRESSOR:
        {
            switch( subType )
            {
                case MachPhys::GRUNT:
                {
                    switch( hwLevel )
                    {
                        case 1:
                            result.push_back( MachPhys::PULSE_RIFLE );
                            break;
                        case 2:
                            result.push_back( MachPhys::PULSE_RIFLE );
                            break;
                        case 3:
                            result.push_back( MachPhys::PULSE_RIFLE );
                            break;
                    }
                    break;
                }
            }
            break;
        }
    }

    return result;
}

MachPhysLegalWeapons::WeaponTypes MachPhysLegalWeapons::emplacementWeaponTypes
(
    MachPhys::MissileEmplacementSubType, uint
)
{
    //Set up vector for returning by value
    WeaponTypes result;
    result.reserve( 4 );
    result.push_back( MachPhys::PULSE_RIFLE );

    return result;
}
/* End LEGALWEP.CPP *************************************************/
