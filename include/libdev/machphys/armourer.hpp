/*
 * A R M O U R E R . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachPhysArmourer

    A brief description of the class should go in here
*/

#ifndef _MACHPHYS_ARMOURER_HPP
#define _MACHPHYS_ARMOURER_HPP

#include "base/base.hpp"
#include "machphys/machphys.hpp"

//forward refs
class MachPhysWeapon;
class MachPhysCanAttack;
class MachPhysConstruction;
class MachPhysMachine;
class W4dEntity;
class MexTransform3d;

class MachPhysArmourer
// Static methods only - cannot be instantiated
{
public:
    //Constructs and returns a new weapon on the heap of type type (with mounting if relevant),
    //for the client to take ownership of.
    static MachPhysWeapon* newWeapon( MachPhys::WeaponType type, MachPhys::Mounting mounting,
                                      W4dEntity* pParent, const MexTransform3d& localTransform );

    //Constructs and mounts the weapons defined by combo onto pMachine/pAttacker.
    static void fitWeapons( MachPhysMachine* pMachine, MachPhysCanAttack* pAttacker,
                            MachPhys::WeaponCombo combo );
    //PRE( pMachine == pAttacker );

    //Constructs and mounts the weapons defined by combo onto pConstruction/pAttacker.
    static void fitWeapons( MachPhysConstruction* pConstruction, MachPhysCanAttack* pAttacker,
                            MachPhys::WeaponCombo combo );
    //PRE( pConstruction == pAttacker );

private:
    MachPhysArmourer();
    ~MachPhysArmourer();
    MachPhysArmourer( const MachPhysArmourer& );
    MachPhysArmourer& operator =( const MachPhysArmourer& );
    bool operator ==( const MachPhysArmourer& );

    //Constructs and mounts the weapons defined by combo onto pAttacker.
    static void fitWeapons( MachPhysCanAttack* pAttacker, MachPhys::WeaponCombo combo );
    //PRE( pAttacker must have the required mountings available );

};


#endif

/* End ARMOURER.HPP *************************************************/
