/*
 * A G G R E S S R . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    MachPhysAggressor

    Implement the physical properties of an aggressor
*/

#ifndef _AGGRESSR_HPP
#define _AGGRESSR_HPP

#include "base/base.hpp"
#include "base/persist.hpp"

#include "machphys/machine.hpp"
#include "machphys/machphys.hpp"
#include "machphys/attack.hpp"

class MachPhysAggressorData;
class MachPhysPunchBlast;
class MachPhysMachinePersistence;
template< class ID, class PART > class MachPhysObjectFactory;
template< class SUBTYPE > class MachPhysSubTypeId;

class MachPhysAggressor
:   public MachPhysMachine,
    public MachPhysCanAttack
{
public:
    MachPhysAggressor(
        W4dEntity* pParent,
        const W4dTransform3d& localTransform,
        MachPhys::AggressorSubType subType,
        size_t bodyLevel,
        size_t brainLevel,
        MachPhys::Race race,
        MachPhys::WeaponCombo combo );

    virtual ~MachPhysAggressor();


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

	virtual const MachPhysMachineData& machineData() const;

	const MachPhysAggressorData& data() const;

    MachPhys::AggressorSubType subType() const;

	bool canPunch() const;

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachPhysAggressor& t );

    PER_MEMBER_PERSISTENT( MachPhysAggressor );
    PER_FRIEND_READ_WRITE( MachPhysAggressor );

    typedef MachPhysSubTypeId< MachPhys::AggressorSubType >  Id;

private:
    // Operation deliberately revoked
    MachPhysAggressor( const MachPhysAggressor& );

    // Operation deliberately revoked
    MachPhysAggressor& operator =( const MachPhysAggressor& );

    // Operation deliberately revoked
    bool operator ==( const MachPhysAggressor& );

    typedef MachPhysObjectFactory< Id, MachPhysAggressor >    Factory;

    //  This is necessary to allow the ti file to instantiate the factory class
    //friend MachPhysAggressor& Factory::part( const ID&, size_t );
    //friend class Factory;
    friend class MachPhysObjectFactory< Id, MachPhysAggressor >;

    //  Necessary to allow the persistence mechanism write out the factory
    friend void perWrite( PerOstream&, const MachPhysMachinePersistence& );
    friend void perRead( PerIstream&, MachPhysMachinePersistence& );

    static  MachPhysAggressor& part( MachPhys::AggressorSubType subType, size_t bodyLevel );
    static  Factory& factory();

    void createExplosionData();

    SysPathName compositeFileName( MachPhys::AggressorSubType, size_t bodyLevel ) const;

    //  This is the constructor that is used by the factory. It is the
    //  only constructor that actually builds an administrator from scratch

    MachPhysAggressor( W4dEntity* pParent, Id id );

    MachPhys::AggressorSubType	subType_;
};

PER_DECLARE_PERSISTENT( MachPhysAggressor );

#endif

/* End AGGRESSR.HPP *************************************************/
