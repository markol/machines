/*
 * C O M M A N D . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "mathex/point3d.hpp"
#include "machgui/command.hpp"
#include "machgui/ingame.hpp"
#include "machgui/internal/commandi.hpp"
#include "machgui/internal/mgsndman.hpp"
#include "machlog/actor.hpp"
#include "machlog/machine.hpp"
#include "machlog/squad.hpp"
#include "machlog/planet.hpp"
#include "machlog/machine.hpp"
#include "machlog/grpmover.hpp"
#include "machphys/mphydata.hpp"
#include "machphys/genedata.hpp"
#include "phys/cspace2.hpp"
#include "mathex/line2d.hpp"
#include "mathex/circle2d.hpp"
#include "mathex/transf3d.hpp"
#include "ctl/pvector.hpp"
#include "ctl/list.hpp"
#include "system/pathname.hpp"

MachGuiCommand::MachGuiCommand( MachInGameScreen* pInGameScreen )
:   isExecuted_( false ),
    isVisible_( false ),
    isSquadronContext_( pInGameScreen->applyCommandToSquadron() ),
	hasPlayedVoiceMail_( false ),
    pImpl_( _NEW( MachGuiCommandImpl( pInGameScreen ) ) )
{
    PRE( pInGameScreen != NULL );

    TEST_INVARIANT;
}

MachGuiCommand::~MachGuiCommand()
{
    TEST_INVARIANT;

    _DELETE( pImpl_ );
}

void MachGuiCommand::CLASS_INVARIANT
{
	INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachGuiCommand& t )
{

    o << "MachGuiCommand " << (void*)&t << " start" << std::endl;
    o << "MachGuiCommand " << (void*)&t << " end" << std::endl;

    return o;
}

void MachGuiCommand::apply()
{
    PRE( isInteractionComplete() );

	MachGuiSoundManager::instance().playSound( "gui/sounds/taskmach.wav" );

    ++commandId();

    //Get the selected set of actors
    const MachInGameScreen::Actors& selectedActors = pImpl_->pInGameScreen_->selectedActors();
    string reason;
    bool succeeded = false;

    pImpl_->clearMachineOperations();

    //Check for being in the squadron menu, with a selected squadron having
    //an intelligent administrator. If so, and the command has an admin version,
    //use that version.
    bool doneAdminVersion = false;
    if( isSquadronContext_ and canAdminApply() )
    {
        //Get first actor, which must be a machine and not inside an APC.
        MachActor* pActor = selectedActors.front();
        if( pActor->objectIsMachine() and not pActor->asMachine().insideAPC() )
        {
            //Check we have a squadron
            MachLogSquadron* pSquadron = pActor->asMachine().squadron();
            if( pSquadron != NULL and pSquadron->hasCommander() )
            {
                succeeded = doAdminApply( &pSquadron->commander(), &reason );
                doneAdminVersion = true;
            }
        }
    }

    //Apply the command to each one if not done as squadron
    if( not doneAdminVersion )
    {
		// Filter out machines inside an APC
		MachInGameScreen::Actors actors;
        actors.reserve( 256 );
		for( MachInGameScreen::Actors::const_iterator iter = selectedActors.begin(); iter != selectedActors.end(); ++iter )
		{
			MachActor* pActor = *iter;
			if ( pActor->objectIsMachine() and pActor->asMachine().insideAPC() )
			{}
			else
			{
				actors.push_back( pActor );
			}
		}

        if( actors.size() > 1 and canApplyToGroup() )
        {
            succeeded = doGroupApply( actors, &reason );
        }
        else
        {
            for( MachInGameScreen::Actors::iterator it = actors.begin(); it != actors.end(); ++it )
            {
                //Get the actor
                MachActor& actor = *(*it);

                //Check the command can be applied, then apply it
                if( canActorEverExecute( actor ) )
				{
                    succeeded = doApply( &actor, &reason ) or succeeded;
				}
            }
        }
    }

    if( pImpl_->machineOperations().size() > 0 )
    {
        MachLogGroupMover mover( pImpl_->machineOperations() );
    }

    //Mark the command as executed
    isExecuted_ = true;

    //Display failure reason if found
    if( not succeeded )
        pImpl_->pInGameScreen_->cursorPromptText( reason );
	else
		// now reset the voice mail played status to allow consecutive reissues of a command to give a voicemail
		// on each occasion
		hasPlayedVoiceMail_ = false;
}

bool MachGuiCommand::isExecuted() const
{
    return isExecuted_;
}

bool MachGuiCommand::hasPlayedVoiceMail() const
{
	return hasPlayedVoiceMail_;
}

void MachGuiCommand::hasPlayedVoiceMail( bool status )
{
	hasPlayedVoiceMail_ = status;
}

MachInGameScreen& MachGuiCommand::inGameScreen() const
{
    return *pImpl_->pInGameScreen_;
}

bool MachGuiCommand::isVisible() const
{
    return isVisible_;
}

void MachGuiCommand::isVisible( bool visible )
{
    isVisible_ = visible;
}

//virtual
bool MachGuiCommand::actorsCanExecute() const
{
    //Default behaviour is to check if any selected actor can ever execute
    bool result = false;

    const MachInGameScreen::Actors& selectedActors = inGameScreen().selectedActors();
    for( MachInGameScreen::Actors::const_iterator it = selectedActors.begin();
         it != selectedActors.end(); ++it )
    {
		MachActor* pActor = *it;
        if( ( not ( pActor->objectIsMachine() and pActor->asMachine().insideAPC() ) ) // Check that machine is not inside an APC
        	and canActorEverExecute( *pActor ) )
        {
            result = true;
            break;
        }
    }

    return result;

}

//virtual
void MachGuiCommand::start()
{
    //Do nothing
}

//virtual
void MachGuiCommand::finish()
{
    //Do nothing
}

//virtual
bool MachGuiCommand::canAdminApply() const
{
    return false;
}

//virtual
bool MachGuiCommand::doAdminApply( MachLogAdministrator*, string* )
{
    PRE( canAdminApply() );
    return false;
}

bool MachGuiCommand::isPointValidOnTerrain(
    const MexPoint2d& point,
    ObstacleModifier modifier ) const
{
    bool    valid = false;

    PhysConfigSpace2d::DomainId domainId;

    if ( MachLogPlanet::instance().configSpace().domain( point, &domainId ) )
    {
        ObstacleFlags flags = 0;

        switch( modifier )
        {
            case USE_ALL_OBSTACLES:
                flags = 0;
                break;
            case IGNORE_SELECTED_ACTOR_OBSTACLES:
                flags = selectedActorObstacleFlags();
                break;
            case IGNORE_ALL_ACTOR_OBSTACLES:
                flags = MachLog::OBSTACLE_ALL;
                break;
        }

        valid = true;

        PhysConfigSpace2d::PolygonId nastyPolygonId;
        if( not MachLogPlanet::instance().configSpace().contains(
          point, flags, &nastyPolygonId, PhysConfigSpace2d::USE_PERMANENT_ONLY ) )
        {
            valid = false;
        }
    }

    return valid;
}

bool MachGuiCommand::convertPointsToValidPoints(ObstacleModifier modifier,
											    MachLogMachine* pMachine,
												const ctl_vector< MexPoint3d >& invalidPath,
												ctl_list< MexPoint2d >* pPath )
{
	PRE( pPath );
	PRE( pPath->size() == 0 );

	ctl_list< MexPoint2d >& path = *pPath;

   size_t nPoints = invalidPath.size();

	// Iterate through way points and adjust to valid way points.
    for( size_t i = 0; i != nPoints; ++i )
	{
		MexPoint2d validPoint;

		bool valid = findClosestPointValidOnTerrain(invalidPath[i],
					 							    modifier,
					 					    		pMachine,
					 					    		&validPoint );

		if ( valid )
		{
        	path.push_back( validPoint );
		}
	}

	if ( path.size() > 0 )
	{
		return true;
	}

	return false;
}

bool MachGuiCommand::convertPointsToValidPoints(ObstacleModifier modifier,
											    MachLogMachine* pMachine,
												const ctl_vector< MexPoint2d >& invalidPath,
												ctl_list< MexPoint2d >* pPath )
{
	PRE( pPath );
	PRE( pPath->size() == 0 );

	ctl_list< MexPoint2d >& path = *pPath;

   size_t nPoints = invalidPath.size();

	// Iterate through way points and adjust to valid way points.
    for( size_t i = 0; i != nPoints; ++i )
	{
		MexPoint2d validPoint;

		bool valid = findClosestPointValidOnTerrain(invalidPath[i],
					 							    modifier,
					 					    		pMachine,
					 					    		&validPoint );

		if ( valid )
		{
        	path.push_back( validPoint );
		}
	}

	if ( path.size() > 0 )
	{
		return true;
	}

	return false;
}

bool MachGuiCommand::convertPointsToValidPoints(ObstacleModifier modifier,
											    MachLogMachine* pMachine,
												const ctl_vector< MexPoint3d >& invalidPath,
												ctl_vector< MexPoint2d >* pPath )
{
	PRE( pPath );
	PRE( pPath->size() == 0 );

	ctl_vector< MexPoint2d >& path = *pPath;

   size_t nPoints = invalidPath.size();

	// Iterate through way points and adjust to valid way points.
    for( size_t i = 0; i != nPoints; ++i )
	{
		MexPoint2d validPoint;

		bool valid = findClosestPointValidOnTerrain(invalidPath[i],
					 							    modifier,
					 					    		pMachine,
					 					    		&validPoint );

		if ( valid )
		{
        	path.push_back( validPoint );
		}
	}

	if ( path.size() > 0 )
	{
		return true;
	}

	return false;
}

bool MachGuiCommand::findClosestPointValidOnTerrain(const MexPoint2d& endPoint,
												    ObstacleModifier modifier,
										    		MachLogMachine* pMachine,
										    		MexPoint2d* pClosestPoint ) const
{
	return findClosestPointValidOnTerrain( 	endPoint,
											pMachine->globalTransform().position(),
											modifier,
											pMachine->highClearence(),
											pMachine->highClearence(),
											pClosestPoint );
}

bool MachGuiCommand::findClosestPointValidOnTerrain(const MexPoint2d& endPoint,
													const MexPoint2d& startPoint,
												    ObstacleModifier modifier,
										    		MATHEX_SCALAR clearance,
													MATHEX_SCALAR stepDistance,
										    		MexPoint2d* pClosestPoint ) const
{
	MexPoint2d pointToCheckNear( endPoint );
	MexPoint2d pointToCheckFar( endPoint );
	bool valid = false;
    ObstacleFlags flags = 0;
    MexLine2d movingLine( startPoint, endPoint );
	MATHEX_SCALAR distanceFromOriginalPoint = 0;
	bool checkFar = false; 	// Flag to simply stop us checking the first point twice ( could be extended
							// to stop us checking the far point when it moves too far away from original point )

	NEIL_STREAM( "MachGuiCommand::findClosestPointValidOnTerrain" << std::endl );

    switch( modifier )
    {
        case USE_ALL_OBSTACLES:
            flags = 0;
            break;
        case IGNORE_SELECTED_ACTOR_OBSTACLES:
            flags = selectedActorObstacleFlags();
            break;
        case IGNORE_ALL_ACTOR_OBSTACLES:
            flags = MachLog::OBSTACLE_ALL;
            break;
    }

	bool keepTrying = true;

	while ( keepTrying and not valid )
	{
		NEIL_STREAM( "pointToCheckNear " << pointToCheckNear << std::endl );
		NEIL_STREAM( "pointToCheckFar" << pointToCheckFar << std::endl );

        PhysConfigSpace2d::DomainId domainId;

		// Check point approaching machines original point
	    if ( MachLogPlanet::instance().configSpace().domain( pointToCheckNear, &domainId ) )
	    {
			PhysConfigSpace2d::PolygonId nastyPolygonId;

	        if( MachLogPlanet::instance().configSpace().contains(	MexCircle2d( pointToCheckNear, clearance ),
	        														flags, &nastyPolygonId,
	        														PhysConfigSpace2d::USE_PERMANENT_ONLY ) )
	        {
	            valid = true;
				*pClosestPoint = pointToCheckNear;
	        }
		}

		// Check point moving away from machines original point
		if ( checkFar and not valid and MachLogPlanet::instance().configSpace().domain( pointToCheckFar, &domainId ) )
	    {
			PhysConfigSpace2d::PolygonId nastyPolygonId;

	        if( MachLogPlanet::instance().configSpace().contains(	MexCircle2d( pointToCheckFar, clearance ),
	        														flags, &nastyPolygonId,
	        														PhysConfigSpace2d::USE_PERMANENT_ONLY ) )
	        {
	            valid = true;
				*pClosestPoint = pointToCheckFar;
	        }
		}

		if ( not valid )
		{
			checkFar = true;

			distanceFromOriginalPoint += stepDistance;
			NEIL_STREAM( "distanceFromOriginalPoint " << distanceFromOriginalPoint << std::endl );

			if ( movingLine.length() < distanceFromOriginalPoint )
			{
				keepTrying = false;
				NEIL_STREAM( "terminating ( movingLine.length " << movingLine.length() << " ) " << std::endl );
			}
			else
			{
				pointToCheckNear = movingLine.pointAtDistance( movingLine.length() - distanceFromOriginalPoint );
				pointToCheckFar = movingLine.pointAtDistance( movingLine.length() + distanceFromOriginalPoint );
			}
		}
	}

	NEIL_STREAM( "MachGuiCommand::findClosestPointValidOnTerrain exit " << valid << std::endl );

    return valid;
}

// static
size_t& MachGuiCommand::commandId()
{
    static  size_t commandId_ = 0;

    return commandId_;
}

// virtual
bool MachGuiCommand::canApplyToGroup() const
{
    return false;
}

// virtual
bool MachGuiCommand::doGroupApply( const Actors&, string* )
{
    PRE( canApplyToGroup() );

    ASSERT_FAIL( "This function should only be called for operations where it has been overridden" );

    return false;
}

//  Return the bitwise OR of the obstacle flags for all selected
//  actors that are machines
MachGuiCommand::ObstacleFlags MachGuiCommand::selectedActorObstacleFlags() const
{
    ObstacleFlags   flags = 0;

    const MachInGameScreen::Actors& selectedActors = pImpl_->pInGameScreen_->selectedActors();

    for( MachInGameScreen::Actors::const_iterator i = selectedActors.begin(); i != selectedActors.end(); ++i )
    {
        const MachActor* pActor = *i;

        if( pActor->objectIsMachine() )
            flags |= pActor->asMachine().obstacleFlags();
    }

    return flags;
}

//virtual
bool MachGuiCommand::processButtonEvent( const DevButtonEvent& )
{
	return false;
}

//static
bool& MachGuiCommand::inFogOfWarFlag()
{
	static bool inFOW;

	return inFOW;
}

//static
bool MachGuiCommand::cursorInFogOfWar()
{
	return inFogOfWarFlag();
}

//static
void MachGuiCommand::cursorInFogOfWar( bool inFOW )
{
	inFogOfWarFlag() = inFOW;
}

void MachGuiCommand::add( MachLogMachine* pMachine, MachLogOperation* pOperation )
{
    pImpl_->add( MachLogMachineOperation( pMachine, pOperation ) );
}

PhysPathFindingPriority MachGuiCommand::pathFindingPriority() const
{
    PhysPathFindingPriority result = Phys::defaultPathFindingPriority() +
      MachPhysData::instance().generalData().pcPathFindingPriority();

    return result;
}

/* End COMMAND.CPP **************************************************/

