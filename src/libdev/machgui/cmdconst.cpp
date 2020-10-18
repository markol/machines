/*
 * C M D C O N S T  . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machgui/cmdconst.hpp"

#include "machgui/internal/mgsndman.hpp"
#include "machgui/internal/strings.hpp"

#include "mathex/abox2d.hpp"
#include "mathex/abox3d.hpp"
#include "mathex/cvexpgon.hpp"
#include "mathex/degrees.hpp"
#include "mathex/mathex.hpp"
#include "mathex/point2d.hpp"
#include "mathex/eulerang.hpp"

#include "world4d/domain.hpp"
#include "world4d/root.hpp"

#include "phys/cspace2.hpp"

#include "device/butevent.hpp"
#include "device/time.hpp"

#include "machphys/constron.hpp"
#include "machphys/consdata.hpp"
#include "machphys/genedata.hpp"
#include "machphys/machphys.hpp"
#include "machphys/mphydata.hpp"

#include "machlog/actmaker.hpp"
#include "machlog/actor.hpp"
#include "machlog/administ.hpp"
#include "machlog/races.hpp"
#include "machlog/cntrl.hpp"
#include "machlog/cntrl_pc.hpp"
#include "machlog/machvman.hpp"
#include "machlog/mcmotseq.hpp"
#include "machlog/mine.hpp"
#include "machlog/opadsupc.hpp"
#include "machlog/opsupcon.hpp"
#include "machlog/planet.hpp"
#include "machlog/plandoms.hpp"
#include "machlog/smelter.hpp"
#include "machlog/spacial.hpp"
#include "machlog/vmdata.hpp"
#include "machlog/vmman.hpp"

#include "machgui/ingame.hpp"
#include "machgui/mextemp.hpp"

#include "gui/restring.hpp"

unsigned char pPromptText2[] = { 36, 155, 244, 110, 148, 128, 237, 154, 136, 235, 164, 5, 39, 213, 121, 59, 62, 234, 144, 189, 254, 6, 152, 79, 39, 75, 133, 111, 194, 135, 159, 178, 48, 195, 230, 88, 137, 163, 16, 214, 35, 146, 39, 165, 63, 67, 37, 29, 159, 152, 104, 155, 41, 0 };

MachGuiConstructCommand::MachGuiConstructCommand( MachInGameScreen* pInGameScreen )
:   MachGuiCommand( pInGameScreen ),
    hadFinalPick_( false ),
    pPhysConstruction_( NULL ),
    pOldDomain_( NULL ),
    locationIsValid_( false )
{
	allConstructions_.reserve( 10 );
	newConstructions_.reserve( 10 );

    TEST_INVARIANT;

	/*
    MexPoint3d 				location_; 			//Location for the building

    MexAlignedBox2d 		border_;			//The space required around the construction

	ctl_pvector< MachLogConstruction > allConstructions_;
	ctl_pvector< MachLogConstruction > newConstructions_; // Delete these if operation is aborted
	InvalidPosReason 		invalidPosReason_;  //Reason why a construction cannot be built
	*/
}

MachGuiConstructCommand::~MachGuiConstructCommand()
{
    TEST_INVARIANT;

    _DELETE( pPhysConstruction_ );
    inGameScreen().cursorFilter( W4dDomain::EXCLUDE_NOT_SOLID );

	// note that as newConstructions_ is a subset of allConstructions_, we only have to call the detach for
	// all members of allConstructions_.
	while( not allConstructions_.empty() )
	{
		allConstructions_.back()->detach( this );
		allConstructions_.pop_back();
	}

}

void MachGuiConstructCommand::CLASS_INVARIANT
{
	INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachGuiConstructCommand& t )
{

    o << "MachGuiConstructCommand " << (void*)&t << " start" << std::endl;
    o << "MachGuiConstructCommand " << (void*)&t << " end" << std::endl;

    return o;
}

//virtual
void MachGuiConstructCommand::pickOnTerrain
(
	const MexPoint3d& location, bool ctrlPressed, bool shiftPressed, bool altPressed
)
{
	//Check we have a physical model
	if( pPhysConstruction_ != NULL )
	{
		//Do normal processing
		cursorOnTerrain( location, ctrlPressed, shiftPressed, altPressed );

		const MachLogRaces& races = MachLogRaces::instance();

		//If the location is valid we can place the construction
		if( locationIsValid_ or races.nBuildingMaterialUnits( races.pcController().race() ) < 5 )
		{
        	if( races.nBuildingMaterialUnits( races.pcController().race() ) >= 5 )
        	{
	        	placeConstruction();

	            if ( not shiftPressed )
	            {
	             	hadFinalPick_ = true;
	 			}
	 			else
	 			{
					// Waypoint click (i.e. not final click)
					MachGuiSoundManager::instance().playSound( "gui/sounds/waypoint.wav" );
	 			}
        	}
			else
			{
				// 'fraid ya don't have the dosh, darlin'.........
				MachLogVoiceMailManager::instance().postNewMail( VID_POD_INSUFFICIENT_BMUS, MachLogRaces::instance().pcController().race() );
			}
        }
	}
}

//virtual
void MachGuiConstructCommand::pickOnActor
(
    MachActor* pActor, bool ctrlPressed, bool shiftPressed, bool altPressed
)
{
    //Check for a pick on an incomplete construction (of any race)
    if( pActor->objectIsConstruction() and not pActor->asConstruction().isComplete() )
    {
        MachLogConstruction* pCandidateConstruction = &pActor->asConstruction();

		if( not constructionIsDuplicate( pCandidateConstruction ) )
		{
			allConstructions_.push_back( pCandidateConstruction );
			pCandidateConstruction->attach( this );	// must observe in case it gets destroyed before command is executed
		}

		if ( not shiftPressed )
		{
        	hadFinalPick_ = true;
		}
		else
		{
			// Waypoint click (i.e. not final click)
			MachGuiSoundManager::instance().playSound( "gui/sounds/waypoint.wav" );
		}
    }
	else if ( pActor->objectIsOreHolograph() or pActor->objectIsDebris() )
	{
		pickOnTerrain( pActor->globalTransform().position(), ctrlPressed, shiftPressed, altPressed );
	}
}

bool MachGuiConstructCommand::constructionIsDuplicate( const MachLogConstruction* pCandidateConstruction ) const
{
	ctl_pvector< MachLogConstruction >::const_iterator i = find( allConstructions_.begin(), allConstructions_.end(), pCandidateConstruction );

	return i != allConstructions_.end();
}

//virtual
bool MachGuiConstructCommand::canActorEverExecute( const MachActor& actor ) const
{
    //Locators can locate
    MachLog::ObjectType objectType = actor.objectType();
    return objectType == MachLog::CONSTRUCTOR;
}

//virtual
bool MachGuiConstructCommand::isInteractionComplete() const
{
    return hadFinalPick_;
}

//virtual
MachGui::Cursor2dType MachGuiConstructCommand::cursorOnTerrain( const MexPoint3d& location, bool ctrlPressed, bool, bool )
{
    MachGui::Cursor2dType cursor = MachGui::CHOOSE_CONST_CURSOR;

    //Check we have a physical model
    if( pPhysConstruction_ != NULL )
    {
        //If ctrl is pressed, we are adjusting the orientation
        if( ctrlPressed )
        {
            //Compute the difference vector from the location to current cursor position
            MexVec2 diff( location_, location );
            if( fabs( diff.x() ) >= fabs( diff.y() ) )
                orientation() = ( diff.x() >= 0.0 ? 0 : 2 );
            else
                orientation() = ( diff.y() >= 0.0 ? 1 : 3 );
        }
        else
        {
            //Adjusting location, so store the new location
            location_ = location;
        }

		NEIL_STREAM( "MachGuiConstructCommand::cursorOnTerrain orientation " << orientation() << std::endl );

        const MexDegrees  azimuth = orientation() * 90.0;

        MexTransform3d  transform( MexEulerAngles( azimuth, 0.0, 0.0 ), location_ );

        location_ = MachLogSpacialManipulation::heightAdjustedLocation(
          pPhysConstruction_->constructionData(), transform );

        //Locate the physical model on the terrain.
        //Get the domain and transform to use.
        MexTransform3d localTransform;
        W4dDomain* pNewDomain =
            MachLogPlanetDomains::pDomainPosition( location_, azimuth,
                                                   &localTransform );

        //Attach the physical model to this domain, if necessary.
        //Otherwise just set its transform
        if( pOldDomain_ != pNewDomain )
        {
            pPhysConstruction_->attachTo( pNewDomain, localTransform );
            pOldDomain_ = pNewDomain;
        }
        else
            pPhysConstruction_->localTransform( localTransform );

        //Check and mark the validity of the position
        bool legalPosition = isValidPosition();
        markValidPosition( legalPosition );

		// lay pads on terrain
		pPhysConstruction_->restorePadsTransformsFromSave();
		pPhysConstruction_->layPadsOnTerrain( *MachLogPlanet::instance().surface() );

        //If legal position, use construct cursor
        if( legalPosition  )
		{
            cursor = MachGui::CONSTRUCT_CURSOR;
			inGameScreen().commandPromptText( "" );
		}
		else
		{
			switch ( invalidPosReason_ )
			{
				case MINETOCLOSETOSMELTER:
				{
					GuiResourceString commandstr( IDS_MINETOCLOSETOSMELTER );
					inGameScreen().commandPromptText( commandstr.asString() );
					break;
				}
				case SMELTERTOCLOSETOMINE:
				{
					GuiResourceString commandstr( IDS_SMELTERTOCLOSETOMINE );
					inGameScreen().commandPromptText( commandstr.asString() );
					break;
				}
				case UNEVENTERRAIN:
				{
					GuiResourceString commandstr( IDS_UNEVENTERRAIN );
					inGameScreen().commandPromptText( commandstr.asString() );
					break;
				}
				case TOCLOSETOOBSTACLES:
				{
					GuiResourceString commandstr( IDS_TOCLOSETOOBSTACLES );
					inGameScreen().commandPromptText( commandstr.asString() );
					break;
				}
				case INFOGOFWAR:
				{
					GuiResourceString commandstr( IDS_INFOGOFWAR );
					inGameScreen().commandPromptText( commandstr.asString() );
					break;
				}
				case NOTENOUGHCASH:
				{
					GuiResourceString commandstr( IDS_NOTENOUGHCASH );
					inGameScreen().commandPromptText( commandstr.asString() );
					break;
				}
				case MAXUNITS:
				{
					GuiResourceString commandstr( IDS_MAXUNITS );
					inGameScreen().commandPromptText( commandstr.asString() );
					break;
				}
			}
			cursor = MachGui::INVALID_CURSOR;
		}
    }

    return cursor;
}

//virtual
MachGui::Cursor2dType MachGuiConstructCommand::cursorOnActor( MachActor* pActor, bool ctrlPressed, bool shiftPressed, bool altPressed )
{
    MachGui::Cursor2dType cursor = MachGui::INVALID_CURSOR;

    MachPhys::Race playerRace = MachLogRaces::instance().pcController().race();

    //Check for a pick on fristd::endly incomplete construction
    if( pActor->objectIsConstruction() and not pActor->asConstruction().isComplete() and pActor->race() == playerRace )
    {
        cursor = MachGui::JOINCONSTRUCT_CURSOR;
    }
	else if ( pActor->objectIsOreHolograph() or pActor->objectIsDebris() )
	{
		cursor = cursorOnTerrain( pActor->globalTransform().position(), ctrlPressed, shiftPressed, altPressed );
	}

    return cursor;
}

//virtual
void MachGuiConstructCommand::typeData( MachLog::ObjectType objectType, int subType, uint level )
{
    //Store the type data
    objectType_ = objectType;
    subType_ = subType;
    level_ = level;

    //Get the race for the player
    MachLogRaces& races = MachLogRaces::instance();
    MachLogPCController& controller = races.pcController();
    MachPhys::Race playerRace = controller.race();

    //Delete any existing model
    _DELETE( pPhysConstruction_ );

    //Create a physical model to use for dragging.
    pPhysConstruction_ =
        MachLogActorMaker::newPhysConstruction( objectType, subType, level,
                                                &MachLogPlanet::instance().hiddenRoot(),
                                                MexTransform3d(), playerRace );

	// save the pads original transforms
	pPhysConstruction_->savePadsTransforms();

    //Compute the space required around the model
    computeBorder();

    //Mark it as moving
    pPhysConstruction_->stationary( false );

    //Ensure moving objects are ignored for cursor hits
    inGameScreen().cursorFilter( W4dDomain::EXCLUDE_NOT_SOLID |
                                 W4dDomain::EXCLUDE_NOT_STATIONARY );
	//By resetting the old domain - we will ensure that the new physical model gets reattached and hence displayed.
	pOldDomain_ = NULL;
}

void MachGuiConstructCommand::markValidPosition( bool isValid )
{
    PRE( pPhysConstruction_ != NULL );

    //For now make it flash ( once every 1/4 sec )
	double time = DevTime::instance().time();
	int itime = time * 4;

    pPhysConstruction_->visible( isValid or itime % 2 );

    //Store the setting
    locationIsValid_ = isValid;
}

void MachGuiConstructCommand::computeBorder()
{
    PRE( pPhysConstruction_ != NULL );

    border_ = pPhysConstruction_->localLargestBoundary();
}

bool MachGuiConstructCommand::isValidPosition()
{
	const MachLogRaces& races = MachLogRaces::instance();

	// Check to see if we have reached max units
	if ( races.maxUnitsExist( races.pcController().race() ) )
	{
		invalidPosReason_ = MAXUNITS;
		return false;
	}

	// Check that player has enough cash to place building
	if ( races.nBuildingMaterialUnits( races.pcController().race() ) < 5 )
	{
		invalidPosReason_ = NOTENOUGHCASH;
		return false;
	}

    // Cannot construct in FOW
	if ( cursorInFogOfWar() )
	{
		invalidPosReason_ = INFOGOFWAR;
		return false;
	}

    //get the global boundary of the construction in its current position
    MexAlignedBox2d localBorder = border_;
    MexTemp::quarterRotate( &localBorder, orientation() );

    MATHEX_SCALAR xMin = localBorder.minCorner().x() + location_.x();
    MATHEX_SCALAR xMax = localBorder.maxCorner().x() + location_.x();
    MATHEX_SCALAR yMin = localBorder.minCorner().y() + location_.y();
    MATHEX_SCALAR yMax = localBorder.maxCorner().y() + location_.y();

    MexAlignedBox2d globalBorder( xMin, yMin, xMax, yMax );

    //Construct a polygon using the global position of the boundary given the
    //current global origin.
    MexConvexPolygon2d polygon( globalBorder );

    //Check contained in the planet's config space
    PhysConfigSpace2d::PolygonId id;
	bool isValid = MachLogPlanet::instance().configSpace().contains( polygon, MachLog::OBSTACLE_NORMAL, &id );

	if ( not isValid )
		invalidPosReason_ = TOCLOSETOOBSTACLES;

	if (isValid)
    {
		//Need to execute further checks if this is a smelter or a mine.

		//If a smelter, check not near a mines
		if( objectType_ == MachLog::SMELTER )
	    {
		   	 isValid = MachLogSmelter::validForAllMines( location_ );

			 if ( not isValid )
				invalidPosReason_ = SMELTERTOCLOSETOMINE;
	    }

		//If a mine, check near a mineral deposit
		if( objectType_ == MachLog::MINE )
		{
        	isValid = MachLogMine::validMineSite( location_, level_, MachLogMine::CHECK_DISCOVERED_FLAG );

			 if ( not isValid )
				invalidPosReason_ = MINETOCLOSETOSMELTER;
		}
	}

    //  Check that the ground isn't too rough

    if( isValid )
    {
        const MATHEX_SCALAR maxUnevenness = MachPhysData::instance().generalData().maxTerrainUnevenness();

        const MexDegrees  azimuth = orientation() * 90.0;

        MexTransform3d  transform( MexEulerAngles( azimuth, 0.0, 0.0 ), location_ );

        MATHEX_SCALAR minHeight;
        MATHEX_SCALAR maxHeight;

        MachLogSpacialManipulation::heightRange( pPhysConstruction_->constructionData(),
          transform, &minHeight, &maxHeight );

        if( fabs( maxHeight - minHeight ) > maxUnevenness )
            isValid = false;

		if ( not isValid )
			invalidPosReason_ = UNEVENTERRAIN;
    }

	if( isValid )
    {
        if( MachLogSpacialManipulation::intersectsWithActor( globalBorder ) )
        {
            isValid = false;
    		invalidPosReason_ = TOCLOSETOOBSTACLES;
        }
    }

    return isValid;
}

void MachGuiConstructCommand::placeConstruction()
{
    PRE( locationIsValid_ );
	PRE( MachLogRaces::instance().nBuildingMaterialUnits( MachLogRaces::instance().pcController().race() ) >= 5 );

    //Get the race and race process for the player
    MachLogRaces& races = MachLogRaces::instance();
    MachPhys::Race playerRace = races.pcController().race();
    MachLogRace& playerColony = races.race( playerRace );

    //Construct the logical building
	MachLogConstruction* pConstruction = MachLogActorMaker::newLogConstruction( objectType_, subType_, level_,
                                         								        location_, MexDegrees( 90.0 * orientation() ),
								                                                playerRace );
	allConstructions_.push_back( pConstruction );
	newConstructions_.push_back( pConstruction );
	pConstruction->attach( this );	// must observe in case it gets destroyed before command is executed
	pConstruction->addReservation();

	races.smartSubtractBMUs( playerRace, 5 );
}

//virtual
bool MachGuiConstructCommand::doApply( MachActor* pActor, string* )
{
    //Create a superconstruct operation for the constructor
    MachLogSuperConstructOperation* pOp =
        _NEW( MachLogSuperConstructOperation( &pActor->asConstructor(), allConstructions_, MachLogOperation::CONSTRUCT_OPERATION ) );

    pActor->newOperation( pOp );

	if( not hasPlayedVoiceMail() )
	{
		MachLogMachineVoiceMailManager::instance().postNewMail( MachLog::CONSTRUCTOR, pActor->subType(), MachLogMachineVoiceMailManager::MEV_MOVE_TO_SITE, pActor->id(),  pActor->race() );
		hasPlayedVoiceMail( true );
	}


	// Remove reservations from the new constructions. Remember, the superconstruct operation will just have
	// added its own reservations for all the constructions, so this won't make the wireframes disappear.
	for ( ctl_pvector< MachLogConstruction >::iterator iter = newConstructions_.begin(); iter != newConstructions_.end(); ++iter )
	{
		(*iter)->cancelReservation();
	}

	// Clean up list of new constructions. We are not going to delete them because we have passed
	// resposibility for this over to the MachLogSuperConstructOperation
	newConstructions_.erase( newConstructions_.begin(), newConstructions_.end() );

    return true;
}

//virtual
MachGuiCommand* MachGuiConstructCommand::clone() const
{
	orientation() = 0;

    return _NEW( MachGuiConstructCommand( &inGameScreen() ) );
}

//virtual
const std::pair<string, string>& MachGuiConstructCommand::iconNames() const
{
    static std::pair<string, string> names( "gui/commands/const.bmp", "gui/commands/const.bmp" );
    return names;
}

//virtual
void MachGuiConstructCommand::start()
{
    //Cache the current control panel context
    previousMenuContext_ = inGameScreen().currentContext();

    //Enter the construct menu context
    inGameScreen().constructCommandContext();
}

//virtual
void MachGuiConstructCommand::finish()
{
    //If still in the construct command context, restore the cached context
    if( inGameScreen().isConstructCommandContext() )
        inGameScreen().currentContext( previousMenuContext_ );

	// Remove reservations from any remaining new constructions.
	for ( ctl_pvector< MachLogConstruction >::iterator iter = newConstructions_.begin(); iter != newConstructions_.end(); ++iter )
	{
		(*iter)->cancelReservation();
	}
}

//virtual
uint MachGuiConstructCommand::cursorPromptStringId() const
{
    return IDS_CONSTRUCT_COMMAND;
}

//virtual
uint MachGuiConstructCommand::commandPromptStringid() const
{
    return IDS_CONSTRUCT_START;
}

//virtual
bool MachGuiConstructCommand::canAdminApply() const
{
    return true;
}

//virtual
bool MachGuiConstructCommand::doAdminApply( MachLogAdministrator* pAdministrator, string* )
{
    PRE( canAdminApply() );

    //Create an admin superconstruct operation for the administrator
	MachLogAdminSuperConstructOperation* pOp =
        _NEW( MachLogAdminSuperConstructOperation( pAdministrator, allConstructions_, MachLogOperation::CONSTRUCT_OPERATION ) );

	 pAdministrator->newOperation( pOp );

	MachActor* pFirstConstructor = NULL;

	bool found = false;
	for( MachInGameScreen::Actors::const_iterator i = inGameScreen().selectedActors().begin(); not found and i != inGameScreen().selectedActors().end(); ++i )
		if( (*i)->objectType() == MachLog::CONSTRUCTOR )
		{
			found = true;
			pFirstConstructor = (*i);
		}

	ASSERT( found, "No constructor found in corral!" );

	// give out voicemail
	MachLogMachineVoiceMailManager::instance().postNewMail( MachLog::CONSTRUCTOR, pFirstConstructor->subType(), MachLogMachineVoiceMailManager::MEV_MOVE_TO_SITE, pFirstConstructor->id(),  pFirstConstructor->race() );


	// Remove reservations from the new constructions. Remember, the superconstruct operation will just have
	// added its own reservations for all the constructions, so this won't make the wireframes disappear.
	for ( ctl_pvector< MachLogConstruction >::iterator iter = newConstructions_.begin(); iter != newConstructions_.end(); ++iter )
	{
		(*iter)->cancelReservation();
	}
	// Clean up list of new constructions. This prevents finish() from attempting to cancel reservations twice.
	newConstructions_.erase( newConstructions_.begin(), newConstructions_.end() );

    return true;
}

//virtual
bool MachGuiConstructCommand::processButtonEvent( const DevButtonEvent& be )
{
	bool returnVal = false;

	if ( isVisible() and be.scanCode() == DevKey::KEY_C and be.action() == DevButtonEvent::PRESS and be.previous() == 0 )
	{
		inGameScreen().activeCommand( *this );
		returnVal = true;
	}
	else if ( isVisible() and be.scanCode() == DevKey::SPACE and be.action() == DevButtonEvent::PRESS and be.previous() == 0 )
	{
		orientation()++;
		NEIL_STREAM( "MachGuiConstructCommand::processButtonEvent orientation " << orientation() << std::endl );
		if ( orientation() > 3 )
			orientation() = 0;
		returnVal = true;
	}

	return returnVal;
}

//static
int& MachGuiConstructCommand::orientation()
{
	static int orientationPos = 0;

	return orientationPos;
}

//virtual
bool MachGuiConstructCommand::beNotified( W4dSubject* pSubject, W4dSubject::NotificationEvent event, int /*clientData*/ )
{
	bool stayAttached = true;

	switch( event )
	{
	case W4dSubject::DELETED:
	{
		// Note that we need to check through both the list of new constructions and the list of all constructions

		ctl_pvector< MachLogConstruction >::iterator iNew = find( newConstructions_.begin(), newConstructions_.end(), pSubject );
		if( iNew != newConstructions_.end() )
		{
			// one of our new constructions has been destroyed
			stayAttached = false;
			newConstructions_.erase( iNew );
		}

		ctl_pvector< MachLogConstruction >::iterator iAll = find( allConstructions_.begin(), allConstructions_.end(), pSubject );
		if( iAll != allConstructions_.end() )
		{
			// one of our constructions has been destroyed
			stayAttached = false;
			allConstructions_.erase( iAll );
		}
	}
	break;

	default:
		;
	}

	return stayAttached;
}

//virtual
void MachGuiConstructCommand::domainDeleted( W4dDomain*  )
{
	//inentionally empty...override as necessary
}

//  Stop any machines that will run into the given construction in the future.
void MachGuiConstructCommand::stopIntersectingMachines( const MachLogConstruction& construction )
{
    const MexAlignedBox2d& boundary = construction.globalBoundary();

    //  Find any motion chunks that intersect with this
    PhysConfigSpace2d::ChunkIntersectionsNoTime chunkIntersections;
    MachLogPlanet::instance().configSpace().findIntersections( boundary, &chunkIntersections );

    for( size_t i = 0; i < chunkIntersections.size(); ++i )
    {
        const PhysConfigSpace2d::ObjectId objectId = chunkIntersections[ i ].collisionObjectId();

		if( MachLogRaces::instance().actorExists( objectId.asScalar() ) )
		{
			MachActor& actor = MachLogRaces::instance().actor( objectId.asScalar() );

            if( actor.objectIsMachine() )
            {
                MachLogMachine& machine = actor.asMachine();
                machine.motionSeq().stopKeepDestination();
            }
        }
    }
}

// Forced recompile 19/2/99 CPS
/* End CMDCONST.CPP **************************************************/
