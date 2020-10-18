/*
 * M I S S I L E . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    MachPhysMissile

    Models a missile launched by a multi-launcher weapon
*/

#ifndef _MACHPHYS_MISSILE_HPP
#define _MACHPHYS_MISSILE_HPP

#include "base/base.hpp"
#include "base/persist.hpp"

#include "phys/phys.hpp"
#include "machphys/machphys.hpp"

#include "machphys/trailprj.hpp"

template< class ID, class PART > class MachPhysObjectFactory;
class MachPhysConstructionPersistence;

class MachPhysMissile : public MachPhysTrailedProjectile
{
public:
    //ctor. Attached to pParent at relative position localTransform.
    //level indicates missile level.

	enum CreateLights { CREATE_LIGHTS, DONT_CREATE_LIGHTS };
    MachPhysMissile( W4dEntity* pParent, const W4dTransform3d& localTransform, size_t level, CreateLights=CREATE_LIGHTS );

    //dtor
    virtual ~MachPhysMissile();

    //Start flying at time startTime.
    //Relevant weapon data in data.
    virtual PhysRelativeTime beLaunched(  const PhysAbsoluteTime& startTime,
                                  const MachPhysWeaponData& data,
                                  const MexPoint3d& targetOffsetGlobal);

	static MachPhys::WeaponType weaponType(size_t level);

	MachPhys::WeaponType weaponType() const;

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachPhysMissile& t );

    PER_MEMBER_PERSISTENT( MachPhysMissile );
    PER_FRIEND_READ_WRITE( MachPhysMissile );

private:
    // Operation deliberately revoked
    MachPhysMissile( const MachPhysMissile& );
    MachPhysMissile& operator =( const MachPhysMissile& );
    bool operator ==( const MachPhysMissile& );

    enum    { MAX_LEVELS = 10 };


    //  This is necessary to allow the ti file to instantiate the factory class
    //friend MachPhysMissile& Factory::part( const ID& id, size_t );
	typedef MachPhysObjectFactory< size_t, MachPhysMissile >    Factory;
	//friend class Factory;
	friend class MachPhysObjectFactory< size_t, MachPhysMissile > ;

    //  Necessary to allow the persistence mechanism write out the factory
    friend void perWrite( PerOstream&, const MachPhysConstructionPersistence& );
    friend void perRead( PerIstream&, MachPhysConstructionPersistence& );

    static  MachPhysMissile& part( size_t level );
    static  Factory& factory();

    //  This is the constructor that is used by the factory. It is the
    //  only constructor that actually builds a missile from scratch
    MachPhysMissile( W4dEntity* pParent, size_t level );

    SysPathName compositeFileName( size_t level ) const;

    ////////////////////////////////////////////////////////////
    // Inherited from MachPhysLinearProjectile

    //Override to add specific animations for the projectile destruction at time.
    //Return duration of such animation.
    virtual PhysRelativeTime doBeDestroyedAt( const PhysAbsoluteTime& time,
                                              MachPhys::StrikeType strikeType );

    ////////////////////////////////////////////////////////////
};

PER_DECLARE_PERSISTENT( MachPhysMissile );


#endif

/* End MISSILE.HPP **************************************************/
