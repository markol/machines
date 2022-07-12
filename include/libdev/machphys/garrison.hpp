/*
 * G A R R I S O N . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    MachPhysGarrison

    A brief description of the class should go in here
*/

#ifndef _MACHPHYS_GARRISON_HPP
#define _MACHPHYS_GARRISON_HPP

#include "machphys/constron.hpp"
#include "machphys/ofactory.hpp"
#include "machphys/constron.hpp"


class MachPhysGarrisonData;
class MachPhysConstructionPersistence;
template< class ID, class PART > class MachPhysObjectFactory;

class MachPhysGarrison : public MachPhysConstruction
{
public:
    MachPhysGarrison(
        W4dEntity* pParent,
        const W4dTransform3d& localTransform,
        size_t level,
        MachPhys::Race race );

    virtual ~MachPhysGarrison();

	//return MachPhysData object for this building
	virtual const MachPhysConstructionData& constructionData() const;
	const MachPhysGarrisonData& data() const;
	void doPercentageComplete( double newPercentage );

	virtual void damageLevel( const double& percent );

    void CLASS_INVARIANT;

    PER_MEMBER_PERSISTENT( MachPhysGarrison );
    PER_FRIEND_READ_WRITE( MachPhysGarrison );

    typedef size_t  Id;

private:
    // Operation deliberately revoked
    MachPhysGarrison( const MachPhysGarrison& );

    // Operation deliberately revoked
    MachPhysGarrison& operator =( const MachPhysGarrison& );

    // Operation deliberately revoked
    bool operator ==( const MachPhysGarrison& );


    //  This is necessary to allow the ti file to instantiate the factory class
    //friend MachPhysGarrison& Factory::part( const ID&, size_t );
    typedef MachPhysObjectFactory< Id, MachPhysGarrison >    Factory;
    //friend class Factory;
    friend class MachPhysObjectFactory< Id, MachPhysGarrison >;

    //  Necessary to allow the persistence mechanism write out the factory
    friend void perWrite( PerOstream&, const MachPhysConstructionPersistence& );
    friend void perRead( PerIstream&, MachPhysConstructionPersistence& );

    static  MachPhysGarrison& part( size_t level );
    static  Factory& factory();

    SysPathName compositeFileName( size_t level ) const;
    SysPathName wireframeFileName( size_t level ) const;
    SysPathName interiorCompositeFileName( size_t level ) const;

    //  This is the constructor that is used by the factory. It is the
    //  only constructor that actually builds a garrison from scratch

    MachPhysGarrison( W4dEntity* pParent, size_t bodyLevel );

    //  Handle the data initialisation when a construction is read in from
    //  a persistence file
    void persistenceInitialiseData();

    MachPhysGarrisonData* pData_;
};

PER_DECLARE_PERSISTENT( MachPhysGarrison );

#endif

/* End GARRISON.HPP *****************************************************/
