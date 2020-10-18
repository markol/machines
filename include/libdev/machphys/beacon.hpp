/*
 * B E A C O N . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    MachPhysBeacon

    A brief description of the class should go in here
*/

#ifndef _MACHPHYS_BEACON_HPP
#define _MACHPHYS_BEACON_HPP

#include "base/base.hpp"
#include "base/persist.hpp"

#include "machphys/constron.hpp"

class MachPhysBeaconData;
class MachPhysConstructionPersistence;
class W4dLink;
template< class ID, class PART > class MachPhysObjectFactory;

class MachPhysBeacon : public MachPhysConstruction
{
public:
    MachPhysBeacon(
        W4dEntity* pParent,
        const W4dTransform3d& localTransform,
        size_t level,
        MachPhys::Race race );

    virtual ~MachPhysBeacon( void );

	//return MachPhysData object for this building
	virtual const MachPhysConstructionData& constructionData() const;
	const MachPhysBeaconData& data() const;

	virtual void damageLevel( const double& percent );
    void CLASS_INVARIANT;

    PER_MEMBER_PERSISTENT( MachPhysBeacon );
    PER_FRIEND_READ_WRITE( MachPhysBeacon );

private:
    // Operation deliberately revoked
    MachPhysBeacon( const MachPhysBeacon& );

    // Operation deliberately revoked
    MachPhysBeacon& operator =( const MachPhysBeacon& );

    // Operation deliberately revoked
    bool operator ==( const MachPhysBeacon& );

    typedef MachPhysObjectFactory< size_t, MachPhysBeacon >    Factory;

    //  This is necessary to allow the ti file to instantiate the factory class
    //friend MachPhysBeacon& Factory::part( const ID&, size_t );
    //friend class Factory;
    friend class MachPhysObjectFactory< size_t, MachPhysBeacon >;

    //  Necessary to allow the persistence mechanism write out the factory
    friend void perWrite( PerOstream&, const MachPhysConstructionPersistence& );
    friend void perRead( PerIstream&, MachPhysConstructionPersistence& );

    static  MachPhysBeacon& part( size_t level );
    static  Factory& factory();

    SysPathName compositeFileName( size_t level ) const;
    SysPathName wireframeFileName( size_t level ) const;

    //  This is the constructor that is used by the factory. It is the
    //  only constructor that actually builds a beacon from scratch

    MachPhysBeacon( W4dEntity* pParent, size_t bodyLevel );

	void doWorking(bool isWorking);

    //  Handle the data initialisation when a construction is read in from
    //  a persistence file
    void persistenceInitialiseData();

    MachPhysBeaconData* pData_;

	W4dLink* pTurner_;
};

PER_DECLARE_PERSISTENT( MachPhysBeacon );

#endif

/* End BEACON.HPP *****************************************************/
