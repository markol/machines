/*
 * C N T R L _ A I  . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved.
 */

#include "base/base.hpp"

#include "stdlib/string.hpp"

#include "ctl/algorith.hpp"

#include "system/metafile.hpp"
#include "system/metaistr.hpp"
#include <memory>

#include "utility/linetok.hpp"

#include "mathex/cvexpgon.hpp"
#include "mathex/abox2d.hpp"

#include "phys/cspace2.hpp"
#include "phys/motchunk.hpp"

#include "sim/manager.hpp"

#include "machphys/mcmovinf.hpp"
#include "machphys/mphydata.hpp"
#include "machphys/minedata.hpp"
#include "machphys/random.hpp"
#include "machphys/station.hpp"
#include "machphys/stations.hpp"

#include "machlog/cntrl_ai.hpp"
#include "machlog/internal/cntrlaii.hpp"

#include "machlog/aggressr.hpp"
#include "machlog/administ.hpp"
#include "machlog/squad.hpp"
#include "machlog/geolocat.hpp"
#include "machlog/rescarr.hpp"
#include "machlog/technici.hpp"
#include "machlog/constree.hpp"
#include "machlog/construc.hpp"

#include "machlog/hwlab.hpp"
#include "machlog/inout.hpp"
#include "machlog/mine.hpp"
#include "machlog/minesite.hpp"
#include "machlog/planet.hpp"
#include "machlog/race.hpp"
#include "machlog/races.hpp"
#include "machlog/resitem.hpp"
#include "machlog/animres.hpp"
#include "machlog/produnit.hpp"
#include "machlog/scenario.hpp"
#include "machlog/optskloc.hpp"
#include "machlog/optskcon.hpp"
#include "machlog/optskatt.hpp"
#include "machlog/optskdrm.hpp"
#include "machlog/optskpat.hpp"
#include "machlog/move.hpp"
#include "machlog/oreholo.hpp"
#include "machlog/spacial.hpp"
#include "machlog/score.hpp"


/* //////////////////////////////////////////////////////////////// */
// comparison function used for sorting pointers to objects
#define CB_MachLogAIController_DEPIMPL()	\
	PRE( pImpl_ )	\
	CB_DEPIMPL( MachLogAIController::ResearchInterest		,researchInterest_) \
	CB_DEPIMPL( string										,rules_ ) \
	CB_DEPIMPL( MachLogAIController::StrategicProductionList,strategicProductionList_) \
	CB_DEPIMPL( MachLogAIController::ProductionList			,constructionProductionList_) \
	CB_DEPIMPL( MachLogAIController::DesiredMachineList		,desiredMachineList_ ) \
	CB_DEPIMPL( bool										,checkForDynamicAllies_ ) \
	CB_DEPIMPL( PhysAbsoluteTime							,nextAllyUpdateTime_ );

PER_DEFINE_PERSISTENT( MachLogAIController );
PER_DEFINE_PERSISTENT( DesiredMachineData );

MachLogAIController::MachLogAIController( MachLogRace* logRace, W4dEntity* pPhysObject, const string& AIStrategicRules )
  : MachLogController( logRace, pPhysObject, MachLogController::AI_CONTROLLER ),
	pImpl_( _NEW ( MachLogAIControllerImpl( AIStrategicRules ) ) )
{
	//the rules length CAN legitimately be zero length from the persistence mechanism.
	CB_MachLogAIController_DEPIMPL();
	checkForDynamicAllies_ = false;
	nextAllyUpdateTime_ = SimManager::instance().currentTime() + allyUpdateInterval();
	if( rules_.length() > 0 )
		readRules( SysPathName( rules_ ) );
}

MachLogAIController::~MachLogAIController()
{
	CB_MachLogAIController_DEPIMPL();
	while( desiredMachineList_.size() > 0 )
	{
		DesiredMachineData* dmd = desiredMachineList_.front();
		_DELETE( dmd->pProdUnit_ );
		_DELETE( dmd );
		desiredMachineList_.erase( desiredMachineList_.begin() );
	}
	while( constructionProductionList_.size() > 0 )
	{
		_DELETE( constructionProductionList_.front() );
		constructionProductionList_.erase( constructionProductionList_.begin() );
	}

	while( strategicProductionList_.size() > 0 )
	{
		_DELETE( strategicProductionList_.back() );
		strategicProductionList_.erase( strategicProductionList_.end() - 1 );
	}
	_DELETE( pImpl_ );
}

/* //////////////////////////////////////////////////////////////// */

static
bool mineSiteIsOccupied( const MexPoint3d& dest )
{
    //Now construct a motion chunk as if we had moved from nearPoint to location2d,
    //and add this
    PhysConfigSpace2d& cSpace = MachLogPlanet::instance().configSpace();

    PhysAbsoluteTime timeNow = SimManager::instance().currentTime();
	MexPoint2d location2d( dest );
	MexPoint2d nearPoint( location2d.x() + 0.01, location2d.y() + 0.01 );
    PhysMotionChunk motionChunk
    (
        nearPoint,
        location2d,
        5,			//TBD: get correct clearance out using some new spanky function.
        PhysRampAcceleration( 0.0, 400.0, -400.0, 0.01, 0.005, 0.005 ),
        PhysAbsoluteTime( timeNow - 0.01 ),
        PhysRelativeTime( 0.0 ),
        MexDouble::minusInfinity(),
        MexDouble::plusInfinity()
    );

    //Add the motion chunk to the space
    PhysRelativeTime collideTime;
    PhysConfigSpace2d::ObjectId collisionObjectId;
	PhysConfigSpace2d::MotionChunkId foundChunkId;
    bool found = cSpace.add( motionChunk, 0, &foundChunkId, &collisionObjectId, &collideTime );
	if( found )
	{
		cSpace.removeMotionChunk( foundChunkId );

		const MexAlignedBox2d& localBoundary = MachPhysData::instance().mineData( 1 ).localBoundary();//MachLogMine::constructionData().localBoundary();
		MexPoint2d lMinC( localBoundary.minCorner().x() + dest.x(), localBoundary.minCorner().y() + dest.y() );
		MexPoint2d lMaxC( localBoundary.maxCorner().x() + dest.x(), localBoundary.maxCorner().y() + dest.y() );
		MexAlignedBox2d globalBoundary( lMinC, lMaxC );
		MexConvexPolygon2d poly( globalBoundary );

		PhysConfigSpace2d::PolygonId junk;
		found = cSpace.contains( poly, MachLog::OBSTACLE_NORMAL, &junk, PhysConfigSpace2d::USE_ALL );
	}
	return not found;
}

void MachLogAIController::deleteHolographAtPosition( const MexPoint3d& pos )
{
	MachLogRaces::Holographs& holographs = MachLogRaces::instance().holographs( race() );
	MachLogRaces::Holographs::iterator i =  holographs.begin();
	for( ; i != holographs.end(); ++i )
	{
		if( (*i)->position() == pos )
		{
//			MachLogRaces::instance().remove( *i );
//			_DELETE( *i );
            //(*i)->isDead( true );
			holographs.erase( i );
			break;
		}
	}
}

PhysRelativeTime MachLogAIController::update( const PhysRelativeTime&, MATHEX_SCALAR )
{
	CB_MachLogAIController_DEPIMPL();
	PhysRelativeTime	interval = 15.0;

	MachLogRaces& races = MachLogRaces::instance();
	MachPhys::Race myRace = race();

	PhysAbsoluteTime timeNow = SimManager::instance().currentTime();

	if( checkForDynamicAllies_ and timeNow >= nextAllyUpdateTime_ and ( races.nMachines( myRace ) + races.nConstructions( myRace ) ) > 0 )
	{
		// note random element to update times - tries to avoid all computer races updating ally relationships at more
		// or less the same time
		nextAllyUpdateTime_ += allyUpdateInterval() + _STATIC_CAST( PhysRelativeTime, ( MachPhysRandom::randomInt( 120 ) - 60 ) );

		HAL_STREAM("MachLogAIController::update " << myRace << std::endl );
		MachPhys::Race highestScoreRace = MachPhys::RED;
		int highestScore = -10000000;
		int nRaces = 0;
		int myIndexInAIRacesList = 0;
		ctl_vector< MachPhys::Race > AIRaces;

		for( MachPhys::Race i = MachPhys::RED; i < MachPhys::N_RACES; ++((int&)i) )
		{
			if( races.raceInGame( i ) and ( races.nMachines( i ) + races.nConstructions( i ) ) > 0 )
			{
				++nRaces;
				if( races.controller( i ).type() == MachLogController::AI_CONTROLLER )
				{
					AIRaces.push_back( i );
					if( myRace == i )
					{
						myIndexInAIRacesList = AIRaces.size()-1;
					}
				}
				if( races.score( i ).grossScore() > highestScore )
				{
					highestScore = races.score( i ).grossScore();
					highestScoreRace = i;
				}
			}
		}

		//Are we winning? If so, we won't set up any ally relationships.
		if( myRace != highestScoreRace )
		{
			//If there are only two races then we won't set up any ally relationships.
			if( nRaces > 2 )
			{
				//Any other AI Races? 'Coz we're not going to set up any ally relationships with human players
				if( AIRaces.size() > 1 )
				{
					// I'm only responsible for setting up relationships with those races coming after me in the list
					for( int i = myIndexInAIRacesList + 1; i < AIRaces.size(); ++i )
					{
						MachPhys::Race otherAIRace = AIRaces[i];

						// first, set all AI races mutual dispositions to neutral pending possible updating
						races.dispositionToRace( otherAIRace, myRace, MachLogRaces::NEUTRAL );
						races.dispositionToRace( myRace, otherAIRace, MachLogRaces::NEUTRAL );

						int myScore = races.score( myRace ).grossScore();
						int otherAIRaceScore = races.score( otherAIRace ).grossScore();

						// if the other AI guy is actually winning, then we're just not gonna be friends.
						if( otherAIRaceScore == highestScore )
						{
							//do nothing - that git's winning
						}
						else
						{
							// chance of being allies for the next 4 minutes depends on the degree to which
							// the winning race is beating us
							// note safety precautions in case scores are negative or zero
							int ourTotalScore = std::max( 1, otherAIRaceScore + myScore );
							int twiceWinningScore = std::max( 1, highestScore * 2 );

							if( MachPhysRandom::randomInt( 0, ourTotalScore + twiceWinningScore ) < twiceWinningScore )
							{
								// Yup, we've decided to be allies for the moment.
								races.dispositionToRace( otherAIRace, myRace, MachLogRaces::ALLY );
								races.dispositionToRace( myRace, otherAIRace, MachLogRaces::ALLY );
					 		}
							// otherwise, our relationship is that of enemies (well, NEUTRAL) for the next 4 minutes.
						}
					}
				}
			}
		}
	}

	MachLogPlanet::Sites::const_iterator i =  MachLogPlanet::instance().sites().begin();
	for( ; i != MachLogPlanet::instance().sites().end(); ++i )
	{
		// not worth building a mine on a site that only has 100 BMUs' worth of ore - it'd cost 50BMUs just
		// to build the cheapest type of mine!

		const MachLogMineralSite* pSite = (*i);
		if( pSite->hasBeenDiscoveredBy( myRace ) and pSite->amountOfOre() > 100 )
		{
			MexPoint2d p( pSite->position().x(), pSite->position().y() );
			if( not hasMineOnSite( p ) )
			{
				if( not mineSiteIsOccupied( pSite->position() ) )
				{
					// deleteHolographAtPosition( pSite->position() );
					MachLogRace* pRace = &races.race( myRace );

					// what type of mine is most appropriate here?
					// Take into account our highest tech level mine available,
					// the funds we have available and whether or not the outlay for the mine
					// is worth it considering the BMUs left in the mineral deposit

					const MachLogConstructionTree& consTree = races.constructionTree();

					int highestHWLevel = 1;

					MachPhys::BuildingMaterialUnits bmusAvailable = races.nBuildingMaterialUnits( myRace );

					MachPhys::BuildingMaterialUnits costOfLevel3Mine = MachPhysData::instance().mineData( 3 ).cost();
					MachPhys::BuildingMaterialUnits costOfLevel5Mine = MachPhysData::instance().mineData( 5 ).cost();

					if( consTree.activated( myRace, MachLog::MINE, 0, 3 )
						and	pSite->amountOfOre() >= costOfLevel3Mine * 3
						and bmusAvailable > costOfLevel3Mine * 0.8  )	// must have most of the dosh already available
					{
						highestHWLevel = 3;
					}

					if( consTree.activated( myRace, MachLog::MINE, 0, 5 )
						and	pSite->amountOfOre() >= costOfLevel5Mine * 3
						and bmusAvailable > costOfLevel5Mine * 0.8 )		// must have most of the dosh already available
					{
						highestHWLevel = 5;
					}

					MachLogProductionUnit* pProd = _NEW( MachLogProductionUnit( MachLog::MINE, 0, highestHWLevel, 0, 1 ) );
					MexTransform3d trans( pSite->position() );
					pProd->globalTransform( trans );

					addUniqueConstructionProductionUnit( pProd );
				}
			}
		}
	}
	return interval;
}

/* //////////////////////////////////////////////////////////////// */

void MachLogAIController::handleEnemyContact( const MachLogMessage& msg )
{
    //Get the actor that sent the message, and see if it is a machine.
    //Only respond to machines that are not inside buildings.
    MachActor& senderActor = *msg.sender();
    if( senderActor.objectIsMachine() )
    {
        MachLogMachine& senderMachine = senderActor.asMachine();
        if( not senderMachine.insideBuilding() )
        {
            //Try to find any idle aggressive machine within 100m
            MexPoint3d senderPosition = senderActor.position();
            MachLogMachine* pIdleAggressiveMachine = NULL;

			MachLogRaces& races = MachLogRaces::instance();
			MachPhys::Race myRace = race();

            //Try the aggressors in this race first
            {
        		MachLogRaces::Aggressors::iterator i =  races.aggressors( myRace ).begin();
        		MachLogRaces::Aggressors::iterator iend =  races.aggressors( myRace ).end();
        		for( ; pIdleAggressiveMachine == NULL and i != iend; ++i )
        		{
                    if( not (*i)->isDead()  and  (*i)->isIdle()  and
                        (*i)->position().sqrEuclidianDistance( senderPosition ) < 10000 )
                        pIdleAggressiveMachine = (*i);
        		}
            }

            //If no luck try the administrators
            if( pIdleAggressiveMachine == NULL )
            {
        		MachLogRaces::Administrators::iterator i =  races.administrators( myRace ).begin();
        		MachLogRaces::Administrators::iterator iend =  races.administrators( myRace ).end();
        		for( ; pIdleAggressiveMachine == NULL and i != iend; ++i )
        		{
                    if( not (*i)->isDead()  and  (*i)->isIdle()  and
                        (*i)->position().sqrEuclidianDistance( senderPosition ) < 10000 )
                        pIdleAggressiveMachine = (*i);
        		}
            }

            //Now if we have a machine, we can send it in the direction of the caller
            if( pIdleAggressiveMachine != NULL )
            {
                //We need to get a legal position for it to move to in the direction of the sender.
                MexPoint2d targetPosition;
                if( MachLogSpacialManipulation::getNearestFreeSpacePoint
                    (
                        senderMachine.physObject().globalTransform(),
                        5.0, pIdleAggressiveMachine->highClearence(),
                        &targetPosition
                    )
                  )
                {
                    MexPoint3d targetPosition3d( targetPosition );
                    pIdleAggressiveMachine->newOperation( _NEW( MachLogMoveToOperation( pIdleAggressiveMachine,
                                                                                        targetPosition3d,
                                                                                        true,
                                                                                        15.0 ) ) );	// 15m tolerance
                }
            }
        }
    }
}

/* //////////////////////////////////////////////////////////////// */

void MachLogAIController::sendMessage( MachLogCommsId , const MachLogMessage& ) const
{
	ASSERT( false, "shouldn't be here" );
}
void MachLogAIController::receiveMessage( const MachLogMessage& msg )
{
	PRE( msg.sender()->race() == race() );

	switch( msg.text() )
	{
		case MachLogMessage::IDLE :
		{
			switch( msg.sender()->objectType() )
			{
				case MachLog::GEO_LOCATOR :
					handleIdleGeoLocator( msg.sender() );
				break;
				case MachLog::SPY_LOCATOR :
					handleIdleSpyLocator( msg.sender() );
				break;
				case MachLog::TECHNICIAN	:
					handleIdleTechnician( msg.sender() );
				break;
				case MachLog::CONSTRUCTOR :
					handleIdleConstructor( msg.sender() );
				break;
				case MachLog::AGGRESSOR :
					handleIdleAggressor( msg.sender() );
				break;
				case MachLog::ADMINISTRATOR :
					handleIdleAdministrator( msg.sender() );
				break;
				case MachLog::RESOURCE_CARRIER :
					handleIdleResourceCarrier( msg.sender() );
				break;
				case MachLog::APC :
					handleIdleAPC( msg.sender() );
				break;
				default	:
					ASSERT_BAD_CASE;
				break;
			};
		} break;
		case MachLogMessage::ENEMY_SIGHTED	:
			handleEnemyContact( msg );
		break;
		default				:
			ASSERT_BAD_CASE;
		break;
	};
}

/* //////////////////////////////////////////////////////////////// */

//virtual
void MachLogAIController::handleIdleTechnician( MachLogCommsId pObj )
{
	CB_MachLogAIController_DEPIMPL();
	MachLogTechnician* obj = &pObj->asTechnician();

	MachLogRaces& races = MachLogRaces::instance();
	MachPhys::Race myRace = race();

	MachLogHardwareLab* pChosenLab = NULL;
	//check to see if we have any labs atall.
	if( races.hardwareLabs( myRace ).size() == 0 )
	{
//		moveOutOfTheWay( obj );
		return;
	}

	//if we are already inside a lab then set that as the destination lab.
	MachLogResearchItem* pmlri = NULL;
	if( obj->insideBuilding() and obj->insideWhichBuilding().objectType() == MachLog::HARDWARE_LAB and
		( obj->insideWhichBuilding().asHardwareLab().currentlyResearching( &pmlri ) or
		obj->insideWhichBuilding().asHardwareLab().availableResearchItems().size() > 0 ) )
	{
		pChosenLab = &obj->insideWhichBuilding().asHardwareLab();
	}
	else
	{
		switch( researchInterest_ )
		{
			case HARDWARE	:
			{
				//find the closest hardware lab that has free stations.
				MATHEX_SCALAR sqrRange = 1000000000;
				MachLogRaces::HardwareLabs::iterator j = races.hardwareLabs( myRace ).begin();
				for( MachLogRaces::HardwareLabs::iterator i = races.hardwareLabs( myRace ).begin();
					i != races.hardwareLabs( myRace ).end(); ++i )
					{
						MachLogHardwareLab* pCandidateLab = (*i);
						if( obj->position().sqrEuclidianDistance( pCandidateLab->position() ) < sqrRange
							and pCandidateLab->availableResearchItems().size() > 0 )
						{
							MachPhysStation* pStation;
							//MachPhysConstructionData& conData = _STATIC_CAST( MachPhysConstructionData&, pCandidateLab->constructionData() );
							MachPhysConstructionData& conData = _CONST_CAST( MachPhysConstructionData&, pCandidateLab->constructionData() );
							if( conData.stations().freeStation( MachPhysStation::RESEARCH_BAY, &pStation ) )
							{
								j = i;
								sqrRange = obj->position().sqrEuclidianDistance( pCandidateLab->position() );
							}
						}
					}

				//point pChosenLab at correct lab...remeber this will default to the first one if no labs found
				//with a free station
				pChosenLab = *j;
			} break;
			default		:
				ASSERT_BAD_CASE;
			break;
		}
	}

	// if there's an enemy aggressive inside that lab, only enter with a 33% chance.
	// This is purely for game variety and entertainment - not as a "smart tactic".
	if( pChosenLab->isEnemyCanAttackInside( obj->race() )
		and MachPhysRandom::randomInt( 0, 3 ) != 0 )
	{
  //		moveOutOfTheWay( obj );
		return;
	}

	MexPoint2d techPos( obj->position().x(), obj->position().y() );

	MexPoint2d dest2;
	if( researchInterest_ == HARDWARE and pChosenLab )
		dest2 = pChosenLab->entranceExternalPoint( 0 );

	MexPoint3d dest3( dest2.x(), dest2.y(), 0 );

	if( pChosenLab->objectType() == MachLog::HARDWARE_LAB )
	{
		pmlri = NULL;
		if( not pChosenLab->currentlyResearching( &pmlri ) )
			for( MachLogResearchTree::ResearchItems::iterator i = pChosenLab->availableResearchItems().begin(); i != pChosenLab->availableResearchItems().end(); ++i)
		 		pChosenLab->addResearchItem( (**i) );
	}

	MachPhysStation* pStation;
	switch( researchInterest_ )
	{
		case HARDWARE:
		{
			//MachPhysConstructionData& conData = _STATIC_CAST( MachPhysConstructionData&, pChosenLab->constructionData() );
			MachPhysConstructionData& conData = _CONST_CAST( MachPhysConstructionData&, pChosenLab->constructionData() );
			if( conData.stations().freeStation( MachPhysStation::RESEARCH_BAY, &pStation ) )
			{
				if( pChosenLab->objectType() == MachLog::HARDWARE_LAB )
				{
					if( researchInterest_ == HARDWARE )
					{
						if( not ( obj->insideBuilding() and obj->insideWhichBuilding().id() == pChosenLab->id() ) )
							obj->newOperation( _NEW( MachLogEnterBuildingOperation( obj, pChosenLab, pStation ) ) );
					}
				}
			}
			else
			{
				//already in this lab? (and hence at a free station). If so do not issue moveOutOfWay.
				if( not ( obj->insideBuilding() and obj->insideWhichBuilding().id() == pChosenLab->id() ) )
					moveOutOfTheWay( obj );
			}
		}
		break;
		default:
			//ASSERT_INFO( researchInterest_ );
			ASSERT_BAD_CASE;
	}

}

/* //////////////////////////////////////////////////////////////// */

bool MachLogAIController::hasMineOnSite( const MexPoint2d& pos ) const
{
	MachLogRaces& races = MachLogRaces::instance();
	MachPhys::Race myRace = race();

	bool found = false;
	MachLogRaces::Mines::const_iterator i =  races.mines( myRace ).begin();
	for( ; i != races.mines( myRace ).end() and not found; ++i )
	{
		MexPoint2d p( (*i)->position().x(), (*i)->position().y() );
		found = ( p == pos );
	}

	return found;
}

/* //////////////////////////////////////////////////////////////// */

//virtual
void MachLogAIController::handleIdleAdministrator( MachLogCommsId pObj )
{
	//need to add level() to machine.hpp
	MachLogAdministrator* obj = &pObj->asAdministrator();
	ASSERT( obj->objectType() == MachLog::ADMINISTRATOR, "Machine type is not an administrator.\n" );
	MachLogAdministrator* pAdmin = (MachLogAdministrator*)obj;
//	//HAL_STREAM( "MachLogAIController::handleIdleAdministrator\n" );
//	//HAL_STREAM( " Race: " << obj->race() << " aggressors " << MachLogRaces::instance().aggressors( obj->race() ).size() << std::endl );
//	//HAL_STREAM( " Administrator " << obj->id() << " has " << pAdmin->objects().size() << " objects already.\n" );
//	if( pAdmin->squadronType() != NO_SQUAD )
//		return;

	if( not pAdmin->squadron() )
		moveOutOfTheWay( obj );
}


void MachLogAIController::readRules( const SysPathName& pathName )
{
	HAL_STREAM("MLAIController::readRules " << pathName.pathname().c_str() << std::endl );

	MachLogRaces& races = MachLogRaces::instance();
	MachPhys::Race myRace = race();

    SysMetaFile metaFile( "mach1.met" );

    std::auto_ptr< std::istream > pIstream;

    if( SysMetaFile::useMetaFile() && metaFile.hasFile( pathName ) )
    {
        //pIstream = _NEW( SysMetaFileIstream( metaFile, pathName, ios::text ) );
        pIstream = std::auto_ptr< std::istream >(
            _NEW( SysMetaFileIstream( metaFile, pathName, std::ios::in ) ));
    }
    else
    {
        ASSERT_FILE_EXISTS( pathName.c_str() );
        //pIstream = _NEW( ifstream( pathName.c_str(), ios::text | ios::in ) );
        pIstream = std::auto_ptr< std::istream >(
            _NEW( std::ifstream( pathName.c_str(), std::ios::in ) ) );
    }

	UtlLineTokeniser parser( *pIstream, pathName );
	bool finishedSPL = false;
    while( not finishedSPL )
    {
		int lineSize = parser.tokens().size();
		HAL_STREAM(" lineSize " << lineSize << std::endl );
		if( lineSize > 0 )
		{
			HAL_STREAM(" token[0] " << parser.tokens()[ 0 ].c_str() << std::endl );
		}
		if( lineSize > 0 )
		{
			if( parser.tokens()[0] == "NUMBER" )
				{
					HAL_STREAM(" detected 'NUMBER' line\n" );
					int number = atol( parser.tokens()[1].c_str() );
					int priority = atol( parser.tokens()[2].c_str() );
					MachLog::ObjectType ot = MachLogScenario::objectType( parser.tokens()[3] );
					int hwLevel = atol( parser.tokens()[4].c_str() );
					int subType = 0;
					MachPhys::WeaponCombo wc = MachPhys::N_WEAPON_COMBOS;
					if( parser.tokens().size() >= 6 )
						subType = MachLogScenario::objectSubType( ot, parser.tokens()[5] );
					if( parser.tokens().size() == 7 )
						wc = MachLogScenario::weaponCombo( parser.tokens()[6] );
					MachLogResearchItem& ri = races.researchTree().researchItem( ot, subType, hwLevel, wc );
					int swLevel = ri.swLevel( myRace );
					MachLogProductionUnit* pProd = _NEW( MachLogProductionUnit( ot, subType, hwLevel, swLevel, priority ) );
//					if( wc != MachPhys::N_WEAPON_COMBOS )
						pProd->weaponCombo( wc );
					HAL_STREAM(" adding desired machine:\n" << *pProd << std::endl );
					addDesiredMachine( pProd, number );

/*					{
						//HAL_STREAM(" priority not zero creating production unit\n" );
						strategicProductionList_.push_back(
							_NEW( MachLogAIStrategicProductionUnit( parser.tokens()[0], number, priority ) )
						);
						//HAL_STREAM(" priority not zero creating production unit DONE!\n" );
					}*/

				}	//token type
			else
				if( parser.tokens()[0] == "END" )
					finishedSPL = true;
		} //lineSize > 0
	parser.parseNextLine();
	} // while

	bool finishedSquadron = false;
	int currentSquad = -1;
    while( not parser.finished() )
    {
		int lineSize = parser.tokens().size();
		if( lineSize > 0 )
		{
			if( parser.tokens()[0] == "SQUAD" )
			{
				HAL_STREAM(" begining a squadron pass\n" );
				MachLogSquadron* pSquad;
				++currentSquad;
//				int currentSquad = races.freeSquadron( myRace, &pSquad );
				if( currentSquad > 9 )
					races.addExtraSquadron( myRace );
				pSquad = races.squadrons( myRace )[ currentSquad ];
				bool finishedSquad = false;
				string taskType;
				HAL_STREAM(" found a free squadron " << currentSquad << std::endl );
				while( not finishedSquad )
				{
					if( parser.tokens().size() > 0 )
					{
						HAL_STREAM(" lineSize " << parser.tokens().size() << std::endl );
						if( parser.tokens()[0] == "AI" )
						{
							taskType = parser.tokens()[1];
							HAL_STREAM(" detected AI line: taskType " << taskType << std::endl );
							createCorrectSquadronOperation( pSquad, parser );
						}
						else if( parser.tokens()[0] == "NUMBER" )
						{
							HAL_STREAM(" detected NUMBER line processing..." );
							int number = atol( parser.tokens()[1].c_str() );
							HAL_STREAM(" number " << number );
							int priority = atol( parser.tokens()[2].c_str() );
							HAL_STREAM(" priority " << priority << std::endl );
							MachLog::ObjectType ot = MachLogScenario::objectType( parser.tokens()[3] );
							HAL_STREAM(" type " << ot );
							int hwLevel = atol( parser.tokens()[4].c_str() );
							HAL_STREAM(" hwLevel " << hwLevel );
							int subType = 0;
							if( parser.tokens().size() >= 6 )
							{
								HAL_STREAM( std::endl << " Token size is >= 6 so getting subtype from tokens[5]\n" );
								subType = MachLogScenario::objectSubType( ot, parser.tokens()[5] );
							}
							MachPhys::WeaponCombo wc = MachPhys::N_WEAPON_COMBOS;
							if( parser.tokens().size() >= 7 )
								wc = MachLogScenario::weaponCombo( parser.tokens()[6] );
							MachLogResearchItem& ri = races.researchTree().researchItem( ot, subType, hwLevel, wc );
							int swLevel = ri.swLevel( myRace );
							MachLogProductionUnit* pProd = _NEW( MachLogProductionUnit( ot, subType, hwLevel, swLevel, priority ) );
//							if( wc != MachPhys::N_WEAPON_COMBOS )
								pProd->weaponCombo( wc );
							pProd->constructionId( pSquad->squadronId() - 1 );
							HAL_STREAM(" adding a desired machine to squad:\n" << *pProd << std::endl );
							pSquad->addDesiredMachine( pProd, number );
						}
						else if( parser.tokens()[0] == "DEFCON" )
						{
							if( parser.tokens()[1] == "LOW" )
								pSquad->autoSetDefCon( MachLog::DEFCON_LOW );
							else if( parser.tokens()[1] == "NORMAL" )
								pSquad->autoSetDefCon( MachLog::DEFCON_NORMAL );
							else if( parser.tokens()[1] == "HIGH" )
								pSquad->autoSetDefCon( MachLog::DEFCON_HIGH );
						}
						else if( parser.tokens()[0] == "END" )
							finishedSquad = true;
					}
					parser.parseNextLine();
				} //while not finished this squad
			}
			else parser.parseNextLine();
		}
	} // while not finished parser
}


//virtual
void MachLogAIController::doOutputOperator( ostream& o) const
{
	CB_MachLogAIController_DEPIMPL();
	o << "MachLogAIController for race " << race() << std::endl;
	o << " construction list.size() " << constructionProductionList_.size() << std::endl;
	if( constructionProductionList_.size() > 0 )
	{
		for( MachLogAIController::ProductionList::const_iterator i = constructionProductionList_.begin();
			i != constructionProductionList_.end(); ++i )
			{
				o << "  " << (*i)->type() << " sub " << (*i)->subType() << " Pri: " << (*i)->priority();
				o << " at:" << (*i)->globalTransform().position() << " place " << MachLogTaskConstructOperation::couldPlaceConstruction( (**i) ) << std::endl;
			}
	}

	o << " Strategic production List.size() " << strategicProductionList_.size() << std::endl;
	if( strategicProductionList_.size() > 0 )
	{
		for( MachLogAIController::StrategicProductionList::iterator i = strategicProductionList_.begin();
			i != strategicProductionList_.end(); ++i )
				o << (*i)->type() << " Num: " << (*i)->number() << " Pri: " << (*i)->priority() << std::endl;
	}
}


ostream& operator<<( ostream& o, const MachLogAIController& t )
{
	t.doOutputOperator( o );
	return o;
}

void MachLogAIController::addDesiredMachine( MachLogProductionUnit* pProd, int desiredNumber )
{
	CB_MachLogAIController_DEPIMPL();
	DesiredMachineData*	dmd = _NEW( DesiredMachineData );
	dmd->pProdUnit_ = pProd;
	dmd->desiredNumber_ = desiredNumber;
	dmd->actualNumber_ = 0;
	desiredMachineList_.push_back( dmd );

}

void MachLogAIController::machineCreated( const MachLogMachine& mlm )
{
	CB_MachLogAIController_DEPIMPL();
	static MachLogProductionUnit prod;
	prod.type( mlm.objectType() );
	prod.subType( mlm.subType() );
	prod.hwLevel( mlm.hwLevel() );
	prod.swLevel( mlm.swLevel() );
	MachPhys::WeaponCombo wc = MachPhys::N_WEAPON_COMBOS;
	if( mlm.objectIsCanAttack() )
		wc = mlm.asCanAttack().weaponCombo();
	prod.weaponCombo( wc );
	prod.priority( MachLogProductionUnit::UNCHECKED_PRIORITY );
	for( DesiredMachineList::iterator i = desiredMachineList_.begin();
		i != desiredMachineList_.end(); ++i )
		if( *((*i)->pProdUnit_) == prod )
			(*i)->actualNumber_++;
}

void MachLogAIController::machineDestroyed( const MachLogMachine& mlm )
{
	CB_MachLogAIController_DEPIMPL();
	static MachLogProductionUnit prod;
	prod.type( mlm.objectType() );
	prod.subType( mlm.subType() );
	prod.hwLevel( mlm.hwLevel() );
	prod.swLevel( mlm.swLevel() );
	prod.priority( MachLogProductionUnit::UNCHECKED_PRIORITY );
	MachPhys::WeaponCombo wc = MachPhys::N_WEAPON_COMBOS;
	if( mlm.objectIsCanAttack() )
		wc = mlm.asCanAttack().weaponCombo();
	prod.weaponCombo( wc );
	for( DesiredMachineList::iterator i = desiredMachineList_.begin();
		i != desiredMachineList_.end(); ++i )
		if( *((*i)->pProdUnit_) == prod )
			(*i)->actualNumber_--;
}

void MachLogAIController::addConstructionProductionUnit( MachLogProductionUnit* pProd )
{
	CB_MachLogAIController_DEPIMPL();
	HAL_STREAM("MLAIController::addConstructionProductionUnit:\n" << *pProd << std::endl );
	constructionProductionList_.push_back( pProd );
}

void MachLogAIController::addUniqueConstructionProductionUnit( MachLogProductionUnit* pProd )
{
	//go through each of the existing construction production units.
	//if the production units are == and the global transforms are equal then do not post this
	//production unit but delete it instead.
	CB_MachLogAIController_DEPIMPL();
	bool foundMatch = false;
	HAL_STREAM("MLAIController::addUniqueConstructionProductionUnit:\n" << *pProd << std::endl );
	for( ProductionList::iterator i = constructionProductionList_.begin();
		i != constructionProductionList_.end() and not foundMatch; ++i )
		{
			if( (**i) == *pProd and (*i)->globalTransform() == pProd->globalTransform() )
				foundMatch = true;
		}
	HAL_STREAM(" found a match with existing = " << foundMatch << std::endl );
	if( foundMatch )
		_DELETE( pProd );
	else
		constructionProductionList_.push_back( pProd );
}

bool MachLogAIController::hasWaitingConstruction()
{
	CB_MachLogAIController_DEPIMPL();
	return (constructionProductionList_.size() > 0);
}

bool MachLogAIController::getHighestPriorityConstruction( MachLogProductionUnit* pProd )
{
	HAL_STREAM( "MachLogAIController::getHighestPriorityConstruction::" << std::endl );

	CB_MachLogAIController_DEPIMPL();
	bool constructionFound = false;

	HAL_STREAM(" constructionProductionList_.size() " << constructionProductionList_.size() << std::endl );
	if( constructionProductionList_.size() > 0 )
	{
		HAL_STREAM( "At least one production in the list." << std::endl );

		int highestPriorityEncountered = -1;
		ProductionList::iterator iCurrentBest;	// = constructionProductionList_.begin();
		for( ProductionList::iterator i = constructionProductionList_.begin();
			i != constructionProductionList_.end(); ++i )
		{
			HAL_STREAM( "Trying the next one......" << std::endl );

			const MachLogProductionUnit& candidateProdUnit = (**i);
			HAL_STREAM(" candidateProdUnit " << candidateProdUnit << std::endl );

			// firstly, do not accept as valid any production unit specifying a construction we are as yet
			// technologically incapable of building

			bool canBuild = MachLogRaces::instance().constructionTree().activated( race(),
																				   candidateProdUnit.type(),
																				   candidateProdUnit.subType(),
																				   candidateProdUnit.hwLevel() );


			HAL_STREAM( "Can we build this one yet?" << canBuild << std::endl );

			// do not accept as valid any production unit specifying a construction that cannot
			// currently be placed on the world at the position specified, nor one that is an
			// alternative site of a construction that has already been built, nor one that is of
			// a lower priority to a valid one already encountered.

			if( canBuild )
			{
				bool priorityIsBestYet = candidateProdUnit.priority() > highestPriorityEncountered;
				HAL_STREAM( "Is the priority better than any yet found?" << priorityIsBestYet << std::endl );

				if( priorityIsBestYet )
				{
					bool ableToPlaceConstruction = MachLogTaskConstructOperation::couldPlaceConstruction( candidateProdUnit );
					HAL_STREAM( "Can we place the construction?" << ableToPlaceConstruction << std::endl );

					if( ableToPlaceConstruction )
					{
						bool thisIsNotDuplicate = MachLogRaces::instance().noAlternativeAlreadyBuilt( race(), candidateProdUnit );
						HAL_STREAM( "Have we confirmed no alternative has already been built?" << thisIsNotDuplicate << std::endl );

						if( thisIsNotDuplicate )
						{
							iCurrentBest = i;
							highestPriorityEncountered = candidateProdUnit.priority();
						}
					}
				}
			}
		}

		if( highestPriorityEncountered >= 0 )	// i.e. something valid actually was encountered
		{
			HAL_STREAM( " this is a valid construction passing back." << std::endl );
			pProd->type( (*iCurrentBest)->type() );
			pProd->subType( (*iCurrentBest)->subType() );
			pProd->hwLevel( (*iCurrentBest)->hwLevel() );
			pProd->swLevel( (*iCurrentBest)->swLevel() );
			pProd->priority( (*iCurrentBest)->priority() );
			pProd->globalTransform( (*iCurrentBest)->globalTransform() );
			pProd->weaponCombo( (*iCurrentBest)->weaponCombo() );
			pProd->needRebuild( (*iCurrentBest)->needRebuild() );
			pProd->constructionId( (*iCurrentBest)->constructionId() );
			_DELETE( *iCurrentBest );
			constructionProductionList_.erase( iCurrentBest );
			constructionFound = true;
		}
	}
	else
		constructionFound = false;

	HAL_STREAM( "Did we find a construction?" << constructionFound << std::endl );

	return constructionFound;
}


void MachLogAIController::createCorrectSquadronOperation( MachLogSquadron* pSquad, const UtlLineTokeniser& parser )
{
	CB_MachLogAIController_DEPIMPL();
	ASSERT( parser.tokens()[0] == "AI","Wrong token type detected in createCorrectSquadronOperation\n" );
	HAL_STREAM("(" << pSquad->id() << ") MLAIController::createCorrectSquadronOp\n" );
	if( parser.tokens()[1] == "LOCATE" )
	{
		HAL_STREAM(" Found Token for: TaskLocateOperation\n" );
		int nDesiredSites = 200;
		MATHEX_SCALAR maxRangeFromPod = 1000000;

		for( int i = 2; i < parser.tokens().size(); ++i )
		{
			if( parser.tokens()[i] == "SITES" )
				nDesiredSites = atol( parser.tokens()[i + 1].c_str() );
			if( parser.tokens()[i] == "RANGE_POD" )
				maxRangeFromPod = atof( parser.tokens()[i + 1].c_str() );
		}
		pSquad->newOperation( _NEW( MachLogTaskLocateOperation( pSquad, nDesiredSites, maxRangeFromPod ) ) );
	}
	else if( parser.tokens()[1] == "CONSTRUCT" )
	{
		HAL_STREAM(" Found Token for: TaskConstructOperation\n" );
		pSquad->newOperation( _NEW( MachLogTaskConstructOperation( pSquad ) ) );
	}
	else if( parser.tokens()[1] == "ATTACK" )
	{
		HAL_STREAM(" Found Token for: TaskAttackOperation\n" );
		ASSERT_INFO( parser.tokens().size() );
		ASSERT( parser.tokens().size() > 3,"TaskAttack must have AT LEAST 4 tokens\n" );
		ASSERT( parser.tokens()[2] == "DELAY","Third token of an attck AI line MUST BE DELAY" );
		//AttackIn parameter is expressed in minutes so convert to seconds
		PhysRelativeTime attackIn = atof( parser.tokens()[3].c_str() );
		attackIn *= 60.0;
		MachLogTaskAttackOperation* pAttackOp = _NEW( MachLogTaskAttackOperation( pSquad, attackIn ) );
		for( int i = 3; i < parser.tokens().size(); ++i )
		{
			HAL_STREAM(" processing token " << parser.tokens()[i] << std::endl );
			ASSERT_INFO( parser.tokens()[i] );
			if( parser.tokens()[i] == "POINT" )
			{
				MATHEX_SCALAR startX = atof( parser.tokens()[i + 1].c_str() );
				MATHEX_SCALAR startY = atof( parser.tokens()[i + 2].c_str() );
				pAttackOp->startingPoint( MexPoint3d( startX, startY, 0 ) );
			}
			else if( parser.tokens()[i] == "MINIMUM" )
			{
				int minimum = atol( parser.tokens()[i + 1].c_str() );
				pAttackOp->nMinimumMachines( minimum );
			}
			else if( parser.tokens()[i] == "RESOURCE" )
			{
				pAttackOp->targetSystemType( MachLog::TARGET_RESOURCE );
			}
			else if( parser.tokens()[i] == "RESEARCH" )
			{
				pAttackOp->targetSystemType( MachLog::TARGET_RESEARCH );
			}
			else if( parser.tokens()[i] == "OBJECT" )
			{
				MachLog::ObjectType ot = MachLogScenario::objectType( parser.tokens()[i+1] );
				int subType = 0;
				for( int j = i; j < parser.tokens().size(); ++j )
				{
					if( parser.tokens()[j] == "SUBTYPE" )
					{
						subType = MachLogScenario::objectSubType( ot, parser.tokens()[j+1] );
					}
				}
				pAttackOp->targetSystemType( MachLog::TARGET_OBJECT );
				pAttackOp->objectType( ot, subType );
			}
			else if( parser.tokens()[i] == "USE" )
			{
				ASSERT( parser.tokens().size() > (i + 1),"Out of tokens on ATTACK USE line\n" );
				if( parser.tokens()[i+1] == "NUKE" )
					pAttackOp->useNuke();
				if( parser.tokens()[i+1] == "ION_ORBITAL_CANNON" )
					pAttackOp->useIonCannon();
			}
			else if( parser.tokens()[i] == "ASSEMBLY" )
			{
				ASSERT( parser.tokens().size() > (i + 1),"Out of tokens on ATTACK ASSEMBLY line\n" );
				pAttackOp->assemblyPoint( atol( parser.tokens()[i+1].c_str() ) );
			}
			else if( parser.tokens()[i] == "DO" )
			{
				ASSERT( parser.tokens().size() > (i + 1),"Out of tokens on ATTACK DO line\n" );
				pAttackOp->doCount( atol( parser.tokens()[i+1].c_str() ) );
			}
			else if( parser.tokens()[i] == "NO_CLUSTER_CHECK" )
			{
				pAttackOp->supressProximityCheck();
			}

		}
		pSquad->newOperation( pAttackOp );
	}
	else if( parser.tokens()[1] == "DROP_MINES" )
	{
		HAL_STREAM(" Found Token for: TaskDropLandMinesOperation\n" );
		ASSERT_INFO( parser.tokens().size() );
		ASSERT( parser.tokens().size() >= 5,"TaskDropLandMines must have AT LEAST 5 tokens\n" );
		MATHEX_SCALAR minRange = atof( parser.tokens()[2].c_str() );
		MATHEX_SCALAR maxRange = atof( parser.tokens()[3].c_str() );
		if( parser.tokens()[4] == "POD" )
		{
			MachLogTaskDropLandMineOperation* pDropMinesOp = _NEW( MachLogTaskDropLandMineOperation( pSquad, minRange, maxRange ) );
			pSquad->newOperation( pDropMinesOp );
		}
		else
		{
			ASSERT( parser.tokens().size() == 7, "Start coordinate token assumed but wrong number of tokens\n" );
			MATHEX_SCALAR startX = atof( parser.tokens()[5].c_str() );
			MATHEX_SCALAR startY = atof( parser.tokens()[6].c_str() );
			MachLogTaskDropLandMineOperation* pDropMinesOp = _NEW( MachLogTaskDropLandMineOperation( pSquad, minRange, maxRange, MexPoint2d(startX, startY ) ) );
			pSquad->newOperation( pDropMinesOp );
		}
	}
	else if( parser.tokens()[1] == "PATROL" )
	{
		HAL_STREAM(" Found Token for: TaskPatrolOperation\n" );
		ASSERT( parser.tokens().size() >= 6,"patrol must have at least two x,y pairs\n" );
		PhysRelativeTime attackIn = 0;
		int indexPathStartAt = 2;
		for( int i = 0; i < parser.tokens().size(); ++i )
		{
			if( parser.tokens()[i] == "DELAY" )
			{
				attackIn = atof( parser.tokens()[i+1].c_str() );
				attackIn *= 60.0;
				indexPathStartAt = i + 2;
			}
		}
		MachLogTaskPatrolOperation::Path path;
		for( int i = indexPathStartAt; i < parser.tokens().size(); i += 2 )
		{
			MexPoint2d point;
			point.x( atof( parser.tokens()[i].c_str() ) );
			point.y( atof( parser.tokens()[i+1].c_str() ) );
			path.push_back( point );
		}
		pSquad->newOperation( _NEW( MachLogTaskPatrolOperation( pSquad, path, attackIn ) ) );
	}
}

void perWrite( PerOstream& ostr, const MachLogAIController& actor )
{
	PER_WRITE_INDENT_STREAM("Writing a MachLogAIController\n" );
	const MachLogController& base1 = actor;

	ostr << base1;

	ostr << actor.pImpl_;
	PER_WRITE_INDENT_STREAM("Writing a MachLogAIController DONE\n" );
}

void perRead( PerIstream& istr, MachLogAIController& actor )
{
	PER_READ_INDENT_STREAM("Writing a MachLogAIController\n" );
	MachLogController& base1 = actor;

	istr >> base1;

	istr >> actor.pImpl_;

	PER_READ_INDENT_STREAM("Writing a MachLogAIController DONE\n" );
}

MachLogAIController::MachLogAIController( PerConstructor con )
:	MachLogController( con )
{
}

void perWrite( PerOstream& ostr, const DesiredMachineData& data )
{
	ostr << data.pProdUnit_;
	ostr << data.desiredNumber_;
	ostr << data.actualNumber_;
}

void perRead( PerIstream& istr, DesiredMachineData& data )
{
	istr >> data.pProdUnit_;
	istr >> data.desiredNumber_;
	istr >> data.actualNumber_;
}

void MachLogAIController::checkForDynamicAllies( bool value )
{
	CB_MachLogAIController_DEPIMPL();
	HAL_STREAM("MachLogAIController::checkForDynamicAllies [" << race() << "] " << value << std::endl );
	checkForDynamicAllies_ = value;
}
bool MachLogAIController::checkForDynamicAllies() const
{
	CB_MachLogAIController_DEPIMPL();
	return checkForDynamicAllies_;
}

//static
PhysRelativeTime MachLogAIController::allyUpdateInterval()
{
	static PhysRelativeTime interval = 240.0;
	return interval;
}

/* //////////////////////////////////////////////////////////////// */

// Forced recompile 19/2/99 CPS
/* End CONTROLR.HPP *************************************************/
