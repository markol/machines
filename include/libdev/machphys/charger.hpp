/*
 * C H A R G E R . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachPhysSuperCharger

    Models a supercharger healing weapon.
*/

#ifndef _MACHPHYS_CHARGER_HPP
#define _MACHPHYS_CHARGER_HPP

#include "base/base.hpp"
#include "base/persist.hpp"
#include "phys/phys.hpp"
#include "machphys/machphys.hpp"
#include "machphys/weapon.hpp"

//forward refs
class W4dEntity;
class MexTransform3d;
class MachPhysHealHelix;
class W4dPointLight;

class MachPhysSuperCharger : public MachPhysWeapon
// Canonical form revoked
{
public:
    //ctor
    MachPhysSuperCharger( W4dEntity* pParent, const MexTransform3d& localTransform,
                    MachPhys::WeaponType type, MachPhys::Mounting mounting );

    //dtor
    virtual ~MachPhysSuperCharger();

    //Begin the healing animation to heal machine at startTime, continuing for duraion,
    //or until stopHealing() is called.
    void startHealing( const MachPhysMachine& machine, const PhysAbsoluteTime& startTime,
                       const PhysRelativeTime& duration );

    //Cancel any current healing animation
    void stopHealing();

    //Return an exemplar supercharger - ensures the mesh is loaded
    static const MachPhysSuperCharger& exemplar( MachPhys::WeaponType type );

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachPhysSuperCharger& t );

    PER_MEMBER_PERSISTENT( MachPhysSuperCharger );
	PER_FRIEND_READ_WRITE( MachPhysSuperCharger );

private:
    MachPhysSuperCharger( const MachPhysSuperCharger& );
    MachPhysSuperCharger& operator =( const MachPhysSuperCharger& );
    bool operator ==( const MachPhysSuperCharger& );

    //One-time constructor (per type) used to create the exemplars
    MachPhysSuperCharger( MachPhys::WeaponType type );

    //the composite file path for given type
    static const char* compositeFilePath( MachPhys::WeaponType type );

	//the lighting effect
	void lighting(const PhysAbsoluteTime& startTime, const PhysRelativeTime& duration);

    friend class MachPhysWeaponPersistence;

    //data members
    MachPhysHealHelix* pHealHelix_; //Any active healing animation's helix
    PhysAbsoluteTime helixDestructTime_; //Time pHealHelix set for garbage collection
    W4dPointLight* pLight_; //healing genertes a local light
};

PER_DECLARE_PERSISTENT( MachPhysSuperCharger );

#endif

/* End CHARGER.HPP **************************************************/
