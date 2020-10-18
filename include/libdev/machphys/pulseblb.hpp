/*
 * P U L S E B L B . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachPhysPulseBlob

    Represents a single pulse blob projectile
*/

#ifndef _MACHPHYS_PULSEBLB_HPP
#define _MACHPHYS_PULSEBLB_HPP

#include "base/base.hpp"
#include "base/persist.hpp"
#include "render/render.hpp"

#include "machphys/lineproj.hpp"
#include "machphys/machphys.hpp"

//forward refs
class MachPhysFireball;

//orthodox canonical (revoked)
class MachPhysPulseBlob : public MachPhysLinearProjectile
{
public:
	enum CreateLights { CREATE_LIGHTS, DONT_CREATE_LIGHTS };

    //ctor. Parent is to be pParent, with relative transform localTransform.
    MachPhysPulseBlob(W4dEntity* pParent, const MexTransform3d& localTransform, 
					  MachPhys::WeaponType type, CreateLights=CREATE_LIGHTS);

    //Return an exemplar pulse blob - ensures the pulse meshes and textures are loaded
    static const MachPhysPulseBlob& exemplar(MachPhys::WeaponType type);

    //dtor
    virtual ~MachPhysPulseBlob();

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachPhysPulseBlob& t );

    PER_MEMBER_PERSISTENT( MachPhysPulseBlob );
    PER_FRIEND_READ_WRITE( MachPhysPulseBlob );
    
private:
    // Operations deliberately revoked
    MachPhysPulseBlob( const MachPhysPulseBlob& );
    MachPhysPulseBlob& operator =( const MachPhysPulseBlob& );
    bool operator ==( const MachPhysPulseBlob& );

	friend class MachPhysOtherPersistence;
    //One-time constructor used to create the exemplar
    MachPhysPulseBlob(MachPhys::WeaponType type);
														     
    //////////////////////////////////////////////////////////
    // Inherited from MachPhysLinearProjectile

    //Override to add specific animations for the projectile destruction at time.
    //Return duration of such animation.
    //Default implementation does nothing and returns zero.
    virtual PhysRelativeTime doBeDestroyedAt( const PhysAbsoluteTime& time,
                                              MachPhys::StrikeType strikeType );
    //////////////////////////////////////////////////////////
	friend class MachPhysWeaponPersistence;
    //data members
    MachPhysFireball* pFireball_;
};

PER_DECLARE_PERSISTENT( MachPhysPulseBlob );

#endif

/* End PULSEBLB.HPP *************************************************/
