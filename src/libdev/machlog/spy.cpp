/*
 * S P Y . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved.
 */

#include "stdlib/string.hpp"

#include "ctl/algorith.hpp"
#include "ctl/pvector.hpp"
#include "ctl/list.hpp"

#include "mathex/transf3d.hpp"
#include "mathex/point2d.hpp"

#include "world4d/domain.hpp"
#include "world4d/soundman.hpp"

#include "sim/manager.hpp"

#include "machphys/random.hpp"
#include "machphys/spylocat.hpp"
#include "machphys/slocdata.hpp"
#include "machphys/snddata.hpp"
#include "machphys/mphydata.hpp"

#include "machlog/constron.hpp"
#include "machlog/spy.hpp"
#include "machlog/planet.hpp"
#include "machlog/plandoms.hpp"
#include "machlog/race.hpp"
#include "machlog/races.hpp"
#include "machlog/resitem.hpp"
#include "machlog/restree.hpp"
#include "machlog/lmine.hpp"
#include "machlog/vmman.hpp"
#include "machlog/vmdata.hpp"
#include "machlog/mcmotseq.hpp"

PER_DEFINE_PERSISTENT( MachLogSpyLocator );

/* //////////////////////////////////////////////////////////////// */

MachLogSpyLocator::MachLogSpyLocator( MachLogMachine::Level hwLevel, MachLogMachine::Level swLevel,
    								MachLogRace * pRace, const MexPoint3d& location )
: MachLogMachine( MachPhys::SPY_LOCATOR, hwLevel, swLevel, pRace,
                  pNewPhysSpyLocator( hwLevel, swLevel, pRace, location ),
                  MachLog::SPY_LOCATOR ),
	lastUpdateTime_( SimManager::instance().currentTime() ),
	pResItemCurrentlyStealing_( NULL ),
	isDownloading_( false ),
	nMines_( MachPhysData::instance().spyLocatorData( hwLevel, swLevel ).mineCapacity() )
{
	hp( data().hitPoints() );
	armour( data().armour() );
	ctl_append( &MachLogRaces::instance().spyLocators( pRace->race() ), this );
}

MachLogSpyLocator::MachLogSpyLocator( MachLogMachine::Level hwLevel, MachLogMachine::Level swLevel,
    								MachLogRace * pRace, const MexPoint3d& location, UtlId withId )
: MachLogMachine( MachPhys::SPY_LOCATOR, hwLevel, swLevel, pRace,
                  pNewPhysSpyLocator( hwLevel, swLevel, pRace, location ),
                  MachLog::SPY_LOCATOR,
                  withId ),
	lastUpdateTime_( SimManager::instance().currentTime() ),
	pResItemCurrentlyStealing_( NULL ),
	isDownloading_( false ),
	nMines_( MachPhysData::instance().spyLocatorData( hwLevel, swLevel ).mineCapacity() )
{
	hp( data().hitPoints() );
	armour( data().armour() );
	ctl_append( &MachLogRaces::instance().spyLocators( pRace->race() ), this );
}

MachLogSpyLocator::~MachLogSpyLocator()
{
	/* Intentionally Empty	*/
}

/* //////////////////////////////////////////////////////////////// */

void MachLogSpyLocator::doOutputOperator( ostream& o ) const
{
	MachLogMachine::doOutputOperator( o );
	o << " Number mines left " << nMines_ << std::endl;
	o << " Stealing enemy tech? "<< isDownloading() << std::endl;
	if( isDownloading() )
	{
		o << " Stealing tech " << pResItemCurrentlyStealing_->objectType();
		o << " subtype " << pResItemCurrentlyStealing_->subType();
		o << " hardware level " << pResItemCurrentlyStealing_->hwLevel();
	}

}

/* //////////////////////////////////////////////////////////////// */
//static
MachPhysSpyLocator* MachLogSpyLocator::pNewPhysSpyLocator
(
    Level hwLevel, Level swLevel, MachLogRace * pRace, const MexPoint3d& location
)
{
    //get domain and transform to use
    MexTransform3d localTransform;
    W4dDomain* pDomain =
        MachLogPlanetDomains::pDomainPosition( location, 0, &localTransform );

    //Construct the physical machine
    return _NEW( MachPhysSpyLocator( pDomain, localTransform, hwLevel, swLevel, pRace->race() ) );
}
/* //////////////////////////////////////////////////////////////// */

MachPhysSpyLocator& MachLogSpyLocator::physSpyLocator()
{
    return _STATIC_CAST( MachPhysSpyLocator&, physObject() );
}
//////////////////////////////////////////////////////////////////////////////////////////

const MachPhysSpyLocator& MachLogSpyLocator::physSpyLocator() const
{
    return _STATIC_CAST( const MachPhysSpyLocator&, physObject() );
}
//////////////////////////////////////////////////////////////////////////////////////////

//virtual
const MachPhysMachineData& MachLogSpyLocator::machineData() const
{
	return data();
}

//////////////////////////////////////////////////////////////////////////////////////////

//virtual
PhysRelativeTime MachLogSpyLocator::update( const PhysRelativeTime& maxCPUTime,
                                      		MATHEX_SCALAR clearance )
{
	PhysRelativeTime interval = MachLogMachine::update( maxCPUTime, clearance );

	if(	insideBuilding()
		and insideWhichBuilding().race()!=race() and insideWhichBuilding().objectType() == MachLog::HARDWARE_LAB
		and not motionSeq().is1stPersonControlled()
		and isIdle() )
	{
		executeTheft();
	}


    return interval;
}


//////////////////////////////////////////////////////////////////////////////////////////

const MachPhysSpyLocatorData& MachLogSpyLocator::data() const
{
	//return  _STATIC_CAST( MachPhysSpyLocatorData&, physMachine().machineData() );
	return  _STATIC_CAST( const MachPhysSpyLocatorData&, physMachine().machineData() );
}

int	MachLogSpyLocator::nMines()	const
{
	return nMines_;
}

//////////////////////////////////////////////////////////////////////////////////////////

void MachLogSpyLocator::placeMine()
{
	ASSERT( nMines_ > 0, "MLSpyLocator:: Run out of mines but still tried to place one\n" );
	_NEW( MachLogLandMine( &logRace(), position() ) );

	W4dEntity& physObj = physObject();
	W4dSoundManager::instance().play(&physObj, SID_MINE_DROP, PhysAbsoluteTime(0), 1);

	--nMines_;

	// If no more mines left, send voicemail, and send notification
	if( nMines_ == 0 )
	{
		MachLogVoiceMailManager::instance().postNewMail( VID_SPY_NO_MORE_MINES, id(), race() );
		notifyObservers( W4dSubject::CLIENT_SPECIFIC, MachLog::WEAPON_AVAILABILITY_CHANGED );
	}

	// bit of a kludge - forces GUI to update selected actor status levels, which includes
	// the number of mines carried currently
	notifyObservers( W4dSubject::CLIENT_SPECIFIC, MachLog::HEALTH_STATUS_CHANGED );
}

//////////////////////////////////////////////////////////////////////////////////////////

void MachLogSpyLocator::restockMines()
{
	nMines_ = data().mineCapacity();
	notifyObservers( W4dSubject::CLIENT_SPECIFIC, MachLog::WEAPON_AVAILABILITY_CHANGED );
}


//////////////////////////////////////////////////////////////////////////////////////////

void MachLogSpyLocator::executeTheft()
{
	HAL_STREAM("(" << id() << ") MachLogSpy::executeTheft\n" );
	HAL_INDENT( 2 );
	PRE ( insideBuilding() );
	PRE (  insideWhichBuilding().objectType() == MachLog::HARDWARE_LAB );

	// Am I already downloading something? If not, try and find something to download.

	if (not isDownloading()) 					// i.e. If we're currently stealing nothing
		pResItemCurrentlyStealing_ = stealNewResearchItem( insideWhichBuilding().race(), (MachPhys::HardwareLabSubType)insideWhichBuilding().subType() ); 	// Attempt to acquire a new item to steal

	// Now, are we in the downloading state, possibly after having acquired a legitimate res item to start stealing?

	if (isDownloading())
	{
		ASSERT ( pResItemCurrentlyStealing_ != NULL, "MachLogSpyLocator::executeTheft - isDownloading() == TRUE yet null pointer assignment for research item pointer pResItemCurrentlyStealing_" );

		int potentialResearchUnits = (int)( (float)data().downloadRate() / 60.0 * ( SimManager::instance().currentTime() - lastUpdateTime_ ) );
		if (potentialResearchUnits > 0)

		{
			HAL_STREAM("going to advanceResearchItem " << (void*)pResItemCurrentlyStealing_ << " by " << potentialResearchUnits << std::endl );
	  		pResItemCurrentlyStealing_->advanceResearch( race(), potentialResearchUnits );

			// Have I now finished stealing this research?
			if (pResItemCurrentlyStealing_->researched( race() ) )
			{
				HAL_STREAM("research item is now researched - set everything off\n" );
				pResItemCurrentlyStealing_ = NULL;
				isDownloading( false );

				// !! And now need to send avoicemail notifying them of the successful theft.
				MachLogVoiceMailManager::instance().postNewMail( VID_SPY_RESEARCH_DOWNLOADED, id(), race() );
			}

			lastUpdateTime_ = SimManager::instance().currentTime();
		}
		// else potentialResearchUnits must have == 0, and the difference between "now" and "the last time I was updated" is
		//									allowed to accumulate.)

	}

	// End  if (isDownloading())			Do nothing if nothing to steal.

	HAL_INDENT( -2 );
	HAL_STREAM("(" << id() << ") MachLogSpy::executeTheft DONE\n" );
}

//////////////////////////////////////////////////////////////////////////////////////////

int MachLogSpyLocator::nResItemsICouldSteal( MachPhys::Race otherRace , MachPhys::HardwareLabSubType hardwareLabSubType )
{
	int nStealableResItems = 0;

	for (MachLogResearchTree::ResearchItems::iterator i =  MachLogRaces::instance().researchTree().researchItems().begin();
			i != MachLogRaces::instance().researchTree().researchItems().end(); ++i)
	{
		if ( stealable( (**i), otherRace, hardwareLabSubType) )
			++nStealableResItems;
	}


	return nStealableResItems;
}

//////////////////////////////////////////////////////////////////////////////////////////

MachLogResearchItem* MachLogSpyLocator::stealNewResearchItem( MachPhys::Race otherRace, MachPhys::HardwareLabSubType hardwareLabSubType )
{

   	int nPossibleResItems = nResItemsICouldSteal( otherRace, hardwareLabSubType  );

	if (nPossibleResItems == 0)
	{
		isDownloading( false );
		return NULL;
	}

	int nResItemsExamined=0;

	ASSERT (nPossibleResItems > 0, "MachLogSpyLocator::stealNewResearchItem: The acquisition of a ResItem shouldn't be being made if the number of stealable items == 0" );

	// send voicemail informing that the download is about to begin
	MachLogVoiceMailManager::instance().postNewMail( VID_SPY_NEW_DOWNLOAD, id(), race() );

	for (MachLogResearchTree::ResearchItems::iterator i = MachLogRaces::instance().researchTree().researchItems().begin();
			i != MachLogRaces::instance().researchTree().researchItems().end(); ++i)
	{
		if ( stealable( (**i), otherRace, hardwareLabSubType ) )
		{
			// If there are e.g. five stealable techs, the chance of the first being chosen is 1/5, then if it isn't selected, the
			// second tech has a 1/4 chance etc. As 4/5 X 1/4 == 1/5, 4/5 X 3/4 X 1/3 == 1/5 etc, all techs have an equal
			// chance of being selected. The last eligible tech is clearly guaranteed (1/1) to be selected if none of the others were.

			if ( MachPhysRandom::randomDouble(0.0,1.0) <= 1.0/(double)(nPossibleResItems - nResItemsExamined) )
			{
				lastUpdateTime_ = SimManager::instance().currentTime();	// Download officially begins NOW.
				isDownloading( true );

				return (*i);
			}
			else
				++nResItemsExamined;
		}

	}

	// this code should never be reached
	ASSERT( false, "MachLogSpyLocator::stealResearchItem(): the probability function should ALWAYS be returning the last possible tech if it reaches it.");

	// Dummy return to satisfy compiler (should never be executed)
	return *MachLogRaces::instance().researchTree().researchItems().begin();
}

//////////////////////////////////////////////////////////////////////////////////////////

bool MachLogSpyLocator::stealable( const MachLogResearchItem& researchItem, MachPhys::Race otherRace, MachPhys::HardwareLabSubType hardwareLabSubType)
{
	// we may wish to alter this rather basic criteria later, for instance only
	// allowing us to steal a tech for which we already have the parent tech researched

	return (researchItem.researched ( otherRace )
			and researchItem.hardwareLabSubType() == hardwareLabSubType
			and researchItem.available( race() )
			and not researchItem.researched( race() ) );
}

//////////////////////////////////////////////////////////////////////////////////////////

void MachLogSpyLocator::isDownloading( bool newStatus )
{
	isDownloading_ = newStatus;
}

//////////////////////////////////////////////////////////////////////////////////////////

bool MachLogSpyLocator::fullyStockedMines() const
{
	return nMines_ == data().mineCapacity();
}

//////////////////////////////////////////////////////////////////////////////////////////

void perWrite( PerOstream& ostr, const MachLogSpyLocator& actor )
{
	const MachLogMachine& base1 = actor;

	ostr << base1;
	ostr << actor.nMines_;
	ostr << actor.lastUpdateTime_;
	ostr << actor.pResItemCurrentlyStealing_;
	ostr << actor.isDownloading_;

	MexTransform3d trans = actor.globalTransform();

    //If we don't do this, the large coords of the transform cause are used when read back in to find
    //a planet domain, which of course falls over big time.
    if( actor.insideAPC() )
        trans.position( MexPoint3d(10,10,0) );

	PER_WRITE_RAW_OBJECT( ostr, trans );
	const W4dId id = actor.id();
	PER_WRITE_RAW_OBJECT( ostr, id );
	actor.persistenceWriteStrategy( ostr );
}

void perRead( PerIstream& istr, MachLogSpyLocator& actor )
{
	MachLogMachine& base1 = actor;

	istr >> base1;
	istr >> actor.nMines_;
	istr >> actor.lastUpdateTime_;
	istr >> actor.pResItemCurrentlyStealing_;
	istr >> actor.isDownloading_;

	MexTransform3d trans;
	W4dId id;
	PER_READ_RAW_OBJECT( istr, trans );
	PER_READ_RAW_OBJECT( istr, id );

	MachPhysSpyLocator* pPhysSpyLocator = MachLogSpyLocator::pNewPhysSpyLocator( base1.hwLevel(), base1.swLevel(), &actor.logRace(), trans.position() );
	actor.setObjectPtr( pPhysSpyLocator, trans );
	actor.id( id );
	actor.machineCreated();
	actor.persistenceReadStrategy( istr );
}

MachLogSpyLocator::MachLogSpyLocator( PerConstructor con )
:	MachLogMachine( con )
{
}
//////////////////////////////////////////////////////////////////////////////////////////
/* End LOCATOR.CPP **************************************************/
