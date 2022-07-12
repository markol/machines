/*
 * H W L A B . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    MachPhysHardwareLab

    A brief description of the class should go in here
*/

#ifndef _MACHPHYS_HWLAB_HPP
#define _MACHPHYS_HWLAB_HPP

#include "base/base.hpp"
#include "base/persist.hpp"

#include "machphys/constron.hpp"
#include "world4d/link.hpp"
#include "ctl/vector.hpp"

class MachPhysHardwareLabData;
class MachPhysConstructionPersistence;
template< class ID, class PART > class MachPhysObjectFactory;
template< class SUBTYPE > class MachPhysSubTypeId;

class MachPhysHardwareLab : public MachPhysConstruction
{
public:
    MachPhysHardwareLab(
        W4dEntity* pParent,
        const W4dTransform3d& localTransform,
		MachPhys::HardwareLabSubType subType,
        size_t level,
        MachPhys::Race race );

    virtual ~MachPhysHardwareLab();

	//return MachPhysData object for this building
	virtual const MachPhysConstructionData& constructionData() const;
	const MachPhysHardwareLabData& data() const;

    MachPhys::HardwareLabSubType subType() const;

	virtual void damageLevel( const double& percent );

    void CLASS_INVARIANT;

    PER_MEMBER_PERSISTENT( MachPhysHardwareLab );
    PER_FRIEND_READ_WRITE( MachPhysHardwareLab );

    typedef MachPhysSubTypeId< MachPhys::HardwareLabSubType >        Id;

private:
    // Operation deliberately revoked
    MachPhysHardwareLab( const MachPhysHardwareLab& );

    // Operation deliberately revoked
    MachPhysHardwareLab& operator =( const MachPhysHardwareLab& );

    // Operation deliberately revoked
    bool operator ==( const MachPhysHardwareLab& );

    typedef MachPhysObjectFactory< Id, MachPhysHardwareLab >    Factory;

    //  This is necessary to allow the ti file to instantiate the factory class
    //friend MachPhysHardwareLab& Factory::part( const ID&, size_t );
    //friend class Factory;
    friend class MachPhysObjectFactory< Id, MachPhysHardwareLab >;

    //  Necessary to allow the persistence mechanism write out the factory
    friend void perWrite( PerOstream&, const MachPhysConstructionPersistence& );
    friend void perRead( PerIstream&, MachPhysConstructionPersistence& );

    //static  MachPhysHardwareLab& factory( MachPhys::HardwareLabSubType, size_t level );
    static  MachPhysHardwareLab& part( MachPhys::HardwareLabSubType, size_t level );
    static  Factory& factory();

    SysPathName compositeFileName( MachPhys::HardwareLabSubType, size_t level ) const;
    SysPathName wireframeFileName( MachPhys::HardwareLabSubType, size_t level ) const;
    SysPathName interiorCompositeFileName( MachPhys::HardwareLabSubType, size_t level ) const;

    //Inherited from MachPhysConstruction
    virtual void doWorking( bool isWorking );

    //  This is the constructor that is used by the factory. It is the
    //  only constructor that actually builds a hardware lab from scratch

    MachPhysHardwareLab( W4dEntity* pParent, Id bodyLevel );

    //  Handle the data initialisation when a construction is read in from
    //  a persistence file
    void persistenceInitialiseData();

    MachPhysHardwareLabData* pData_;
    MachPhys::HardwareLabSubType subType_;
	size_t level_;
	ctl_vector< W4dLink* > neons_;
};

PER_DECLARE_PERSISTENT( MachPhysHardwareLab );

#endif

/* End HWLAB.HPP *****************************************************/
