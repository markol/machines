/*
 * M I S S E M P . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    MachPhysMissileEmplacement

    A brief description of the class should go in here
*/

#ifndef _MACHPHYS_MISSEMP_HPP
#define _MACHPHYS_MISSEMP_HPP

#include "base/base.hpp"
#include "base/persist.hpp"

#include "machphys/constron.hpp"
#include "machphys/attack.hpp"


class MachPhysMissileEmplacementData;
class MachPhysConstructionPersistence;
class MachPhysTurnerTracker;
class MexRadians;

template< class ID, class PART > class MachPhysObjectFactory;
template< class SUBTYPE > class MachPhysSubTypeId;

class MachPhysMissileEmplacement
:	public MachPhysConstruction,
    public MachPhysCanAttack
{
public:
    MachPhysMissileEmplacement(
        W4dEntity* pParent,
        const W4dTransform3d& localTransform,
        MachPhys::MissileEmplacementSubType subType,
        size_t level,
        MachPhys::Race race,
        MachPhys::WeaponCombo combo );

    virtual ~MachPhysMissileEmplacement( void );

    ///////////////////////////////////////////////////////
    //Inherited from MachPhysCanAttack

    //Makes the underlying physical model available
    virtual W4dComposite& asComposite();
    virtual const W4dComposite& asComposite() const;

    //True iff the attacker can turn its weapon carrier to track a target
    virtual bool canTrackWeaponBase() const;

    //Override to cause the weapon base to turn in order to track targetObject.
    //Default implementation does nothing.
    virtual void doWeaponBaseTrackTarget( const W4dEntity& targetObject );

    //Override to stop the weapon base tracking any target.
    //Default implementation does nothing.
    virtual void doStopWeaponBaseTrackingTarget();

    ///////////////////////////////////////////////////////

	//return MachPhysData object for this building
	virtual const MachPhysConstructionData& constructionData() const;
	const MachPhysMissileEmplacementData& data() const;

    MachPhys::MissileEmplacementSubType subType( void ) const;

    void CLASS_INVARIANT;

    //Turn through angle radians (ie, relative or absolute )
    PhysRelativeTime turn( const MexRadians& angle );

    //Turn through angle radians (absolute )
    PhysRelativeTime turnTo( const MexRadians& angle );

    //Global transform of the turn link - indicates direction weapons facing
	const W4dTransform3d& globalAspectTransform() const;

    //Turn as required to face in the direction of targetObject.
    //Supercedes prior turn() and is cancelled by subsequent turn().
    //Client must ensure this is cancelled immediately if targetObject is deleted.
    void trackTarget( const W4dEntity& targetObject );

    //Stop tracking any current target object.
    void stopTracking();

	enum AnimKey : unsigned char {DOME_OPEN = 1, DOME_CLOSE};

	PhysRelativeTime prepForLaunch(const PhysAbsoluteTime& startTime, AnimKey key);

	virtual void damageLevel( const double& percent );

    PER_MEMBER_PERSISTENT( MachPhysMissileEmplacement );
    PER_FRIEND_READ_WRITE( MachPhysMissileEmplacement );

    typedef MachPhysSubTypeId< MachPhys::MissileEmplacementSubType > Id;

private:
    // Operation deliberately revoked
    MachPhysMissileEmplacement( const MachPhysMissileEmplacement& );

    // Operation deliberately revoked
    MachPhysMissileEmplacement& operator =( const MachPhysMissileEmplacement& );

    // Operation deliberately revoked
    bool operator ==( const MachPhysMissileEmplacement& );

    typedef MachPhysObjectFactory< Id, MachPhysMissileEmplacement >    Factory;

    //  This is necessary to allow the ti file to instantiate the factory class
    //friend MachPhysMissileEmplacement& Factory::part( const ID&, size_t );
    //friend class Factory;
    friend class MachPhysObjectFactory< Id, MachPhysMissileEmplacement >;

    //  Necessary to allow the persistence mechanism write out the factory
    friend void perWrite( PerOstream&, const MachPhysConstructionPersistence& );
    friend void perRead( PerIstream&, MachPhysConstructionPersistence& );

    static  MachPhysMissileEmplacement& part( MachPhys::MissileEmplacementSubType subType, size_t level );
    static  Factory& factory();

    SysPathName compositeFileName( MachPhys::MissileEmplacementSubType subType, size_t level ) const;
    SysPathName wireframeFileName( MachPhys::MissileEmplacementSubType subType, size_t level ) const;

    //  This is the constructor that is used by the factory. It is the
    //  only constructor that actually builds a missile emplacement from scratch

    MachPhysMissileEmplacement( W4dEntity* pParent, Id id );

    //  Handle the data initialisation when a construction is read in from
    //  a persistence file
    void persistenceInitialiseData();

    MachPhysMissileEmplacementData* pData_;
    MachPhys::MissileEmplacementSubType subType_;

	W4dLink*    pTurnLink_;
    MachPhysTurnerTracker* pTurnerTracker_;
};

PER_DECLARE_PERSISTENT( MachPhysMissileEmplacement );
PER_ENUM_PERSISTENT( MachPhysMissileEmplacement::AnimKey );

#endif

/* End MISSEMP.HPP *****************************************************/
