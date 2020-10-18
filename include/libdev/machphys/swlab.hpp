/*
 * S W L A B . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    MachPhysSoftwareLab

    A brief description of the class should go in here
*/

#ifndef _MACHPHYS_SWLAB_HPP
#define _MACHPHYS_SWLAB_HPP

#include "base/base.hpp"
#include "base/persist.hpp"

#include "machphys/constron.hpp"
#include "machphys/ofactory.hpp"
#include "machphys/snddata.hpp"

class MachPhysSoftwareLabData;

class MachPhysSoftwareLab : public MachPhysConstruction
{
public:
    MachPhysSoftwareLab(
        W4dEntity* pParent,
        const W4dTransform3d& localTransform,
        size_t level,
        MachPhys::Race race );

    virtual ~MachPhysSoftwareLab( void );

	//return MachPhysData object for this building
	virtual const MachPhysConstructionData& constructionData() const;
	const MachPhysSoftwareLabData& data() const;

    void CLASS_INVARIANT;

    PER_MEMBER_PERSISTENT( MachPhysSoftwareLab );
    PER_FRIEND_READ_WRITE( MachPhysSoftwareLab );

    typedef size_t  Id;

private:
    // Operation deliberately revoked
    MachPhysSoftwareLab( const MachPhysSoftwareLab& );

    // Operation deliberately revoked
    MachPhysSoftwareLab& operator =( const MachPhysSoftwareLab& );

    // Operation deliberately revoked
    bool operator ==( const MachPhysSoftwareLab& );

    typedef MachPhysObjectFactory< Id, MachPhysSoftwareLab >    Factory;

    //  This is necessary to allow the ti file to instantiate the factory class
    //friend MachPhysSoftwareLab& Factory::part( const ID&, size_t );

    static  MachPhysSoftwareLab& factory( size_t level );
    SysPathName compositeFileName( size_t level ) const;
    SysPathName wireframeFileName( size_t level ) const;
    SysPathName interiorCompositeFileName( size_t level ) const;
    void initialiseLevel1();

    //  This is the constructor that is used by the factory. It is the
    //  only constructor that actually builds a software lab from scratch

    MachPhysSoftwareLab( W4dEntity* pParent, Id bodyLevel );

    MachPhysSoftwareLabData* pData_;
};

PER_DECLARE_PERSISTENT( MachPhysSoftwareLab );

#endif

/* End SWLAB.HPP *****************************************************/
