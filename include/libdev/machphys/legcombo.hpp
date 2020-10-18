/*
 * L E G C O M B O . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachPhysLegalWeaponCombos

    A brief description of the class should go in here
*/

#ifndef _MACHPHYS_LEGCOMBO_HPP
#define _MACHPHYS_LEGCOMBO_HPP

#include "base/base.hpp"
#include "ctl/vector.hpp"
#include "machphys/machphys.hpp"

class MachPhysLegalWeaponCombos
// Canonical form revoked
{
public:
    //  Singleton class
    static MachPhysLegalWeaponCombos& instance();
    ~MachPhysLegalWeaponCombos();

	typedef ctl_vector<MachPhys::WeaponCombo> LegalWeaponCombos;

    LegalWeaponCombos  legalWeaponCombos( MachPhys::MachineType type, const size_t& hwLevel ) const;
    LegalWeaponCombos  legalWeaponCombos( MachPhys::AdministratorSubType subType, const size_t& hwLevel ) const;
    LegalWeaponCombos  legalWeaponCombos( MachPhys::AggressorSubType subType, const size_t& hwLevel ) const;
    LegalWeaponCombos  legalWeaponCombos( MachPhys::ConstructorSubType subType, const size_t& hwLevel ) const;
    LegalWeaponCombos  legalWeaponCombos( MachPhys::TechnicianSubType subType, const size_t& hwLevel ) const;

    LegalWeaponCombos  legalWeaponCombos( MachPhys::ConstructionType type, const size_t& hwLevel ) const;
    LegalWeaponCombos  legalWeaponCombos( MachPhys::FactorySubType subType, const size_t& hwLevel ) const;
    LegalWeaponCombos  legalWeaponCombos( MachPhys::HardwareLabSubType subType, const size_t& hwLevel ) const;
    LegalWeaponCombos  legalWeaponCombos( MachPhys::MissileEmplacementSubType subType, const size_t& hwLevel ) const;

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachPhysLegalWeaponCombos& t );

private:
    MachPhysLegalWeaponCombos( const MachPhysLegalWeaponCombos& );
    MachPhysLegalWeaponCombos& operator =( const MachPhysLegalWeaponCombos& );
    bool operator ==( const MachPhysLegalWeaponCombos& );

    MachPhysLegalWeaponCombos();
};


#endif

/* End LEGCOMBO.HPP ****************************************************/
