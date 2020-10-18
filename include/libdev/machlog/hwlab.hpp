/*
 * HWLAB . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    MachLogHardwareLab

    Handle logical operations for a HardwareLab construction
*/

#ifndef _MACHLOG_HWLAB_HPP
#define _MACHLOG_HWLAB_HPP

#include "base/base.hpp"

#include "machlog/constron.hpp"
#include "machlog/restree.hpp"

//Forward declarations
class MachPhysHardwareLab;
class MachLogRace;
class MexPoint3d;
class MexRadians;
class MachLogResearchItem;
class MachLogHardwareLabImpl;
class MachPhysHardwareLabData;
//Orthodox canonical (revoked)
class MachLogHardwareLab : public MachLogConstruction
{
public:
    //Construct hwlab of deisgnated race and level at location, rotated thru angle about
    //z axis.
    MachLogHardwareLab( MachPhys::HardwareLabSubType subType, MachLogRace* pRace, uint level,
                    const MexPoint3d& location, const MexRadians& angle );

    MachLogHardwareLab( MachPhys::HardwareLabSubType subType, MachLogRace* pRace, uint level,
                    const MexPoint3d& location, const MexRadians& angle, UtlId );

    ~MachLogHardwareLab( void );

    //Inherited from SimActor
    virtual PhysRelativeTime update( const PhysRelativeTime& maxCPUTime,
                                     MATHEX_SCALAR clearanceFromDisplayedVolume );

	virtual const MachPhysConstructionData& constructionData() const;
	const MachPhysHardwareLabData& data() const;

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachLogHardwareLab& t );
//	const MachLogResearchTree::ResearchItems& researchItems();
	MachLogResearchTree::ResearchItems& availableResearchItems();
	MachLogResearchTree::ResearchItems& currentResearchQueue();
	//this function will check against the MLRaces::ReserachTree for availability and also
	//for new technicians turning things on via tech level. once something is on it is permanent.
	void initialiseResearchItems();

	//if you add something that you can't see or can't be found
	//in researchtree returns false.
	bool addResearchItem( const MachLogResearchItem& );
	//returns false if it wasn't in the list of things to research.
	void removeResearchItem( const MachLogResearchItem& );
	void cancelAllResearch();
	bool currentlyResearching( MachLogResearchItem** ) const;
	MachPhys::BuildingMaterialUnits amountBuilt() const;
	MachPhys::ResearchUnits amountResearched() const;
	enum MoveDirection { TOWARDS_FRONT, TOWARDS_BACK };
	void moveResearchItem( const MachLogResearchItem&, MoveDirection );

	const MachPhys::HardwareLabSubType subType() const;
	
	// cumulative research rate of all currently residing technicians
	float totalResearchRate() const;

	PER_MEMBER_PERSISTENT_VIRTUAL( MachLogHardwareLab );
	PER_FRIEND_READ_WRITE( MachLogHardwareLab );


protected:
	virtual void doOutputOperator( ostream& ) const;

private:
    // Operations deliberately revoked
    MachLogHardwareLab( const MachLogHardwareLab& );
    MachLogHardwareLab& operator =( const MachLogHardwareLab& );
    bool operator ==( const MachLogHardwareLab& );

    //Construct a new physical hwlab. Used in ctor initializer list
    static MachPhysHardwareLab* pNewPhysHardwareLab( MachPhys::HardwareLabSubType subType, MachLogRace* pRace, uint level,
                     const MexPoint3d& location, const MexRadians& angle );

    //The physical hardware lab
    MachPhysHardwareLab* pPhysHardwareLab();
    const MachPhysHardwareLab* 		pPhysHardwareLab() const;

	void ensureLabAnimationIsOn();
	void ensureLabAnimationIsOff();

    //Data members
	MachLogHardwareLabImpl* 		pImpl_;
};

PER_DECLARE_PERSISTENT( MachLogHardwareLab );

#ifdef _INLINE
    #include "machlog/hwlab.ipp"
#endif


#endif

/* End HWLAB.HPP **************************************************/
