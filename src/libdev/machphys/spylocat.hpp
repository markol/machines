/*
 * S P Y L O C A T . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    MachPhysSpyLocator

    Implement the physical properties of a spy locator
*/

#ifndef _MACHPHYS_SPYLOCAT_HPP
#define _MACHPHYS_SPYLOCAT_HPP

#include "base/base.hpp"
#include "base/persist.hpp"
#include "phys/phys.hpp"
#include "machphys/machine.hpp"


class MachPhysSpyLocatorData;
class MachPhysMachinePersistence;
class MachPhysSpyLocatorImpl;
template< class ID, class PART > class MachPhysObjectFactory;

class MachPhysSpyLocator : public MachPhysMachine
{
public:
    MachPhysSpyLocator(
        W4dEntity* pParent,
        const W4dTransform3d& localTransform,
        size_t bodyLevel,
        size_t brainLevel,
        MachPhys::Race race );

    virtual ~MachPhysSpyLocator();

    //Set/get locating mode
    void isLocating( bool doLocate );
    bool isLocating() const;

    //Inherited from MachPhysMachine
    virtual void move( const MachPhysMachineMoveInfo& info );

	virtual const MachPhysMachineData& machineData() const;

	const MachPhysSpyLocatorData& data() const;

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachPhysSpyLocator& t );

    PER_MEMBER_PERSISTENT( MachPhysSpyLocator );
    PER_FRIEND_READ_WRITE( MachPhysSpyLocator );

private:
    // Operations deliberately revoked
    MachPhysSpyLocator( const MachPhysSpyLocator& );
    MachPhysSpyLocator& operator =( const MachPhysSpyLocator& );
    bool operator ==( const MachPhysSpyLocator& );

    //  This is necessary to allow the ti file to instantiate the factory class
//    friend MachPhysSpyLocator& Factory::part( const ID&, size_t );
    typedef MachPhysObjectFactory< size_t, MachPhysSpyLocator >    Factory;
    //friend class Factory;
    friend class MachPhysObjectFactory< size_t, MachPhysSpyLocator >;

    //  Necessary to allow the persistence mechanism write out the factory
    friend void perWrite( PerOstream&, const MachPhysMachinePersistence& );
    friend void perRead( PerIstream&, MachPhysMachinePersistence& );

    static  MachPhysSpyLocator& part( size_t bodyLevel );
    static  Factory& factory();

    SysPathName compositeFileName( size_t bodyLevel ) const;

    //  This is the constructor that is used by the factory. It is the
    //  only constructor that actually builds an administrator from scratch

    MachPhysSpyLocator( W4dEntity* pParent, size_t bodyLevel );

    void createExplosionData();

    // Data members
    MachPhysSpyLocatorImpl* pImpl_;
};

PER_DECLARE_PERSISTENT( MachPhysSpyLocator );

#endif

/* End SPYLOCAT.HPP *************************************************/
