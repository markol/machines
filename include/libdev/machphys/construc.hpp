/*
 * C O N S T R U C . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    MachPhysConstructor

    Implement the physical properties of a transporter
*/

#ifndef _CONSTRUC_HPP
#define _CONSTRUC_HPP

#include "base/base.hpp"
#include "base/persist.hpp"

#include "machphys/machine.hpp"
#include "machphys/machphys.hpp"

class MachPhysConstructorData;
class MachPhysMachinePersistence;
class MachPhysLight;
class MachPhysConstruction;
class MachPhysSparks;
class MachPhysJetRing;
class W4dLink;
class W4dPointLight;
template< class ID, class PART > class MachPhysObjectFactory;
template< class SUBTYPE > class MachPhysSubTypeId;

class MachPhysConstructor : public MachPhysMachine
{
public:

    MachPhysConstructor(
        W4dEntity* pParent,
        const W4dTransform3d& localTransform,
        MachPhys::ConstructorSubType subType,
        size_t bodyLevel,
        size_t brainLevel,
        MachPhys::Race race );

    virtual ~MachPhysConstructor();

    //Run a constructing animation, returning time it will take
    PhysRelativeTime construct();

    void startConstructing( const MachPhysConstruction& construction, const PhysAbsoluteTime& startTime);
    void stopConstructing();

	bool isConstructing() const;
	void isConstructing( bool isConstructing );

	virtual const MachPhysMachineData& machineData() const;

	const MachPhysConstructorData& data() const;

    MachPhys::ConstructorSubType subType() const;

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachPhysConstructor& t );

    PER_MEMBER_PERSISTENT( MachPhysConstructor );
    PER_FRIEND_READ_WRITE( MachPhysConstructor );

    typedef MachPhysSubTypeId< MachPhys::ConstructorSubType >    Id;

private:
    // Operation deliberately revoked
    MachPhysConstructor( const MachPhysConstructor& );

    // Operation deliberately revoked
    MachPhysConstructor& operator =( const MachPhysConstructor& );

    // Operation deliberately revoked
    bool operator ==( const MachPhysConstructor& );

    typedef MachPhysObjectFactory< Id, MachPhysConstructor >    Factory;


    //  This is necessary to allow the ti file to instantiate the factory class
//    friend MachPhysConstructor& Factory::part( const ID&, size_t );
//    friend MachPhysConstructor& Factory::part( size_t );
    //friend class Factory;
    friend class MachPhysObjectFactory< Id, MachPhysConstructor >;

    //  Necessary to allow the persistence mechanism write out the factory
    friend void perWrite( PerOstream&, const MachPhysMachinePersistence& );
    friend void perRead( PerIstream&, MachPhysMachinePersistence& );

    static  MachPhysConstructor& part( MachPhys::ConstructorSubType subType, size_t bodyLevel );
    static  Factory& factory();

    void createExplosionData();

    SysPathName compositeFileName( MachPhys::ConstructorSubType subType, size_t bodyLevel ) const;

    //  This is the constructor that is used by the factory. It is the
    //  only constructor that actually builds a constructor from scratch

    MachPhysConstructor(
        W4dEntity* pParent, Id id );

    //Data members
    MachPhys::ConstructorSubType subType_;
	bool isConstructing_;

	W4dLink* pLJet_;
	W4dLink* pRJet_;

	MachPhysJetRing* pLJetRing_;
	MachPhysJetRing* pRJetRing_;

	MachPhysSparks* pLSparks_;
	MachPhysSparks* pRSparks_;

	W4dPointLight* pLLight_;
	W4dPointLight* pRLight_;

	MachPhysLight* pLLightShadow_;
	MachPhysLight* pRLightShadow_;
};

PER_DECLARE_PERSISTENT( MachPhysConstructor );

#endif

/* End CONSTRUC.HPP *************************************************/
