/*
 * P O D . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    MachPhysPod

    A brief description of the class should go in here
*/

#ifndef _MACHPHYS_POD_HPP
#define _MACHPHYS_POD_HPP

#include "base/base.hpp"
#include "base/persist.hpp"

#include "machphys/constron.hpp"
#include "machphys/attack.hpp"

class MachPhysPodData;
class MachPhysConstructionPersistence;
template< class ID, class PART > class MachPhysObjectFactory;

class MachPhysPod : public MachPhysConstruction,
                    public MachPhysCanAttack
{
public:
    MachPhysPod(
        W4dEntity* pParent,
        const W4dTransform3d& localTransform,
        size_t level,
        MachPhys::Race race );

    virtual ~MachPhysPod( void );

	//return MachPhysData object for this building
	virtual const MachPhysConstructionData& constructionData() const;
	const MachPhysPodData& data() const;
	void ionCannonResearched( bool researched );

    ///////////////////////////////////////////////////////
    //Inherited from MachPhysCanAttack

    //Makes the underlying physical model available
    virtual W4dComposite& asComposite();
    virtual const W4dComposite& asComposite() const;

	virtual void damageLevel( const double& percent );

    //True iff the attacker can turn its weapon carrier to track a target
    virtual bool canTrackWeaponBase() const;

    ///////////////////////////////////////////////////////

    void CLASS_INVARIANT;

    PER_MEMBER_PERSISTENT( MachPhysPod );
    PER_FRIEND_READ_WRITE( MachPhysPod );

    typedef size_t  Id;

private:
    // Operation deliberately revoked
    MachPhysPod( const MachPhysPod& );

    // Operation deliberately revoked
    MachPhysPod& operator =( const MachPhysPod& );

    // Operation deliberately revoked
    bool operator ==( const MachPhysPod& );

    //  This is necessary to allow the ti file to instantiate the factory class
    //friend MachPhysPod& Factory::part( const ID&, size_t );
    typedef MachPhysObjectFactory< Id, MachPhysPod >    Factory;
    //friend class Factory;
    friend class MachPhysObjectFactory< Id, MachPhysPod >;

    //  Necessary to allow the persistence mechanism write out the factory
    friend void perWrite( PerOstream&, const MachPhysConstructionPersistence& );
    friend void perRead( PerIstream&, MachPhysConstructionPersistence& );

    static  MachPhysPod& part( size_t level );
    static  Factory& factory();

    SysPathName compositeFileName( size_t level ) const;
    SysPathName wireframeFileName( size_t ) const;
    SysPathName interiorCompositeFileName( size_t level ) const;

    //  This is the constructor that is used by the factory. It is the
    //  only constructor that actually builds a pod from scratch

    MachPhysPod( W4dEntity* pParent, size_t level );

	void doWorking(bool isWorking);

    //  Handle the data initialisation when a construction is read in from
    //  a persistence file
    void persistenceInitialiseData();

    MachPhysPodData* pData_;

	W4dLink* pTop_;
	W4dLink* pIonTop_;
	W4dLink* pTurner_;
};

PER_DECLARE_PERSISTENT( MachPhysPod );

#endif

/* End POD.HPP *****************************************************/
