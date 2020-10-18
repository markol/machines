/*
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "stdlib/string.hpp"
#include "ctl/vector.hpp"
#include "machgui/hwrbicon.hpp"
#include "machgui/actbmpnm.hpp"
#include "machgui/buildbar.hpp"
#include "machgui/actnamid.hpp"
#include "machgui/ingame.hpp"
#include "machlog/resitem.hpp"
#include "gui/painter.hpp"
#include "gui/restring.hpp"
#include "machgui/internal/strings.hpp"
#include "machgui/internal/mgsndman.hpp"

typedef std::pair< SysPathName, SysPathName > SysPathNames;

/* ////////////////////////////////////////////// constructor /////////////////////////////////////////////////// */

MachGuiNewResearchIcon::MachGuiNewResearchIcon( GuiDisplayable *pParent,
												const Gui::Coord& rel,
												const GuiBitmap& bitmap )
: GuiButtonWithFilledBorder(	pParent,
								MachGuiNewResearchIcon::exteriorRelativeBoundary( bitmap, GuiBorderMetrics( 1, 1, 1 ), rel ),
								GuiBorderMetrics( 1, 1, 1 ),
								GuiFilledBorderColours( Gui::BLACK(),
														MachGui::OFFWHITE(),
														MachGui::ALMOSTBLACK(),
														Gui::RED() ),
								Gui::Coord( 1, 1 ) ),
  bitmap_( bitmap )
{
	// Intentionally Empty
}

/* /////////////////////////////////////////////// destructor /////////////////////////////////////////////////// */

MachGuiNewResearchIcon::~MachGuiNewResearchIcon()
{
	// Intentionally Empty
}

/* ////////////////////////////////////////////////////////////////////////////////////////////////////////////// */

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MachGuiNewResearchIcon::doDisplayInteriorEnabled( const Gui::Coord& abs )
{
	Gui::Coord absCopy( abs );
	absCopy.y( absCopy.y() + MachGuiBuildProgressBar::height() );

	GuiPainter::instance().blit( bitmap_, absCopy );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// static
Gui::Box MachGuiNewResearchIcon::exteriorRelativeBoundary( const GuiBitmap& bitmap, const GuiBorderMetrics& m, const Gui::Coord& rel )
{
	return Gui::Box( rel, m.totalHorizontalThickness() + bitmap.width(),
						m.totalVerticalThickness() + bitmap.height() + MachGuiBuildProgressBar::height() );
}







///////
// ***************************************** code for MachHWResearchBankIcon ******************************************
///////


#include "machphys/machphys.hpp"

/* ////////////////////////////////////////////// constructor /////////////////////////////////////////////////// */

MachHWResearchBankIcon::MachHWResearchBankIcon(	GuiDisplayable* pParent,
											    MachLogResearchItem* pResearchItem,
												MachInGameScreen *pInGameScreen,
												MachPhys::Race race )
:   MachGuiNewResearchIcon( 	pParent,
								Gui::Coord(0,0), //Will be relocated by icon sequence parent
					        	Gui::bitmap( SysPathName( MachActorBitmaps::name( pResearchItem->objectType(), pResearchItem->subType(), pResearchItem->hwLevel(), pResearchItem->weaponCombo(), race ) ) ) ),
	pInGameScreen_( pInGameScreen ),
	pResearchItem_( pResearchItem ),
	needsPromptUpdate_( false )
{

    TEST_INVARIANT;

	pProgressBar_ = _NEW( MachGuiBuildProgressBar(	this,
													Gui::Coord( 2, 2 ),
													width() - 4 ) );

	popupButton( false );

	//Sets its current level

    double percentComplete = 100.0 *
    	(double( pResearchItem->amountResearched( race ) ) /
         double( pResearchItem->researchCost() ));

	updateProgress( percentComplete );
}

/* /////////////////////////////////////////////// destructor /////////////////////////////////////////////////// */

MachHWResearchBankIcon::~MachHWResearchBankIcon()
{
    TEST_INVARIANT;

	//Clear the cursor prompt string
    pInGameScreen_->clearCursorPromptText();
}

/* ////////////////////////////////////////////////////////////////////////////////////////////////////////////// */

void MachHWResearchBankIcon::CLASS_INVARIANT
{
	INVARIANT( this != NULL );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ostream& operator <<( ostream& o, const MachHWResearchBankIcon& t )
{

    o << "MachHWResearchBankIcon " << (void*)&t << " start" << std::endl;
    o << "MachHWResearchBankIcon " << (void*)&t << " end" << std::endl;

    return o;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//virtual
void MachHWResearchBankIcon::doBeDepressed( const GuiMouseEvent& )
{
	MachGuiSoundManager::instance().playSound( "gui/sounds/igclick.wav" );

    pProgressBar_->depress( true );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//virtual
void MachHWResearchBankIcon::doBeReleased( const GuiMouseEvent& )
{
	MachGuiSoundManager::instance().playSound( "gui/sounds/igclick.wav" );

	pProgressBar_->depress( false );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//static
size_t MachHWResearchBankIcon::reqHeight()
{
	// TODO : Remove hard coded values
	return MachGuiBuildProgressBar::height() + 38 /* Bitmap height */ + 4 /* Border */;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//static
size_t MachHWResearchBankIcon::reqWidth()
{
	return 42; // TODO : Remove hard coded value
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MachHWResearchBankIcon::updateProgress( float complete )
{
	pProgressBar_->percentageComplete( complete );

	needsPromptUpdate_ = true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

MachLogResearchItem& MachHWResearchBankIcon::researchItem() const
{
	return *pResearchItem_;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//virtual
void MachHWResearchBankIcon::doHandleMouseEnterEvent( const GuiMouseEvent& mouseEvent )
{
	GuiButtonWithFilledBorder::doHandleMouseEnterEvent( mouseEvent );

	needsPromptUpdate_ = false;

	displayCursorPromptText();
}

void MachHWResearchBankIcon::displayCursorPromptText()
{
	GuiString prompt = MachLogActorStringIdRestorer::getActorPromptText( 	pResearchItem_->objectType(),
																			pResearchItem_->subType(),
																			pResearchItem_->weaponCombo(),
																			pResearchItem_->hwLevel(),
																			IDS_RESEARCHING_PROMPT,
																			IDS_RESEARCHING_WITH_WEAPON_PROMPT );

	// Add bmu cost and rp cost to end of prompt text
	char bmuBuffer[20];
	char rpBuffer[20];
//	itoa( pResearchItem_->buildingCost(), bmuBuffer, 10 );
//	itoa( pResearchItem_->researchCost(), rpBuffer, 10 );
    sprintf(bmuBuffer, "%d", pResearchItem_->buildingCost());
    sprintf(rpBuffer, "%d", pResearchItem_->researchCost());

	if ( pResearchItem_->buildingCost() != 0 )
	{
		GuiStrings strings;
		strings.push_back( GuiString( bmuBuffer ) );
		strings.push_back( GuiString( rpBuffer ) );
		GuiResourceString costText( IDS_COST_WITH_RP, strings );
		prompt += "\n" + costText.asString();
	}
	else
	{
		GuiResourceString costText( IDS_COST_RP, GuiString( rpBuffer ) );
		prompt += "\n" + costText.asString();
	}

 	// Percentage complete info
	char buffer[20];
	sprintf(buffer, "%d", (uint) pProgressBar_->percentageComplete());
//	GuiResourceString percentCompleteText( IDS_RESEARCHPERCENTAGECOMPLETE, GuiString( itoa( pProgressBar_->percentageComplete(), buffer, 10 ) ) );
	GuiResourceString percentCompleteText( IDS_RESEARCHPERCENTAGECOMPLETE, GuiString( buffer ) );
	prompt += ", " + percentCompleteText.asString();

    pInGameScreen_->cursorPromptText( prompt, not needsPromptUpdate_ );

	needsPromptUpdate_ = false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//virtual
void MachHWResearchBankIcon::doHandleMouseExitEvent( const GuiMouseEvent& mouseEvent )
{
    //Clear the cursor prompt string
    pInGameScreen_->clearCursorPromptText();

	GuiButtonWithFilledBorder::doHandleMouseExitEvent( mouseEvent );
}

//virtual
void MachHWResearchBankIcon::doHandleContainsMouseEvent( const GuiMouseEvent& mouseEvent )
{
	GuiButtonWithFilledBorder::doHandleContainsMouseEvent( mouseEvent );

	if ( needsPromptUpdate_ )
	{
		displayCursorPromptText();
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* End PRODICON.CPP *************************************************/
