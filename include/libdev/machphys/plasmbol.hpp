/*
 * P L A S M B O L . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachPhysPlasmaBolt

    Represents a single plasma bolt
*/

#ifndef _MACHPHYS_PLASMBOL_HPP
#define _MACHPHYS_PLASMBOL_HPP

#include "base/base.hpp"
#include "base/persist.hpp"
#include "machphys/machphys.hpp"
#include "machphys/lineproj.hpp"

//forward refs
class W4dMaterialPlan;
class RenMaterial;

class MachPhysPlasmaBolt : public MachPhysLinearProjectile
// Canonical form revoked
{
public:
	enum CreateLights { CREATE_LIGHTS, DONT_CREATE_LIGHTS };

    //ctor. Parent is to be pParent, with relative transform localTransform.
    //weapon type is indicated by type. (Determines size of bolt)
    MachPhysPlasmaBolt( W4dEntity* pParent, const MexTransform3d& localTransform,
                        MachPhys::WeaponType type, CreateLights=CREATE_LIGHTS );

    //Return an exemplar plasma bolt - ensures the plasma meshes and textures are loaded
    static const MachPhysPlasmaBolt& exemplar( MachPhys::WeaponType type );

    //dtor
    virtual ~MachPhysPlasmaBolt();

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachPhysPlasmaBolt& t );

    PER_MEMBER_PERSISTENT( MachPhysPlasmaBolt );
    PER_FRIEND_READ_WRITE( MachPhysPlasmaBolt );

private:
    //revoked
    MachPhysPlasmaBolt( const MachPhysPlasmaBolt& );
    MachPhysPlasmaBolt& operator =( const MachPhysPlasmaBolt& );
    bool operator ==( const MachPhysPlasmaBolt& );

	friend class MachPhysOtherPersistence;
    //One-time constructor used to create the exemplar
    MachPhysPlasmaBolt( MachPhys::WeaponType type );

    //////////////////////////////////////////////////////////
    // Inherited from MachPhysLinearProjectile

    //Override to add specific animations for the projectile destruction at time.
    //Return duration of such animation.
    virtual PhysRelativeTime doBeDestroyedAt( const PhysAbsoluteTime& time,
                                              MachPhys::StrikeType strikeType );
    //////////////////////////////////////////////////////////

    //Material plan to use for the weapon type
    const W4dMaterialPlanPtr& boltMaterialPlan( MachPhys::WeaponType type ) const;

    //Create a material plan for the bolt of type type
    static W4dMaterialPlan* createBoltMaterialPlan( MachPhys::WeaponType type );

    //the material to use for the central TTF
    static const RenMaterial& centreMaterial();

    //Create an impact flash attached tp pParent at localPosition offset at startTime.
    //Returns duration of the flash.
    PhysRelativeTime createImpactFlash( W4dEntity* pParent,
                     const MexTransform3d& localPosition, const PhysAbsoluteTime& startTime );

    //data members
    MachPhys::WeaponType boltType_; //Firing wepaon type
};

PER_DECLARE_PERSISTENT( MachPhysPlasmaBolt );

#endif

/* End PLASMBOL.HPP *************************************************/
