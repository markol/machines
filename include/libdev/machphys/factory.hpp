/*
 * M I N E . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    MachPhysFactory

    A brief description of the class should go in here
*/

#ifndef _MACHPHYS_FACTORY_HPP
#define _MACHPHYS_FACTORY_HPP

#include "base/base.hpp"
#include "base/persist.hpp"

#include "machphys/constron.hpp"

class MachPhysFactoryData;
class PhysTimedAnglePlan;
class MachPhysConstructionPersistence;
class MachPhysSmokePlume;
template< class ID, class PART > class MachPhysObjectFactory;
template< class SUBTYPE > class MachPhysSubTypeId;

class MachPhysFactory : public MachPhysConstruction
{
public:
    MachPhysFactory(
        W4dEntity* pParent,
        const W4dTransform3d& localTransform,
        MachPhys::FactorySubType subType,
        size_t level,
        MachPhys::Race race );

    virtual ~MachPhysFactory( void );

	//return MachPhysData object for this building
	virtual const MachPhysConstructionData& constructionData() const;
	const MachPhysFactoryData& data() const;

    MachPhys::FactorySubType subType( void ) const;

	virtual void damageLevel( const double& percent );

    void CLASS_INVARIANT;

    PER_MEMBER_PERSISTENT( MachPhysFactory );
    PER_FRIEND_READ_WRITE( MachPhysFactory );

    typedef MachPhysSubTypeId< MachPhys::FactorySubType >        Id;

private:
    // Operation deliberately revoked
    MachPhysFactory( const MachPhysFactory& );

    // Operation deliberately revoked
    MachPhysFactory& operator =( const MachPhysFactory& );

    // Operation deliberately revoked
    bool operator ==( const MachPhysFactory& );

    typedef MachPhysObjectFactory< Id, MachPhysFactory >    Factory;

    //  This is necessary to allow the ti file to instantiate the factory class
    //friend MachPhysFactory& Factory::part( const ID&, size_t );
    //friend class Factory;
    friend class MachPhysObjectFactory< Id, MachPhysFactory >;


    //  Necessary to allow the persistence mechanism write out the factory
    friend void perWrite( PerOstream&, const MachPhysConstructionPersistence& );
    friend void perRead( PerIstream&, MachPhysConstructionPersistence& );

    static  MachPhysFactory& part( MachPhys::FactorySubType subType, size_t level );
    static  Factory& factory();

    SysPathName compositeFileName( MachPhys::FactorySubType subType, size_t level ) const;
    SysPathName wireframeFileName( MachPhys::FactorySubType subType, size_t level ) const;
    SysPathName interiorCompositeFileName( MachPhys::FactorySubType subType, size_t level ) const;

    //  This is the constructor that is used by the factory. It is the
    //  only constructor that actually builds a factory from scratch

    MachPhysFactory( W4dEntity* pParent, Id id );

    void doWorking( bool isWorking );

	// emit smoke if sufficient time has passed since the last puff was emitted
	void dealWithSmoke();

    //  Handle the data initialisation when a construction is read in from
    //  a persistence file
    void persistenceInitialiseData();

    MachPhysFactoryData* pData_;
    MachPhys::FactorySubType subType_;

	W4dLink* pFan1_;
	W4dLink* pFan2_;
	W4dLink* pFan3_;
	W4dLink* pFan4_;

	size_t level_;

	MachPhysSmokePlume* pSmoke1_;
	MachPhysSmokePlume* pSmoke2_;
	MachPhysSmokePlume* pSmoke3_;
	MachPhysSmokePlume* pSmoke4_;
	MachPhysSmokePlume* pSmoke5_;

	PhysAbsoluteTime nextSmokeTime_;
};

PER_DECLARE_PERSISTENT( MachPhysFactory );

#endif

/* End FACTORY.HPP *****************************************************/
