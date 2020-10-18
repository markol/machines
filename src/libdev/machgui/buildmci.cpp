/*
 * B U I L D M C I . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machgui/buildmci.hpp"
#include "machgui/prodbank.hpp"
#include "machgui/actbmpnm.hpp"
#include "machgui/actnamid.hpp"
#include "machgui/ingame.hpp"
#include "machlog/actor.hpp"
#include "machlog/factory.hpp"
#include "machlog/resitem.hpp"
#include "machlog/cntrl_pc.hpp"
#include "machlog/races.hpp"
#include "gui/restring.hpp"
#include "machgui/internal/strings.hpp"
#include "machgui/internal/mgsndman.hpp"

MachBuildMachineIcon::MachBuildMachineIcon
(
    GuiDisplayable* pParent, MachInGameScreen* pInGameScreen,
    MachProductionBank* pProductionBank, MachLogFactory* pFactory,
    const MachLogResearchItem& item
)
:   GuiIcon
    (
        pParent,
        Gui::Coord(0,0), //Will be relocated by icon sequence parent
        SysPathName( MachActorBitmaps::name( item.objectType(), item.subType(), item.hwLevel(), item.weaponCombo(), MachLogRaces::instance().pcController().race() ) )
    ),
    pInGameScreen_( pInGameScreen ),
    pProductionBank_( pProductionBank ),
    pFactory_( pFactory ),
	researchItem_( item )
{
    TEST_INVARIANT;
}

MachBuildMachineIcon::~MachBuildMachineIcon()
{
    TEST_INVARIANT;

}

void MachBuildMachineIcon::CLASS_INVARIANT
{
	INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachBuildMachineIcon& t )
{

    o << "MachBuildMachineIcon " << (void*)&t << " start" << std::endl;
    o << "MachBuildMachineIcon " << (void*)&t << " end" << std::endl;

    return o;
}

//virtual
void MachBuildMachineIcon::doBeDepressed( const GuiMouseEvent& )
{
	MachGuiSoundManager::instance().playSound( "gui/sounds/igclick.wav" );
}

//virtual
void MachBuildMachineIcon::doBeReleased(const GuiMouseEvent& )
{
    //Add the item to the queue
    //TBD: Hardcode sware level of 1 should be obtained from research data once implemented
    pFactory_->buildMachine( researchItem_.objectType(), researchItem_.subType(), researchItem_.hwLevel(), researchItem_.swLevel( pFactory_->race() ), 0, researchItem_.weaponCombo() );

    //Update the icons
    pProductionBank_->updateQueueIcons();
}

//static
size_t MachBuildMachineIcon::reqWidth()
{
	return 42;  // TODO : remove hardcoded value
}

//static
size_t MachBuildMachineIcon::reqHeight()
{
	return 42; // TODO : remove hardcoded value
}

//virtual
void MachBuildMachineIcon::doHandleMouseEnterEvent( const GuiMouseEvent& mouseEvent )
{
	GuiIcon::doHandleMouseEnterEvent( mouseEvent );

	GuiString prompt = MachLogActorStringIdRestorer::getActorPromptText( 	researchItem_.objectType(),
																			researchItem_.subType(),
																			researchItem_.weaponCombo(),
																			researchItem_.hwLevel(),
																			IDS_BUILD_PROMPT,
																			IDS_BUILD_WITH_WEAPON_PROMPT );

	// Add bmu cost to end of prompt text
	char buffer[20];
//	itoa( researchItem_.factoryInstanceCost(), buffer, 10 );
	sprintf(buffer, "%d", researchItem_.factoryInstanceCost());
	GuiResourceString bmuCostText( IDS_COST, GuiString( buffer ) );

	prompt += "\n" + bmuCostText.asString();

	//Set the cursor prompt
    pInGameScreen_->cursorPromptText( prompt );
}

//virtual
void MachBuildMachineIcon::doHandleMouseExitEvent( const GuiMouseEvent& mouseEvent )
{
    //Clear the cursor prompt string
    pInGameScreen_->clearCursorPromptText();

	GuiIcon::doHandleMouseExitEvent( mouseEvent );
}

/* End BUILDMCI.CPP *************************************************/
