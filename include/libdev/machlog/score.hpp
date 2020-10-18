/*
 * S C O R E . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachLogScore

    Score. A score object contains all kinds of values hence the impl class.
*/

#ifndef _MACHLOG_SCORE_HPP
#define _MACHLOG_SCORE_HPP

#include "base/base.hpp"
#include "machphys/machphys.hpp"
#include "machlog/machlog.hpp"

class MachLogScoreImpl;

class MachLogScore
// Canonical form revoked
{
public:
    MachLogScore( MachPhys::Race );
    ~MachLogScore( void );

    void CLASS_INVARIANT;

	PER_MEMBER_PERSISTENT( MachLogScore );
	PER_FRIEND_READ_WRITE( MachLogScore );

	int machinesBuilt() const;
	void machineBuilt();

	int militaryMachinesBuilt() const;
	void militaryMachineBuilt();

	int constructionsBuilt() const;
	void constructionBuilt();

	//When one of my machines is destroyed the following data is updated
	int myMachinesDestroyed() const;
	void myMachineDestroyed();
	
	//total of all machines I destroyed
	int otherMachinesDestroyed() const;
	void otherMachineDestroyed();
	
	//total of all machines I destroyed by RACE
	int raceMachinesDestroyed( MachPhys::Race ) const;
	void raceMachineDestroyed( MachPhys::Race );
	
	//total of all my machines destroyed by another race, by RACE
	int raceMyMachinesDestroyed( MachPhys::Race ) const;
	void raceMyMachineDestroyed( MachPhys::Race );

	int myConstructionsDestroyed() const;
	void myConstructionDestroyed();

	int otherConstructionsDestroyed() const;
	void otherConstructionDestroyed();

	//total of all constructions I destroyed by RACE
	int raceConstructionsDestroyed( MachPhys::Race ) const;
	void raceConstructionDestroyed( MachPhys::Race );
	
	//total of all my constructions destroyed by another race, by RACE
	int raceMyConstructionsDestroyed( MachPhys::Race ) const;
	void raceMyConstructionDestroyed( MachPhys::Race );

	int itemsResearched() const;
	void itemResearched();

	int totalResearchCost() const;
	void totalResearchIncreased( int increase );

	MachPhys::BuildingMaterialUnits	BMUsMined() const;
	void BMUsMinedIncreased( MachPhys::BuildingMaterialUnits increase );

	int grossScore() const;
	void changeGrossScore( int difference );
	
	void actorDestroyedNoCulprit( int value, const MachActor& victimActor );
	void actorDestroyedByRace( int value, const MachActor& victimActor, MachPhys::Race destroyingRace );
	void destroyedAnotherRacesActor( int value, const MachActor& victimActor, MachPhys::Race victimRace );

	//Value which will be used by the debrief to determine if a score should be displayed on screen.
	//defaults to true.
	bool scoreShouldBeDisplayed() const;
	void scoreShouldBeDisplayed( bool );

	//used to reset the score object to zero.
	//Not normally called except if a score is being supressed
	//The race and the displayed flag are RETAINED though
	void resetNumbers();

	//reset the array based numbers of this score object
	void resetPartialNumbers( MachPhys::Race );

private:

	enum KillerOrVictim{ KILLER, VICTIM };
	
	void adjustStatsForDestruction( KillerOrVictim killVictFlag, const MachActor& victimActor );
	
    friend ostream& operator <<( ostream& o, const MachLogScore& t );
	friend class MachLogMessageBroker;

    MachLogScore( const MachLogScore& );
    MachLogScore& operator =( const MachLogScore& );

	void assignScoreFromImpl( const MachLogScoreImpl& ) const;

	MachLogScoreImpl*		pImpl_;
};

PER_DECLARE_PERSISTENT( MachLogScore );

#endif

/* End SCORE.HPP ****************************************************/
