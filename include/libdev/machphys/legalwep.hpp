/*
 * L E G A L W E P . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachPhysLegalWeapons

    Singleton class acting as repository for information about which machines
    and missile emplacements have what mountings and what legal weapon types
    can be added to them.
*/

#ifndef _MACHPHYS_LEGALWEP_HPP
#define _MACHPHYS_LEGALWEP_HPP

#include "base/base.hpp"
#include "machphys/machphys.hpp"

//forward refs
template< class T > class ctl_vector;

class MachPhysLegalWeapons
// Canonical form revoked
{
public:
    //  Singleton class
    static MachPhysLegalWeapons& instance( void );
    ~MachPhysLegalWeapons( void );

    //Useful typedefs
    typedef ctl_vector< MachPhys::Mounting > Mountings;
    typedef ctl_vector< MachPhys::WeaponType > WeaponTypes;

    //returns the legal mounting positions for given machine type, sub-type and hardware level
    Mountings machineMountings( MachPhys::MachineType type, uint subType, uint hwLevel );

    //returns the legal mounting positions for given sub-type and hardware level of
    //missile emplacement.
    Mountings emplacementMountings( MachPhys::MissileEmplacementSubType subType,
                                           uint hwLevel );

    //the legal weapon types that can be mounted on a machine of given type, sub-type
    //and hardware level
    WeaponTypes machineWeaponTypes( MachPhys::MachineType type, uint subType, uint hwLevel );

    //the legal weapon types that can be mounted on a missile emplacmenet of given sub-type
    //and hardware level
    WeaponTypes emplacementWeaponTypes( MachPhys::MissileEmplacementSubType subType,
                                    uint hwLevel );

    void CLASS_INVARIANT;

private:
    MachPhysLegalWeapons( const MachPhysLegalWeapons& );
    MachPhysLegalWeapons& operator =( const MachPhysLegalWeapons& );
    bool operator ==( const MachPhysLegalWeapons& );

    MachPhysLegalWeapons( void );
};


#endif

/* End LEGALWEP.HPP *************************************************/
