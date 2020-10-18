/*
 * S P Y . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved.
 */

#ifndef _MACHLOG_SPY_LOCATOR_HPP
#define _MACHLOG_SPY_LOCATOR_HPP

#include "machlog/machine.hpp"

#include "machphys/machphys.hpp"

/* //////////////////////////////////////////////////////////////// */

//class ostream;
class MachPhysSpyLocator;
class MachPhysSpyLocatorData;
class MachLogResearchItem;

class MachLogSpyLocator
: public MachLogMachine 
{
public:

	MachLogSpyLocator( MachLogMachine::Level hwLevel, MachLogMachine::Level swLevel, 
    						MachLogRace * pRace, const MexPoint3d& location );

	MachLogSpyLocator( MachLogMachine::Level hwLevel, MachLogMachine::Level swLevel, 
    						MachLogRace * pRace, const MexPoint3d& location, UtlId );

	virtual ~MachLogSpyLocator();
	
	///////////////////////////////

	//view of MachPhys data objects
	virtual const MachPhysMachineData& machineData() const;
	const MachPhysSpyLocatorData& data() const;
	
	virtual PhysRelativeTime update( const PhysRelativeTime& maxCPUTime,
                                      MATHEX_SCALAR clearence );

	void placeMine();
	void restockMines();
	int nResItemsICouldSteal( MachPhys::Race otherRace, MachPhys::HardwareLabSubType hardwareLabSubType );	//returns total number of stealable technologies at the present time
	
	// Get methods public
	int	nMines()	const;
	bool isDownloading() const	{ return isDownloading_; }
	bool fullyStockedMines() const;

	PER_MEMBER_PERSISTENT_VIRTUAL( MachLogSpyLocator );
	PER_FRIEND_READ_WRITE( MachLogSpyLocator );

protected:

	virtual void doOutputOperator( ostream& ) const;
	
private:

	MachLogSpyLocator( const MachLogSpyLocator& );
	MachLogSpyLocator& operator =( const MachLogSpyLocator& );
	bool operator ==( const MachLogSpyLocator& ) const;

    //The physical SpyLocator
    MachPhysSpyLocator& physSpyLocator();
    const MachPhysSpyLocator& physSpyLocator() const;
	
	//Functions used for stealing other races' technologies
   	MachLogResearchItem* stealNewResearchItem( MachPhys::Race otherRace, MachPhys::HardwareLabSubType hardwareLabSubType );
	bool stealable( const MachLogResearchItem& researchItem, MachPhys::Race otherRace, MachPhys::HardwareLabSubType hardwareLabSubType);
	
	void executeTheft( void );
	// PRE ( insideBuilding() );
	// PRE (  insideWhichBuilding().objectType() == MachLog::HARDWARE_LAB );


    static MachPhysSpyLocator* pNewPhysSpyLocator( Level hwLevel, Level swLevel, MachLogRace * pRace,
                                                const MexPoint3d& location );
												
	// Set methods private
	
	void isDownloading( bool newStatus );

	int						nMines_;
	PhysAbsoluteTime		lastUpdateTime_;
	MachLogResearchItem*	pResItemCurrentlyStealing_;
	bool					isDownloading_;				// Whether or not the spy is actively downloading an enemy's research item
};

PER_DECLARE_PERSISTENT( MachLogSpyLocator );
/* //////////////////////////////////////////////////////////////// */

#endif	/*	#ifndef _LOG_Spy_HPP	*/

/* End Spy.HPP **************************************************/