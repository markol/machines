/*
 * MINE . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    MachLogMine

    Handle logical operations for a Mine construction
*/

#ifndef _MACHLOG_MINE_HPP
#define _MACHLOG_MINE_HPP

#include "base/base.hpp"

#include "machlog/constron.hpp"

//Forward declarations
class MachPhysMine;
class MachLogRace;
class MexPoint3d;
class MexRadians;
class MachLogMineralSite;
class MachLogSmelter;
class MachPhysMineData;

//Orthodox canonical (revoked)
class MachLogMine : public MachLogConstruction
{
public:
	enum CheckDiscoveredFlag { IGNORE_DISCOVERED_FLAG, CHECK_DISCOVERED_FLAG };
    //Construct smelter of deisgnated race and level at location, rotated thru angle about
    //z axis.
    MachLogMine( MachLogRace* pRace, uint level,
                    const MexPoint3d& location, const MexRadians& angle, const CheckDiscoveredFlag check );

    MachLogMine( MachLogRace* pRace, uint level,
                    const MexPoint3d& location, const MexRadians& angle, 
                    const CheckDiscoveredFlag check , UtlId );

    ~MachLogMine( void );

    //Inherited from SimActor
    virtual PhysRelativeTime update( const PhysRelativeTime& maxCPUTime,
                                     MATHEX_SCALAR clearanceFromDisplayedVolume );

	virtual void doOutputOperator( ostream& o ) const;						

	virtual const MachPhysConstructionData& constructionData() const;
	const MachPhysMineData& data() const;

    //True if location is close enough to a mineral deposit to place a mine
    static bool validMineSite( const MexPoint3d& location, uint hwLevel, const CheckDiscoveredFlag );
	
	static bool validForAllSmelters ( const MexPoint3d& location );
	static bool outsideMinimumMineSmelterRange ( const MexPoint3d& location, const MachLogConstruction* pTarget);

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachLogMine& t );

	MachPhys::BuildingMaterialUnits	ore() const;

	bool hasMineralSite() const;
	const MachLogMineralSite& mineralSite() const;
	//PRE( hasMineralSite );
	
	// meaning that the mine is either still active, or still has ore left to pick up at the surface
	bool worthVisiting();
	
	void mineralSiteIsExhausted();
	void newMineralSiteAvailable();
	
	MachPhys::BuildingMaterialUnits removeOre( MachPhys::BuildingMaterialUnits oreRemoved );

	PER_MEMBER_PERSISTENT_VIRTUAL( MachLogMine );
	PER_FRIEND_READ_WRITE( MachLogMine );

protected:

	//virtual
	void doStartExplodingAnimation();
	//virtual
	void doEndExplodingAnimation();

private:
    // Operations deliberately revoked
    MachLogMine( const MachLogMine& );
    MachLogMine& operator =( const MachLogMine& );
    bool operator ==( const MachLogMine& );

    //Construct a new physical smelter. Used in ctor initializer list
    static MachPhysMine* pNewPhysMine( MachLogRace* pRace, uint level,
                     const MexPoint3d& location, const MexRadians& angle );

    //The physical mine
    MachPhysMine* pPhysMine();
    const MachPhysMine* pPhysMine() const;
	
	// The minerals site we're currently attached to
	MachLogMineralSite& mineralSite();
	//PRE( hasMineralSite );
	
	// returns true if valid site found. Updates pSite_ as a side-effect if succeeds.
	bool assignToNearestMineralSite();
	
	// returns true if valid site found. Does not check for the site having been discovered. If
	// valid site found, marks it as discovered if it was not already. Updates pSite_ as a side-effect if succeeds.
	bool discoverAndAssignToNearestMineralSite( MachLogRace* pRace );	
	
	//maximum distance a mine is allowed to be away from the nearest (discovered) mineral site given its hw level	
	static MATHEX_SCALAR sqrMaxMineralDistance( uint hwLevel );


    //Data members
	MachPhys::BuildingMaterialUnits	ore_;
	MachLogMineralSite*				pSite_;
	PhysAbsoluteTime				lastUpdateTime_;
	
};

PER_DECLARE_PERSISTENT( MachLogMine );

#ifdef _INLINE
    #include "machlog/mine.ipp"
#endif


#endif

/* End MINE.HPP **************************************************/
