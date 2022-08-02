/*
 * T E C H N I C I . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    MachPhysTechnician

    Implement the physical properties of a transporter
*/

#ifndef _TECHNICI_HPP
#define _TECHNICI_HPP

#include "base/base.hpp"
#include "base/persist.hpp"

#include "machphys/machine.hpp"
#include "machphys/machphys.hpp"

class MachPhysTechnicianData;
class MachPhysMachinePersistence;
template< class ID, class PART > class MachPhysObjectFactory;
template< class SUBTYPE > class MachPhysSubTypeId;

class MachPhysTechnician : public MachPhysMachine
{
public:
    MachPhysTechnician(
        W4dEntity* pParent,
        const W4dTransform3d& localTransform,
        MachPhys::TechnicianSubType subType,
        size_t bodyLevel,
        size_t brainLevel,
        MachPhys::Race race );

    virtual ~MachPhysTechnician();

	virtual const MachPhysMachineData& machineData() const;

	const MachPhysTechnicianData& data() const;

    MachPhys::TechnicianSubType subType() const;

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachPhysTechnician& t );

	PhysRelativeTime research();

    PER_MEMBER_PERSISTENT( MachPhysTechnician );
    PER_FRIEND_READ_WRITE( MachPhysTechnician );

    typedef MachPhysSubTypeId< MachPhys::TechnicianSubType > Id;

private:
    // Operation deliberately revoked
    MachPhysTechnician( const MachPhysTechnician& );

    // Operation deliberately revoked
    MachPhysTechnician& operator =( const MachPhysTechnician& );

    // Operation deliberately revoked
    bool operator ==( const MachPhysTechnician& );

    typedef MachPhysObjectFactory< Id, MachPhysTechnician >    Factory;

    //  This is necessary to allow the ti file to instantiate the factory class
    //friend MachPhysTechnician& Factory::part( const ID&, size_t );
    //friend class Factory;
    friend class MachPhysObjectFactory< Id, MachPhysTechnician >;

    //  Necessary to allow the persistence mechanism write out the factory
    friend void perWrite( PerOstream&, const MachPhysMachinePersistence& );
    friend void perRead( PerIstream&, MachPhysMachinePersistence& );

    static  MachPhysTechnician& part( MachPhys::TechnicianSubType subType, size_t bodyLevel );
    static  Factory& factory();

	void createExplosionData();
    SysPathName compositeFileName( MachPhys::TechnicianSubType subType, size_t bodyLevel ) const;

    //  This is the constructor that is used by the factory. It is the
    //  only constructor that actually builds an administrator from scratch

    MachPhysTechnician( W4dEntity* pParent, Id id );

    //Data members
    MachPhys::TechnicianSubType subType_;
};

PER_DECLARE_PERSISTENT( MachPhysTechnician );

#endif

/* End TECHNICI.HPP *************************************************/
