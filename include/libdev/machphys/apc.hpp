/*
 * A P C . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    MachPhysAPC

    Implement the physical properties of a personnel carrier
*/

#ifndef _MACHPHYS_APC_HPP
#define _MACHPHYS_APC_HPP

#include "base/base.hpp"
#include "base/persist.hpp"

#include "machphys/machine.hpp"
#include "machphys/machphys.hpp"

class MachPhysAPCData;
class MachPhysMachinePersistence;
template< class ID, class PART > class MachPhysObjectFactory;

class MachPhysAPC : public MachPhysMachine
{
public:
//     MachPhysAPC(
//         W4dEntity* pParent,
//         const W4dTransform3d& localTransform,
//         const SysPathName& compositeFileName );

    MachPhysAPC(
        W4dEntity* pParent,
        const W4dTransform3d& localTransform,
        size_t bodyLevel,
        size_t brainLevel,
        MachPhys::Race race );

//     MachPhysAPC(
//         W4dEntity* pParent,
//         const MexTransform3d& transform,
//         const MachPhysAPC& copyMe,
//         const W4dLink& faceplate );

    virtual ~MachPhysAPC( void );

	virtual const MachPhysMachineData& machineData() const;

	const MachPhysAPCData& data() const;

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachPhysAPC& t );

    PER_MEMBER_PERSISTENT( MachPhysAPC );
    PER_FRIEND_READ_WRITE( MachPhysAPC );

    typedef size_t  Id;

private:
    // Operation deliberately revoked
    MachPhysAPC( const MachPhysAPC& );

    // Operation deliberately revoked
    MachPhysAPC& operator =( const MachPhysAPC& );

    // Operation deliberately revoked
    bool operator ==( const MachPhysAPC& );

    typedef MachPhysObjectFactory< Id, MachPhysAPC >    Factory;

    //  This is necessary to allow the ti file to instantiate the factory class
    //friend MachPhysAPC& Factory::part( const ID& id, size_t );
    //friend class Factory;
    friend class MachPhysObjectFactory< Id, MachPhysAPC >;

    //  This constructor for use by the factory only
    MachPhysAPC( W4dEntity* pParent, Id bodyLevel );

    SysPathName compositeFileName( size_t bodyLevel ) const;

    //  Necessary to allow the persistence mechanism write out the factory
    friend void perWrite( PerOstream&, const MachPhysMachinePersistence& );
    friend void perRead( PerIstream&, MachPhysMachinePersistence& );

    static  MachPhysAPC&    part( size_t bodyLevel );
    static  Factory& factory();

    void createExplosionData( void );
};

PER_DECLARE_PERSISTENT( MachPhysAPC );

#endif

/* End PERCARR.HPP *************************************************/
