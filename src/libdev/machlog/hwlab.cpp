/*
 * H W L A B . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions
#include "stdlib/string.hpp"

#include "ctl/list.hpp"

#include "mathex/transf3d.hpp"
#include "mathex/abox2d.hpp"
#include "mathex/eulerang.hpp"

#include "phys/phys.hpp"
#include "phys/motchunk.hpp"

#include "world4d/domain.hpp"

#include "sim/manager.hpp"

#include "machphys/hwlab.hpp"
#include "machphys/harddata.hpp"
#include "machphys/mphydata.hpp"
#include "machphys/stations.hpp"
#include "machphys/techdata.hpp"

#include "machlog/hwlab.hpp"
#include "machlog/internal/hwlabi.hpp"
#include "machlog/machlog.hpp"
#include "machlog/hwlab.hpp"
#include "machlog/planet.hpp"
#include "machlog/plandoms.hpp"
#include "machlog/race.hpp"
#include "machlog/races.hpp"
#include "machlog/restree.hpp"
#include "machlog/resitem.hpp"
#include "machlog/technici.hpp"
#include "machlog/vmman.hpp"
#include "machlog/vmdata.hpp"
#include "machlog/animres.hpp"
#include "machlog/mcmotseq.hpp"

#include "machlog/network.hpp"
#include "machlog/messbrok.hpp"
#include "machlog/cntrl.hpp"

#ifndef _INLINE
    #include "machlog/hwlab.ipp"
#endif

PER_DEFINE_PERSISTENT( MachLogHardwareLab );
#define CB_MachLogHardwareLab_DEPIMPL()	\
	PRE( pImpl_ )	\
	CB_DEPIMPL( MachLogResearchTree::ResearchItems 	,availableResearchItems_ ) \
	CB_DEPIMPL( MachLogResearchTree::ResearchItems 	,currentResearchQueue_ ) \
	CB_DEPIMPL( PhysAbsoluteTime					,lastUpdateTime_ ) \
	CB_DEPIMPL( MachPhys::HardwareLabSubType 		,subType_ );

//////////////////////////////////////////////////////////////////////////////////////////

MachLogHardwareLab::MachLogHardwareLab( MachPhys::HardwareLabSubType subType, MachLogRace* pRace, uint level,
                    const MexPoint3d& location, const MexRadians& angle )
:   MachLogConstruction( pRace, pNewPhysHardwareLab( subType, pRace, level, location, angle ),
						 MachLog::HARDWARE_LAB,
                         MachPhysData::instance().hardwareLabData( subType, level ) ),
	pImpl_( _NEW( MachLogHardwareLabImpl( subType ) ) )
{

    TEST_INVARIANT;
	CB_MachLogHardwareLab_DEPIMPL();
    availableResearchItems_.reserve( 10 );

	//do not set the hp here
	//hp( data().hitPoints() );
	armour( data().armour() );
	MachLogRaces::instance().hardwareLabs( pRace->race() ).push_back( this );
}

MachLogHardwareLab::MachLogHardwareLab( MachPhys::HardwareLabSubType subType, MachLogRace* pRace, uint level,
                    const MexPoint3d& location, const MexRadians& angle, UtlId withId )
:   MachLogConstruction( pRace, pNewPhysHardwareLab( subType, pRace, level, location, angle ),
						 MachLog::HARDWARE_LAB,
						 withId,
                         MachPhysData::instance().hardwareLabData( subType, level ) ),
	pImpl_( _NEW( MachLogHardwareLabImpl( subType ) ) )
{

	CB_MachLogHardwareLab_DEPIMPL();
    availableResearchItems_.reserve( 10 );

    TEST_INVARIANT;
	//do not set the hp here
	//hp( data().hitPoints() );
	armour( data().armour() );
	MachLogRaces::instance().hardwareLabs( pRace->race() ).push_back( this );
}
//////////////////////////////////////////////////////////////////////////////////////////

MachLogHardwareLab::~MachLogHardwareLab()
{
	CB_MachLogHardwareLab_DEPIMPL();
	HAL_STREAM("(" << id() << ") MLHWLab::DTOR erasing available  list and currentlist\n" );
	while( availableResearchItems_.size() > 0 )
		availableResearchItems_.erase( availableResearchItems_.begin() );

	while( currentResearchQueue_.size() > 0 )
		currentResearchQueue_.erase( currentResearchQueue_.begin() );

    TEST_INVARIANT;
}
//////////////////////////////////////////////////////////////////////////////////////////

void MachLogHardwareLab::CLASS_INVARIANT
{
	INVARIANT( this != NULL );
}
//////////////////////////////////////////////////////////////////////////////////////////
void MachLogHardwareLab::doOutputOperator( ostream& o ) const
{
	CB_MachLogHardwareLab_DEPIMPL();
	MachLogConstruction::doOutputOperator( o );
    o << "MachLogHardwareLab " << (void*)this << std::endl;
	o << " MLRT.size() " << MachLogRaces::instance().researchTree().researchItems().size() << std::endl;
	o << " availableResearchItems_.size " << availableResearchItems_.size() << std::endl;
	if( availableResearchItems_.size() )
	{
		for( MachLogResearchTree::ResearchItems::iterator i = availableResearchItems_.begin(); i < availableResearchItems_.end(); ++i )
			o << (*i)->objectType() << " sub " << (*i)->subType() << " hw " << (*i)->hwLevel() << " wc " << (*i)->weaponCombo() << std::endl;
	}
	o << " CurrentResearchQueue_.size " << currentResearchQueue_.size() << std::endl;
	MachLogResearchItem* pRi = NULL;
	if( currentlyResearching( &pRi ) )
	{
		o << " Current Research " << pRi->objectType() << " sub" << pRi->subType() << " hw " << pRi->hwLevel() << " wc " << pRi->weaponCombo() << " tech " << pRi->technologyLevel() << std::endl;
		o << " amount researched " << pRi->amountResearched( race() ) << "/" << pRi->researchCost() << " built " << pRi->amountBuilt( race() ) << "/" << pRi->buildingCost() << std::endl;
	}
	else
		o << " Not researching.\n";
}

ostream& operator <<( ostream& o, const MachLogHardwareLab& t )
{
	t.doOutputOperator( o );
    return o;
}
//////////////////////////////////////////////////////////////////////////////////////////
//static
MachPhysHardwareLab* MachLogHardwareLab::pNewPhysHardwareLab
(
    MachPhys::HardwareLabSubType subType, MachLogRace* pRace, uint level, const MexPoint3d& location, const MexRadians& zAngle
)
{
    //get domain and transform to use
    MexTransform3d localTransform;
    W4dDomain* pDomain =
        MachLogPlanetDomains::pDomainPosition( location, zAngle, &localTransform );

    //Construct the smelter
    return _NEW( MachPhysHardwareLab( pDomain, localTransform, subType, level, pRace->race() ) );
}
//////////////////////////////////////////////////////////////////////////////////////////

//virtual
PhysRelativeTime MachLogHardwareLab::update( const PhysRelativeTime& alteredMaxCPUTime, MATHEX_SCALAR junk )
{
	CB_MachLogHardwareLab_DEPIMPL();
    bool complete = isComplete();
	PhysAbsoluteTime timeNow = SimManager::instance().currentTime();

	PhysRelativeTime interval = 10.0;

    if( not updateCompletionVisualisation() )
	{
		ensureLabAnimationIsOff();
		interval = 0.1;
	}

	if( complete )
	{
		if( not currentResearchQueue_.empty() )
		{
			//HAL_STREAM("(" << id() << ") MLHardwareLab::update...have something in currentReserachQueue\n" );

			MachLogResearchItem* pRi = currentResearchQueue_.front();

			float researchRate = totalResearchRate();

			int availableResearchUnits = (int)( ( researchRate / 60.0 ) * ( timeNow - lastUpdateTime_ ) );

			// if there's at least one tech (if no techs, researchrate = 0), do the lab animation
			// note that this is independent of whether any units have been added this turn
			// (i.e. it's callback-time-independent)
			if( researchRate > 0 )
			{
				ensureLabAnimationIsOn();
			}
			else
			{
				ensureLabAnimationIsOff();
			}

			if( availableResearchUnits > 0 )
			{
				lastUpdateTime_ = timeNow;

				int costToAddResearch = pRi->costToAddResearch( availableResearchUnits );
				MachLogRaces& races = MachLogRaces::instance();
				MachPhys::Race myRace = race();

				if( costToAddResearch > races.nBuildingMaterialUnits( myRace ) );
				{
					// okay, we CAN afford to add the research

					races.smartSubtractBMUs( myRace, costToAddResearch );

					pRi->advanceResearch( myRace, availableResearchUnits );

					races.totalResearchScoreAdjust( myRace, availableResearchUnits );

					// check for research now being complete - if so, issue voicemail and boot it off the queue
					if( pRi->researched( myRace ) )
					{
						MachLogVoiceMailManager::instance().postNewMail( VID_TECHNICIAN_RESEARCH_COMPLETE, id(), myRace );
						if( currentResearchQueue_.size() > 1 )
							MachLogVoiceMailManager::instance().postNewMail( VID_TECHNICIAN_RESEARCHING_NEXT, id(), myRace );

						races.newItemResearchedScoreAdjust( myRace );
					}
					//Auto populate research items queue - this should be done by the handle idle technicians
					//But the idleness is not a good measure to use when you are looking at the small "single" person labs
					//as timing will cause the research queue not to be extended.
					if( races.controller( myRace ).type() == MachLogController::AI_CONTROLLER )
						for( MachLogResearchTree::ResearchItems::iterator i = availableResearchItems().begin(); i != availableResearchItems().end(); ++i )
							addResearchItem( (**i) );
				}
			}

			// now, need to flush queue of any items researched.
			bool completedSweepWithNothingFound = false;
			bool erasedItems = false;

			while( not completedSweepWithNothingFound )
			{
				MachLogResearchTree::ResearchItems::iterator iFound = currentResearchQueue_.end();

				for( MachLogResearchTree::ResearchItems::iterator i = currentResearchQueue_.begin();
																  i != currentResearchQueue_.end();
																  ++i )
				{
					if( (*i)->researched( race() ) )
					{
						iFound = i;
						break;
					}
				}
				if( iFound != currentResearchQueue_.end() )
				{
					// note that iterator will still be in the correct place
					currentResearchQueue_.erase( iFound );
					erasedItems = true;
				}
				else
					completedSweepWithNothingFound = true;
			}

			// If anything was erased, broadcast notification of research completion
			if( erasedItems )
				notifyObservers( CLIENT_SPECIFIC, MachLog::HW_RESEARCH_COMPLETE );

		}// END if( not currentResearchQueue_.empty() )
		else
		{
			ensureLabAnimationIsOff();
			lastUpdateTime_ = timeNow;
		}
	}
	else
		// lab not yet built - update lastUpdateTime to timeNow so that the lab, when completed, will not receive
		// research points from the time period before it was completed
		lastUpdateTime_ = timeNow;


	MachActor::update( alteredMaxCPUTime, junk );

    return interval;
}

float MachLogHardwareLab::totalResearchRate() const
{
	CB_MachLogHardwareLab_DEPIMPL();

	float availableResearchUnits = 0;
    int nResearchStationsFree = constructionData().stations().nStations();

	for( MachLogRaces::Technicians::const_iterator i = MachLogRaces::instance().technicians( race() ).begin();
		i != MachLogRaces::instance().technicians( race() ).end(); ++i)
	if( (*i)->insideBuilding() and (*i)->insideWhichBuilding().id() == id() and
			&(*i)->motionSeq().currentConfigSpace() == &interiorConfigSpace() )
	{
		if( (*i)->isIdle() )
			(*i)->newOperation( _NEW( MachLogResearchAnimation( *i ) ) );

		float researchContributed = 0;

		if( nResearchStationsFree )
		{
			researchContributed = (*i)->data().researchRate();
			--nResearchStationsFree;
		}
		else
			researchContributed = _STATIC_CAST( float, (*i)->data().researchRate() ) / 5.0;

		availableResearchUnits += researchContributed;
	}

	// 10% bonus for level 3 labs, 20% bonus for level 5 labs
	availableResearchUnits *= ( 1.0 + ( ( level()-1 ) * 0.05 ) );

	return availableResearchUnits;
}


//////////////////////////////////////////////////////////////////////////////////////////

MachPhysHardwareLab* MachLogHardwareLab::pPhysHardwareLab()
{
    return (MachPhysHardwareLab*)( &physObject() );
}

const MachPhysHardwareLab* MachLogHardwareLab::pPhysHardwareLab() const
{
    return (MachPhysHardwareLab*)( &physObject() );
}
//////////////////////////////////////////////////////////////////////////////////////////

//virtual
const MachPhysConstructionData& MachLogHardwareLab::constructionData() const
{
	return data();
}

const MachPhysHardwareLabData& MachLogHardwareLab::data() const
{
	return pPhysHardwareLab()->data();
}

MachLogResearchTree::ResearchItems& MachLogHardwareLab::availableResearchItems()
{
	//HAL_STREAM("(" << id() << ") MLHardwareLab::availableResearchItems\n" );
	CB_MachLogHardwareLab_DEPIMPL();
	if( availableResearchItems_.size() > 0 )
		return availableResearchItems_;
	initialiseResearchItems();
	return availableResearchItems_;
}

void MachLogHardwareLab::initialiseResearchItems()
{
	CB_MachLogHardwareLab_DEPIMPL();
	//HAL_STREAM("(" << id() << ") MLHardwareLab::initialiseResearchItems\n" );
	int highestTechLevel = 0;
	for( MachLogRaces::Technicians::const_iterator j = MachLogRaces::instance().technicians( race() ).begin();
		j != MachLogRaces::instance().technicians( race() ).end(); ++j )
			if( (*j)->technologyLevel() > highestTechLevel )
				highestTechLevel = (*j)->technologyLevel();

	//HAL_STREAM(" highest technology level for race " << (int)race() << " is " << highestTechLevel << std::endl );
	while( availableResearchItems_.size() > 0 )
		availableResearchItems_.erase( availableResearchItems_.begin() );

	const MachLogResearchTree::ResearchItems& items = MachLogRaces::instance().researchTree().researchItems();
	//HAL_STREAM(" scanning the tree for stuff. no items " << items.size() << std::endl );
	for( MachLogResearchTree::ResearchItems::const_iterator i = items.begin(); i!=items.end(); ++i)
	{
		MachLogResearchItem* pRi = *i;
		if( pRi->available( race() ) and not pRi->researched( race() ) and pRi->technologyLevel() <= highestTechLevel and subType_ == pRi->hardwareLabSubType() )
		{
			if( pRi->hasParentDependancy() )
			{
				if( pRi->parentDependancy().researched( race() ) )
					availableResearchItems_.push_back( pRi );
			}
			else
				availableResearchItems_.push_back( pRi );
		}
	}
}

MachLogResearchTree::ResearchItems& MachLogHardwareLab::currentResearchQueue()
{
	CB_MachLogHardwareLab_DEPIMPL();
	return currentResearchQueue_;
}

bool MachLogHardwareLab::currentlyResearching( MachLogResearchItem** ppMLRI ) const
{
	CB_MachLogHardwareLab_DEPIMPL();
	if( currentResearchQueue_.size() )
	{
		*ppMLRI = currentResearchQueue_.front();
		return true;
	}
	*ppMLRI = NULL;
	return false;
}

bool MachLogHardwareLab::addResearchItem( const MachLogResearchItem& mlri )
{
	//HAL_STREAM("(" << id() << ") MLHardwareLab::addResearchItem item is\n" << mlri.objectType() << " sub " << mlri.subType() << " hw " << mlri.hwLevel() << std::endl );
	CB_MachLogHardwareLab_DEPIMPL();
	bool found = false;
	for( MachLogResearchTree::ResearchItems::iterator i = availableResearchItems_.begin();
		i != availableResearchItems_.end() and not found; ++i)
		if( (*i)->objectType() == mlri.objectType() and
			(*i)->subType() == mlri.subType() and
			(*i)->hwLevel() == mlri.hwLevel() and
			(*i)->weaponCombo() == mlri.weaponCombo() )
		{
			//HAL_STREAM(" found a match in the available items list\n" );
			bool foundExistingEntry = false;
			for( MachLogResearchTree::ResearchItems::iterator j = currentResearchQueue_.begin();
				j != currentResearchQueue_.end(); ++j )
				if( (*j)->objectType() == mlri.objectType() and
					(*j)->subType() == mlri.subType() and
					(*j)->hwLevel() == mlri.hwLevel() and
					(*j)->weaponCombo() == mlri.weaponCombo() )
					foundExistingEntry = true;
			if( not foundExistingEntry )
			{
				found = true;
				currentResearchQueue_.push_back( (*i) );
			}
		}

	// voicemails - firstly, only give one if the research queue was previously	empty (it will now be size 1)
	if( found )
		if( currentResearchQueue_.size() == 1 )
		{
			// The voicemail will vary depending on whether or not there are actually technicians in the lab.
			// we'll actually use and cache the total research rate value for efficiency - if it's 0, there's no technicians,
			// if more than 0, we can use the value to determine which of two subpossibility voicemails to play.
			float researchRate = totalResearchRate();

		   	if( researchRate > 0 )
			{
				// at least one technician is in the lab.
				// Now, is this a standard piece of research or a particularly tricky one?
				if( (float)mlri.researchCost()/researchRate > 4.0 )
					MachLogVoiceMailManager::instance().postNewMail( VID_TECHNICIAN_RESEARCHING_TOUGH_ONE, id(), race() );
				else
					MachLogVoiceMailManager::instance().postNewMail( VID_TECHNICIAN_RESEARCHING, id(), race() );
			}
			else
			{
				// We may need to inform the punter that there's currently no techs inside this lab. HOWEVER,
				// suppress this if techs have already been tasked to enter this building and are on their way.

				bool noTechsOnWayEncountered = true;

				const MachLogRaces::Technicians& myTechs = MachLogRaces::instance().technicians( race() );

				for( MachLogRaces::Technicians::const_iterator i = myTechs.begin();
					 noTechsOnWayEncountered and i != myTechs.end();
					 ++i )
				{
					if( (*i)->isEnteringBuilding( *this ) )
					{
						noTechsOnWayEncountered = false;
					}
				}

				if( noTechsOnWayEncountered )
				{
					// give the mail
					MachLogVoiceMailManager::instance().postNewMail( VID_POD_NO_TECHNICIANS, race() );
				}
			}
		}

	return found;
}

void MachLogHardwareLab::removeResearchItem( const MachLogResearchItem& mlri )
{
	CB_MachLogHardwareLab_DEPIMPL();

	bool found = false;

	MachLogResearchTree::ResearchItems::iterator i = find( currentResearchQueue_.begin(), currentResearchQueue_.end(), &mlri );

	ASSERT( i != currentResearchQueue_.end(), "Research item was not found!" );

	currentResearchQueue_.erase( i );
}

void MachLogHardwareLab::cancelAllResearch()
{
	CB_MachLogHardwareLab_DEPIMPL();

	currentResearchQueue_.erase( currentResearchQueue_.begin(), currentResearchQueue_.end() );
}

void MachLogHardwareLab::moveResearchItem( const MachLogResearchItem& mlri,
										 MachLogHardwareLab::MoveDirection direction)
{
	CB_MachLogHardwareLab_DEPIMPL();
	MachLogResearchTree::ResearchItems::iterator tempIterator = currentResearchQueue_.end();
	--tempIterator;
	for( MachLogResearchTree::ResearchItems::iterator i = currentResearchQueue_.begin();
		i != currentResearchQueue_.end(); ++i )
		if( (*i) == &mlri )
		{
//			_DELETE( (*i) );
			if( direction == TOWARDS_FRONT and i == currentResearchQueue_.begin() )
				return;
			if( direction == TOWARDS_BACK and i == tempIterator )
				return;

			MachLogResearchTree::ResearchItems::iterator insertIterator = i;
			switch( direction )
			{
			case TOWARDS_FRONT:
				--insertIterator;
				currentResearchQueue_.insert( insertIterator, &_CONST_CAST( MachLogResearchItem&, mlri ) );
				currentResearchQueue_.erase( i );
				break;

			case TOWARDS_BACK:
				insertIterator++;
				insertIterator++;
				currentResearchQueue_.insert( insertIterator, &_CONST_CAST( MachLogResearchItem&, mlri ) );
				currentResearchQueue_.erase( i );
				break;
			default:
				ASSERT_BAD_CASE;
			}
		}
}

MachPhys::BuildingMaterialUnits MachLogHardwareLab::amountBuilt() const
{
	CB_MachLogHardwareLab_DEPIMPL();
	if( currentResearchQueue_.size() > 0 )
	{
		return currentResearchQueue_.front()->amountBuilt( race() );
	}
	return 0;
}

MachPhys::ResearchUnits MachLogHardwareLab::amountResearched() const
{
	CB_MachLogHardwareLab_DEPIMPL();
	if( currentResearchQueue_.size() > 0 )
	{
		return currentResearchQueue_.front()->amountResearched( race() );
	}
	return 0;
}

const MachPhys::HardwareLabSubType MachLogHardwareLab::subType() const
{
	CB_MachLogHardwareLab_DEPIMPL();
	return subType_;
}

void perWrite( PerOstream& ostr, const MachLogHardwareLab& actor )
{
	const MachLogConstruction& base1 = actor;

	ostr << base1;
	ostr << actor.pImpl_;

	const MexTransform3d& trans = actor.globalTransform();
	PER_WRITE_RAW_OBJECT( ostr, trans );
	const W4dId id = actor.id();
	PER_WRITE_RAW_OBJECT( ostr, id );
	uint level = actor.level();
	PER_WRITE_RAW_OBJECT( ostr, level );
	bool isWorking = actor.pPhysHardwareLab()->isWorking();
	PER_WRITE_RAW_OBJECT( ostr, isWorking );

	actor.persistenceWriteStrategy( ostr );
}

void perRead( PerIstream& istr, MachLogHardwareLab& actor )
{
	MachLogConstruction& base1 = actor;

	istr >> base1;
	istr >> actor.pImpl_;

	MexTransform3d trans;
	W4dId id;
	uint level;
	bool isWorking;
	PER_READ_RAW_OBJECT( istr, trans );
	PER_READ_RAW_OBJECT( istr, id );
	PER_READ_RAW_OBJECT( istr, level );
	PER_READ_RAW_OBJECT( istr, isWorking );

	MachPhysHardwareLab* pPhysHardwareLab = MachLogHardwareLab::pNewPhysHardwareLab( (MachPhys::HardwareLabSubType)actor.subType(), &actor.logRace(), level, trans.position(), trans.rotationAsEulerAngles().azimuth() );
	actor.setObjectPtr( pPhysHardwareLab, trans );
	actor.id( id );
	actor.constructionCreated( pPhysHardwareLab );
	MachLogRaces::instance().nConstructions( actor.race() )++;
	pPhysHardwareLab->isWorking( isWorking );
	actor.persistenceReadStrategy( istr );
}

MachLogHardwareLab::MachLogHardwareLab( PerConstructor con )
:	MachLogConstruction( con )
{
}

void MachLogHardwareLab::ensureLabAnimationIsOn()
{
	if( not pPhysHardwareLab()->isWorking() )
	{
		pPhysHardwareLab()->isWorking( true );
		if( MachLogNetwork::instance().isNetworkGame() )
			MachLogNetwork::instance().messageBroker().sendPlayNormalObjectAnimationMessage( id(), true );
	}
}

void MachLogHardwareLab::ensureLabAnimationIsOff()
{
	if( pPhysHardwareLab()->isWorking() )
	{
		pPhysHardwareLab()->isWorking( false );
		if( MachLogNetwork::instance().isNetworkGame() )
			MachLogNetwork::instance().messageBroker().sendPlayNormalObjectAnimationMessage( id(), false );
	}

}


//////////////////////////////////////////////////////////////////////////////////////////
/* End HWLAB.CPP **************************************************/
