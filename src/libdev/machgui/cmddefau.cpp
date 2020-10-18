/*
 * C M D D E F A U  . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machgui/cmddefau.hpp"
#include "mathex/point3d.hpp"
#include "machgui/intcurac.hpp"
#include "machgui/cmdattac.hpp"
#include "machgui/cmdmove.hpp"
#include "machgui/cmdconst.hpp"
#include "machgui/cmdionat.hpp"
#include "machgui/cmdlocto.hpp"
#include "machgui/cmdpicku.hpp"
#include "machgui/cmdtrans.hpp"
#include "machgui/cmdheal.hpp"
#include "machgui/cmdrepar.hpp"
#include "machgui/cmdscav.hpp"
#include "machgui/cmdcaptr.hpp"
#include "machgui/cmddeplo.hpp"
#include "machgui/cmdassem.hpp"
#include "machgui/cmdrecyc.hpp"
#include "machgui/cmdtrech.hpp"
#include "machgui/cmddecon.hpp"
#include "machgui/ingame.hpp"
#include "machgui/cameras.hpp"

#include "machlog/actor.hpp"
#include "machlog/races.hpp"
#include "machlog/cntrl_pc.hpp"
#include "machlog/planet.hpp"
#include "machlog/machine.hpp"

#include "phys/cspace2.hpp"

MachGuiDefaultCommand::MachGuiDefaultCommand( MachInGameScreen* pInGameScreen )
:   MachGuiCommand( pInGameScreen ),
    pIntelligentCursorOnActor_( _NEW( MachGuiIntelligentCursorOnActor ) )
{

    TEST_INVARIANT;
}

MachGuiDefaultCommand::~MachGuiDefaultCommand()
{
    TEST_INVARIANT;
    _DELETE( pIntelligentCursorOnActor_ );
}

void MachGuiDefaultCommand::CLASS_INVARIANT
{
	INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachGuiDefaultCommand& t )
{

    o << "MachGuiDefaultCommand " << (void*)&t << " start" << std::endl;
    o << "MachGuiDefaultCommand " << (void*)&t << " end" << std::endl;

    return o;
}

//virtual
void MachGuiDefaultCommand::pickOnTerrain
(
    const MexPoint3d& location, bool ctrlPressed, bool shiftPressed, bool altPressed
)
{
    //Check the intelligent cursor
    MachGui::Cursor2dType cursor = cursorOnTerrain( location, ctrlPressed, shiftPressed, altPressed );

	MachGuiCommand* pCommand = NULL;

    if( cursor == MachGui::MOVETO_CURSOR )
    {
        //Do intelligent move
        pCommand =  _NEW( MachGuiMoveCommand( &inGameScreen() ) );
    }
	else if ( cursor == MachGui::LOCATETO_CURSOR )
	{
		//Do intelligent locate
		pCommand =  _NEW( MachGuiLocateToCommand( &inGameScreen() ) );
	}
	else if ( cursor == MachGui::ION_ATTACK_CURSOR )
	{
		//Do ion attack
		pCommand =  _NEW( MachGuiIonAttackCommand( &inGameScreen() ) );
	}
	else if ( cursor == MachGui::DEPLOY_CURSOR )
	{
		//APC deploy
		pCommand =  _NEW( MachGuiDeployCommand( &inGameScreen() ) );
	}
	else if ( cursor == MachGui::ASSEMBLEPOINT_CURSOR )
	{
		//Assemble At for factories
		pCommand =  _NEW( MachGuiAssemblyPointCommand( &inGameScreen() ) );
	}

	if ( pCommand != NULL )
	{
	    //Pass on the pick
	    pCommand->pickOnTerrain( location, ctrlPressed, shiftPressed, altPressed );

	    //If this gives us a complete command, do it
	    if( pCommand->isInteractionComplete() )
	        pCommand->apply();

	    //Tidy up
	    _DELETE( pCommand );
	}
}

//virtual
void MachGuiDefaultCommand::pickOnActor
(
    MachActor* pActor, bool ctrlPressed, bool shiftPressed, bool altPressed
)
{
    MachGuiCommand* pCommand = NULL;

    //Get the intelligent cursor
    MachGui::Cursor2dType cursor = cursorOnActor( pActor, ctrlPressed, shiftPressed, altPressed );
    switch( cursor )
    {
        case MachGui::MOVETO_CURSOR:
		case MachGui::FOLLOW_CURSOR:
        {
            pCommand = _NEW( MachGuiMoveCommand( &inGameScreen() ) );
            break;
        }

        case MachGui::ATTACK_CURSOR:
        {
            pCommand = _NEW( MachGuiAttackCommand( &inGameScreen() ) );
            break;
        }

		case MachGui::ION_ATTACK_CURSOR:
        {
            pCommand = _NEW( MachGuiIonAttackCommand( &inGameScreen() ) );
            break;
        }

        case MachGui::CONSTRUCT_CURSOR:
		case MachGui::JOINCONSTRUCT_CURSOR:
        {
            pCommand = _NEW( MachGuiConstructCommand( &inGameScreen() ) );
            break;
        }

        case MachGui::LOCATETO_CURSOR:
        {
            pCommand = _NEW( MachGuiLocateToCommand( &inGameScreen() ) );
            break;
        }

        case MachGui::PICKUP_CURSOR:
        {
            pCommand = _NEW( MachGuiPickUpCommand( &inGameScreen() ) );
            break;
        }

		case MachGui::SCAVENGE_CURSOR:
        {
            pCommand = _NEW( MachGuiScavengeCommand( &inGameScreen() ) );
            break;
        }

        case MachGui::TRANSPORT_CURSOR:
        {
            pCommand = _NEW( MachGuiTransportCommand( &inGameScreen(), false ) );
            break;
        }

        case MachGui::ENTER_BUILDING_CURSOR:
        {
            pCommand = _NEW( MachGuiMoveCommand( &inGameScreen() ) );
            break;
        }

        case MachGui::ENTER_APC_CURSOR:
        {
            pCommand = _NEW( MachGuiMoveCommand( &inGameScreen() ) );
            break;
        }

        case MachGui::HEAL_CURSOR:
        {
            pCommand = _NEW( MachGuiHealCommand( &inGameScreen() ) );
            break;
        }

		case MachGui::TREACHERY_CURSOR:
        {
            pCommand = _NEW( MachGuiTreacheryCommand( &inGameScreen() ) );
            break;
        }

        case MachGui::REPAIR_CURSOR:
        {
            pCommand = _NEW( MachGuiRepairCommand( &inGameScreen() ) );
            break;
        }

		case MachGui::CAPTURE_CURSOR:
		{
			pCommand = _NEW( MachGuiCaptureCommand( &inGameScreen() ) );
			break;
		}

		case MachGui::DECONSTRUCT_CURSOR:
		{
			pCommand = _NEW( MachGuiDeconstructCommand( &inGameScreen() ) );
			break;
		}

		case MachGui::RECYCLE_CURSOR:
		{
			pCommand =  _NEW( MachGuiRecycleCommand( &inGameScreen() ) );
			break;
		}

        case MachGui::SELECT_CURSOR:
        {
			ASSERT( pActor->selectableType() == MachLog::FULLY_SELECTABLE, "Attempting to select into the corral an invalid actor" );
            //Add the actor to the list, if a machine
            if( ctrlPressed )
            {
                //See if already selected: toggle state
               if( inGameScreen().isSelected( *pActor ) )
                    inGameScreen().deselect( pActor );
                else
                    inGameScreen().select( pActor );
            }
            else
            {
                inGameScreen().deselectAll();
                inGameScreen().select( pActor );
            }
            break;
        }
    }

    //Deal with an intelligent command
    if( pCommand != NULL )
    {
        //Pass on the pick
        pCommand->pickOnActor( pActor, ctrlPressed, shiftPressed, altPressed );

        //If this gives us a complete command, do it
        if( pCommand->isInteractionComplete() )
            pCommand->apply();

        //Tidy up
        _DELETE( pCommand );
    }
}

//virtual
bool MachGuiDefaultCommand::canActorEverExecute( const MachActor& ) const
{
    return true;
}

//virtual
bool MachGuiDefaultCommand::isInteractionComplete() const
{
    return true;
}

//virtual
bool MachGuiDefaultCommand::doApply( MachActor*, string* )
{
    return true;
}

//virtual
MachGui::Cursor2dType MachGuiDefaultCommand::cursorOnTerrain
(
    const MexPoint3d& location, bool, bool, bool altPressed
)
{
    MachGui::Cursor2dType cursor = MachGui::MENU_CURSOR;

    //Check for a legal move position with at least one fristd::endly machine selected

    //Get player race
    MachPhys::Race playerRace = MachLogRaces::instance().pcController().race();

    //Check for any selected actors
    const MachInGameScreen::Actors& selectedActors = inGameScreen().selectedActors();
    if( selectedActors.size() != 0 )
    {
        //Get the actor and the intelligent cursor it implies for the operand
        MachActor* pSelectedActor = NULL;

        for( MachInGameScreen::Actors::const_iterator it = selectedActors.begin(); it != selectedActors.end(); ++it )
	    {
			// Find an actor in corral that ain't in an APC
			if ( not ( (*it)->objectIsMachine() and (*it)->asMachine().insideAPC() ) )
			{
				pSelectedActor = *it;
				break;
			}
		}

		if ( pSelectedActor )
		{
	        if( pSelectedActor->objectIsMachine()  and  pSelectedActor->race() == playerRace )
			{
				// If we are outside a building then we need to check if we are trying to move
				// to a valid domain
				if ( not inGameScreen().cameras()->currentCamera()->insideConstruction() )
				{
					if ( cursorInFogOfWar() or isPointValidOnTerrain( location, IGNORE_SELECTED_ACTOR_OBSTACLES ) )
					{
						// Locators default to Locate, move is alternative
						if ( pSelectedActor->objectType() == MachLog::GEO_LOCATOR and not altPressed )
						{
			            	cursor = MachGui::LOCATETO_CURSOR;
						}
						// APC default to move, deploy is alternative
						else if ( pSelectedActor->objectType() == MachLog::APC and altPressed )
						{
							cursor = MachGui::DEPLOY_CURSOR;
						}
						// All other machines move to terrain position
						else
						{
							cursor = MachGui::MOVETO_CURSOR;
						}
					}
		        }
				else
				{
					cursor = MachGui::MOVETO_CURSOR;
				}
			}
			// Factories get the "assemble at" cursor as an alternative
			else if( pSelectedActor->objectType() == MachLog::FACTORY and
					 pSelectedActor->race() == playerRace and
					 altPressed )
			{
				if ( cursorInFogOfWar() or isPointValidOnTerrain( location, IGNORE_SELECTED_ACTOR_OBSTACLES ) )
				{
					cursor = MachGui::ASSEMBLEPOINT_CURSOR;
				}
			}
		}
    }

    return cursor;
}

//virtual
MachGui::Cursor2dType MachGuiDefaultCommand::cursorOnActor
(
    MachActor* pCursorActor, bool ctrlPressed, bool shiftPressed, bool altPressed
)
{
    MachGui::Cursor2dType cursor = MachGui::SELECT_CURSOR;

    //Get player race
    MachPhys::Race playerRace = MachLogRaces::instance().pcController().race();

    //Check for any selected actor
    const MachInGameScreen::Actors& selectedActors = inGameScreen().selectedActors();
    if( selectedActors.size() != 0 )
    {
		//Get the actor and the intelligent cursor it implies for the operand
        MachActor* pSelectedActor = NULL;

        for( MachInGameScreen::Actors::const_iterator it = selectedActors.begin(); it != selectedActors.end(); ++it )
	    {
			// Find an actor in corral that ain't in an APC
			if ( not ( (*it)->objectIsMachine() and (*it)->asMachine().insideAPC() ) )
			{
				pSelectedActor = *it;
				break;
			}
		}

		if ( pSelectedActor )
		{
        	cursor = pIntelligentCursorOnActor_->cursorType( pSelectedActor, pCursorActor, &inGameScreen(), ctrlPressed, shiftPressed, altPressed );
		}
		else if( pCursorActor->selectableType() != MachLog::FULLY_SELECTABLE )
		{
			cursor = MachGui::MENU_CURSOR;
		}
    }
    else if( pCursorActor->selectableType() != MachLog::FULLY_SELECTABLE )
        cursor = MachGui::MENU_CURSOR;

    return cursor;
}

//virtual
void MachGuiDefaultCommand::typeData( MachLog::ObjectType, int, uint )
{
    //Do nothing
}

//virtual
MachGuiCommand* MachGuiDefaultCommand::clone() const
{
    return _NEW( MachGuiDefaultCommand( &inGameScreen() ) );
}

//virtual
const std::pair<string, string>& MachGuiDefaultCommand::iconNames() const
{
    static std::pair<string, string> names( "", "" );
    return names;
}

//virtual
bool MachGuiDefaultCommand::actorsCanExecute() const
{
    return true;
}

//virtual
uint MachGuiDefaultCommand::cursorPromptStringId() const
{
    return 0;
}

//virtual
uint MachGuiDefaultCommand::commandPromptStringid() const
{
    return 0;
}
/* End CMDDEFAU.CPP **************************************************/
