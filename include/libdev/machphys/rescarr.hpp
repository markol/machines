/*
 * R E S C A R R . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    MachPhysResourceCarrier

    Implement the physical properties of a resource carrier
*/

#ifndef _MACHPHYS_RESCARR_HPP
#define _MACHPHYS_RESCARR_HPP

#include "base/base.hpp"
#include "base/persist.hpp"
#include "phys/phys.hpp"
#include "machphys/machine.hpp"
#include "machphys/machphys.hpp"

class MachPhysResourceCarrierData;
class MachPhysMachinePersistence;
template< class ID, class PART > class MachPhysObjectFactory;

class MachPhysResourceCarrier : public MachPhysMachine
{
public:
//     MachPhysResourceCarrier(
//         W4dEntity* pParent,
//         const W4dTransform3d& localTransform,
//         const SysPathName& compositeFileName );

    MachPhysResourceCarrier(
        W4dEntity* pParent,
        const W4dTransform3d& localTransform,
        size_t bodyLevel,
        size_t brainLevel,
        MachPhys::Race race );

//     MachPhysResourceCarrier(
//         W4dEntity* pParent,
//         const MexTransform3d& transform,
//         const MachPhysResourceCarrier& copyMe,
//         const W4dLink& faceplate );

    virtual ~MachPhysResourceCarrier();

	virtual const MachPhysMachineData& machineData() const;

	const MachPhysResourceCarrierData& data() const;

	//perform the loading animation from startTime, return the duration
	const PhysRelativeTime doLoading( const PhysAbsoluteTime& startTime );

	//only level 3 sgavenges
	bool isScavenger() const;

	//perform the scavenging animations
	const PhysRelativeTime doScavenge( const PhysAbsoluteTime& startTime );
	//PRE( isScavenger() );

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachPhysResourceCarrier& t );

    PER_MEMBER_PERSISTENT( MachPhysResourceCarrier );
    PER_FRIEND_READ_WRITE( MachPhysResourceCarrier );

    typedef size_t  Id;

private:
    // Operation deliberately revoked
    MachPhysResourceCarrier( const MachPhysResourceCarrier& );

    // Operation deliberately revoked
    MachPhysResourceCarrier& operator =( const MachPhysResourceCarrier& );

    // Operation deliberately revoked
    bool operator ==( const MachPhysResourceCarrier& );

    typedef MachPhysObjectFactory< Id, MachPhysResourceCarrier >    Factory;

    //  This is necessary to allow the ti file to instantiate the factory class
    //friend MachPhysResourceCarrier& Factory::part( const ID&, size_t );
    //friend class Factory;
    friend class MachPhysObjectFactory< Id, MachPhysResourceCarrier > ;

    //  This constructor for use by the factory only
    MachPhysResourceCarrier( W4dEntity* pParent, size_t bodyLevel );

    //  Necessary to allow the persistence mechanism write out the factory
    friend void perWrite( PerOstream&, const MachPhysMachinePersistence& );
    friend void perRead( PerIstream&, MachPhysMachinePersistence& );

    SysPathName compositeFileName( size_t bodyLevel ) const;
    static  MachPhysResourceCarrier&    part( size_t bodyLevel );
    static  Factory& factory();

    void createExplosionData();

	size_t bodyLevel_;
};

PER_DECLARE_PERSISTENT( MachPhysResourceCarrier );

#endif

/* End RESCARR.HPP *************************************************/
