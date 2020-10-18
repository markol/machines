/*
 * M I N E . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    MachPhysMine

    A brief description of the class should go in here
*/

#ifndef _MACHPHYS_MINE_HPP
#define _MACHPHYS_MINE_HPP

#include "base/base.hpp"
#include "base/persist.hpp"

#include "machphys/constron.hpp"
#include "world4d/link.hpp"
#include "ctl/vector.hpp"


class MachPhysMineData;
class MachPhysConstructionPersistence;
class W4dLink;
class W4dGenericComposite;
template< class ID, class PART > class MachPhysObjectFactory;
template< class T > class CtlCountedPtr;
template< class T > class ctl_pvector;
class W4dCompositePlan;
typedef CtlCountedPtr< W4dCompositePlan > W4dCompositePlanPtr;

class MachPhysMine : public MachPhysConstruction
{
public:
    MachPhysMine(
        W4dEntity* pParent,
        const W4dTransform3d& localTransform,
        size_t level,
        MachPhys::Race race );

    virtual ~MachPhysMine( void );

    void CLASS_INVARIANT;

	//return MachPhysData object for this building
	virtual const MachPhysConstructionData& constructionData() const;
	const MachPhysMineData& data() const;

	virtual void damageLevel( const double& percent );

    PER_MEMBER_PERSISTENT( MachPhysMine );
    PER_FRIEND_READ_WRITE( MachPhysMine );

    typedef size_t  Id;

protected:

    //Inherited from MachPhysConstruction
    virtual void doWorking( bool isWorking );

private:
    // Operation deliberately revoked
    MachPhysMine( const MachPhysMine& );

    // Operation deliberately revoked
    MachPhysMine& operator =( const MachPhysMine& );

    // Operation deliberately revoked
    bool operator ==( const MachPhysMine& );

    //  This is necessary to allow the ti file to instantiate the factory class
    //friend MachPhysMine& Factory::part( const ID&, size_t );
    typedef MachPhysObjectFactory< Id, MachPhysMine >   Factory;
    //friend class Factory;
    friend class MachPhysObjectFactory< Id, MachPhysMine >;

    //  Necessary to allow the persistence mechanism write out the factory
    friend void perWrite( PerOstream&, const MachPhysConstructionPersistence& );
    friend void perRead( PerIstream&, MachPhysConstructionPersistence& );

    static  MachPhysMine& part( size_t level );
    static  Factory& factory();

    SysPathName compositeFileName( size_t level ) const;
    SysPathName wireframeFileName( size_t level ) const;

    //  This is the constructor that is used by the factory. It is the
    //  only constructor that actually builds an Mine from scratch

    MachPhysMine( W4dEntity* pParent, Id bodyLevel );

    //Construct and return level4 working animation
    W4dCompositePlanPtr workingAnimationLevel4();

    //virtual void doPercentageComplete( double newPercentage );

    //  Handle the data initialisation when a construction is read in from
    //  a persistence file
    void persistenceInitialiseData();

    //  Data members

    MachPhysMineData* pData_;
	ctl_vector< W4dLink* > fans_;

};

PER_DECLARE_PERSISTENT( MachPhysMine );

#endif

/* End MINE.HPP *****************************************************/
