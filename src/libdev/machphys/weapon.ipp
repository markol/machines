/*
 * W E A P O N . I P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of inline non-template methods and inline global functions

#ifdef _INLINE
    #define _CODE_INLINE    inline
#else
    #define _CODE_INLINE
#endif

_CODE_INLINE
MachPhys::WeaponType MachPhysWeapon::type() const
{
    return type_;
}

_CODE_INLINE
MachPhys::Mounting MachPhysWeapon::mounting() const
{
    return mounting_;
}

_CODE_INLINE
uint MachPhysWeapon::nextLaunchOffset() const
{
    return nextLaunchOffset_;
}

_CODE_INLINE
void MachPhysWeapon::machine( MachPhysMachine* pMachine )
{
    PRE( not hasConstruction () );
    pMachine_ = pMachine;
    convertMaterials();
}

_CODE_INLINE
bool MachPhysWeapon::hasMachine() const
{
    return pMachine_ != NULL;
}

_CODE_INLINE
MachPhysMachine& MachPhysWeapon::machine() const
{
    PRE( hasMachine() );
    return *pMachine_;
}

_CODE_INLINE
void MachPhysWeapon::construction( MachPhysConstruction* pConstruction )
{
    PRE( not hasMachine() );
    pConstruction_ = pConstruction;
    convertMaterials();
}

_CODE_INLINE
bool MachPhysWeapon::hasConstruction() const
{
    return pConstruction_ != NULL;
}

_CODE_INLINE
MachPhysConstruction& MachPhysWeapon::construction() const
{
    PRE( hasConstruction() );
    return *pConstruction_;
}
/* End WEAPON.IPP ***************************************************/
