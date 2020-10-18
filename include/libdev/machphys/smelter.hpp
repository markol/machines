/*
 * S M E L T E R . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    MachPhysSmelter

    A brief description of the class should go in here
*/

#ifndef _MACHPHYS_SMELTER_HPP
#define _MACHPHYS_SMELTER_HPP

#include "base/base.hpp"
#include "base/persist.hpp"

#include "machphys/constron.hpp"

#include "phys/phys.hpp"

class MachPhysSmelterData;
class PhysTimedAnglePlan;
class MachPhysSmokePlume;
class MachPhysConstructionPersistence;
template< class ID, class PART > class MachPhysObjectFactory;

class MachPhysSmelter : public MachPhysConstruction
{
public:
    MachPhysSmelter(
        W4dEntity* pParent,
        const W4dTransform3d& localTransform,
        size_t level,
        MachPhys::Race race );

    virtual ~MachPhysSmelter( void );

	//return MachPhysData object for this building
	virtual const MachPhysConstructionData& constructionData() const;
	const MachPhysSmelterData& data() const;

	virtual void damageLevel( const double& percent );

    void CLASS_INVARIANT;

    PER_MEMBER_PERSISTENT( MachPhysSmelter );
    PER_FRIEND_READ_WRITE( MachPhysSmelter );

    typedef size_t  Id;

private:
    // Operation deliberately revoked
    MachPhysSmelter( const MachPhysSmelter& );

    // Operation deliberately revoked
    MachPhysSmelter& operator =( const MachPhysSmelter& );

    // Operation deliberately revoked
    bool operator ==( const MachPhysSmelter& );


    typedef MachPhysObjectFactory< size_t, MachPhysSmelter >    Factory;
    //  This is necessary to allow the ti file to instantiate the factory class
    //friend MachPhysSmelter& Factory::part( const ID&, size_t );
    //friend class Factory;
    friend class MachPhysObjectFactory< size_t, MachPhysSmelter >;

    //  Necessary to allow the persistence mechanism write out the factory
    friend void perWrite( PerOstream&, const MachPhysConstructionPersistence& );
    friend void perRead( PerIstream&, MachPhysConstructionPersistence& );

    static  MachPhysSmelter& part( size_t level );
    static  Factory& factory();

    SysPathName compositeFileName( size_t level ) const;
    SysPathName wireframeFileName( size_t level ) const;
    SysPathName interiorCompositeFileName( size_t level ) const;

    //  This is the constructor that is used by the factory. It is the
    //  only constructor that actually builds a smelter from scratch

    MachPhysSmelter( W4dEntity* pParent, size_t bodyLevel );

    void doWorking( bool isWorking );
    void turnFansOn();

    //  Handle the data initialisation when a construction is read in from
    //  a persistence file
    void persistenceInitialiseData();

    MachPhysSmelterData* pData_;

	W4dLink* pFan1_;
	W4dLink* pFan2_;
	PhysTimedAnglePlan* pAnglePlan1_;
	PhysTimedAnglePlan* pAnglePlan2_;

	MachPhysSmokePlume* pSmoke_;
    PhysAbsoluteTime lastSmokePlumeTime_;
	size_t level_;
};

PER_DECLARE_PERSISTENT( MachPhysSmelter );

#endif

/* End SMELTER.HPP *****************************************************/
