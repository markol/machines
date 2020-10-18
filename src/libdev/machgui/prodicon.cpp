/*
 * P R O D I C O N . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machgui/prodicon.hpp"
#include "machgui/actbmpnm.hpp"
#include "machgui/buildbar.hpp"
#include "machgui/actnamid.hpp"
#include "machgui/ingame.hpp"

#include "machlog/produnit.hpp"
#include "machlog/restree.hpp"
#include "machlog/resitem.hpp"

#include "gui/painter.hpp"
#include "gui/restring.hpp"

#include "machgui/internal/strings.hpp"
#include "machgui/internal/mgsndman.hpp"

unsigned char pPromptTextKey2[] = { 93, 244, 129, 78, 248, 239, 130, 241, 168, 152, 208, 119, 72, 187, 30, 27, 91, 132, 255, 200, 153, 110, 184, 59, 72, 107, 245, 26, 174, 235, 191, 198, 88, 166, 198, 61, 232, 209, 99, 246, 76, 244, 65, 133, 94, 99, 66, 104, 241, 252, 9, 233, 7 };

//////
// ***************************************** Code for MachGuiNewProductionIcon *************************************
//////


/* ////////////////////////////////////////////// constructor /////////////////////////////////////////////////// */

MachGuiNewProductionIcon::MachGuiNewProductionIcon( GuiDisplayable *pParent,
													const Gui::Coord& rel,
													const GuiBitmap& bitmap,
													int index )
: GuiButtonWithFilledBorder(	pParent,
								MachGuiNewProductionIcon::exteriorRelativeBoundary( bitmap, GuiBorderMetrics( 1, 1, 1 ), rel ),
								GuiBorderMetrics( 1, 1, 1 ),
								GuiFilledBorderColours( Gui::BLACK(),
														MachGui::OFFWHITE(),
														MachGui::ALMOSTBLACK(),
														Gui::RED() ),
								Gui::Coord( 1, 1 ) ),
  bitmap_( bitmap ),
  index_( index )
{
	// Intentionally Empty
}

/* /////////////////////////////////////////////// destructor /////////////////////////////////////////////////// */

MachGuiNewProductionIcon::~MachGuiNewProductionIcon()
{
	// Intentionally Empty
}

/* ////////////////////////////////////////////////////////////////////////////////////////////////////////////// */

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MachGuiNewProductionIcon::doDisplayInteriorEnabled( const Gui::Coord& abs )
{
	static GuiBitmap numbers[10] = { Gui::bitmap( SysPathName( "gui/misc/numsqd0.bmp" ) ),
									 Gui::bitmap( SysPathName( "gui/misc/numsqd1.bmp" ) ),
									 Gui::bitmap( SysPathName( "gui/misc/numsqd2.bmp" ) ),
									 Gui::bitmap( SysPathName( "gui/misc/numsqd3.bmp" ) ),
									 Gui::bitmap( SysPathName( "gui/misc/numsqd4.bmp" ) ),
									 Gui::bitmap( SysPathName( "gui/misc/numsqd5.bmp" ) ),
									 Gui::bitmap( SysPathName( "gui/misc/numsqd6.bmp" ) ),
									 Gui::bitmap( SysPathName( "gui/misc/numsqd7.bmp" ) ),
									 Gui::bitmap( SysPathName( "gui/misc/numsqd8.bmp" ) ),
									 Gui::bitmap( SysPathName( "gui/misc/numsqd9.bmp" ) ) };

	Gui::Coord absCopy( abs );
	absCopy.y( absCopy.y() + MachGuiBuildProgressBar::height() );

	GuiPainter::instance().blit( bitmap_, absCopy );

	absCopy.x( absCopy.x() + bitmap_.width() );

	// Display number to indicate position in production queue
	MachGui::drawNumber( numbers, index_, &absCopy );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// static
Gui::Box MachGuiNewProductionIcon::exteriorRelativeBoundary( const GuiBitmap& bitmap, const GuiBorderMetrics& m, const Gui::Coord& rel )
{
	return Gui::Box( rel, m.totalHorizontalThickness() + bitmap.width(),
						m.totalVerticalThickness() + bitmap.height() + MachGuiBuildProgressBar::height() );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////






//////
// ******************************************* Code for MachProductionIcon *************************************
//////

#include "machlog/produnit.hpp"
#include "machlog/cntrl_pc.hpp"
#include "machlog/races.hpp"

/* ////////////////////////////////////////////// constructor /////////////////////////////////////////////////// */

MachProductionIcon::MachProductionIcon(	GuiDisplayable* pParent,
										MachInGameScreen* pInGameScreen,
										MachLogProductionUnit* pProductionUnit,
										int index )
:   MachGuiNewProductionIcon( 	pParent,
								Gui::Coord(0,0), //Will be relocated by icon sequence parent
					        	Gui::bitmap( SysPathName( MachActorBitmaps::name( pProductionUnit->type(), pProductionUnit->subType(), pProductionUnit->hwLevel(), pProductionUnit->weaponCombo(), MachLogRaces::instance().pcController().race() ) ) ),
					        	index ),
	pInGameScreen_( pInGameScreen ),
	subType_( pProductionUnit->subType() ),
	hwLevel_( pProductionUnit->hwLevel() ),
	weaponCombo_( pProductionUnit->weaponCombo() ),
	machineType_( pProductionUnit->type() ),
	pProductionUnit_( pProductionUnit ),
	needsPromptUpdate_( false )
{

    TEST_INVARIANT;

	pProgressBar_ = _NEW( MachGuiBuildProgressBar(	this,
													Gui::Coord( 2, 2 ),
													width() - 4 ) );

	popupButton( false );
}

/* /////////////////////////////////////////////// destructor /////////////////////////////////////////////////// */


MachProductionIcon::~MachProductionIcon()
{
    TEST_INVARIANT;

    //Clear the cursor prompt string
    pInGameScreen_->clearCursorPromptText();
}

/* ////////////////////////////////////////////////////////////////////////////////////////////////////////////// */

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MachProductionIcon::CLASS_INVARIANT
{
	INVARIANT( this != NULL );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//virtual
void MachProductionIcon::doBeDepressed( const GuiMouseEvent& )
{
	MachGuiSoundManager::instance().playSound( "gui/sounds/igclick.wav" );
    pProgressBar_->depress( true );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//virtual
void MachProductionIcon::doBeReleased( const GuiMouseEvent& )
{
	MachGuiSoundManager::instance().playSound( "gui/sounds/igclick.wav" );
	pProgressBar_->depress( false );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//static
size_t MachProductionIcon::buttonHeight()
{
	// TODO : Remove hard coded values
	return MachGuiBuildProgressBar::height() + 38 /* Bitmap height */ + 4 /* Border */;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//static
size_t MachProductionIcon::buttonWidth()
{
	return 42; // TODO : Remove hard coded value
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MachProductionIcon::updateProgress( float complete )
{
	pProgressBar_->percentageComplete( complete );

	needsPromptUpdate_ = true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//virtual
void MachProductionIcon::doHandleMouseEnterEvent( const GuiMouseEvent& mouseEvent )
{
	MachGuiNewProductionIcon::doHandleMouseEnterEvent( mouseEvent );

	needsPromptUpdate_ = false;

	displayCursorPromptText();
}

void MachProductionIcon::displayCursorPromptText()
{
	GuiString prompt = MachLogActorStringIdRestorer::getActorPromptText(   	machineType_,
																			subType_,
																			weaponCombo_,
																			hwLevel_,
																			IDS_BUILDING_PROMPT,
																			IDS_BUILDING_WITH_WEAPON_PROMPT );

	// Add bmu cost to end of prompt text
	char buffer[20];
//	itoa( MachLogRaces::instance().researchTree().researchItem( machineType_, subType_, hwLevel_, weaponCombo_ ).factoryInstanceCost(), buffer, 10 );
    sprintf(buffer, "%d", MachLogRaces::instance().researchTree().researchItem( machineType_, subType_, hwLevel_, weaponCombo_ ).factoryInstanceCost());
	GuiResourceString bmuCostText( IDS_COST, GuiString( buffer ) );

	prompt += "\n" + bmuCostText.asString();

	// Percentage complete info
//	GuiResourceString percentCompleteText( IDS_BUILDPERCENTAGECOMPLETE, GuiString( itoa( pProgressBar_->percentageComplete(), buffer, 10 ) ) );
	sprintf(buffer, "%d", ( uint ) pProgressBar_->percentageComplete());
	GuiResourceString percentCompleteText( IDS_BUILDPERCENTAGECOMPLETE, GuiString( buffer ) );
	prompt += ", " + percentCompleteText.asString();

	// Max units reached?
	if ( MachLogRaces::instance().maxUnitsExist( MachLogRaces::instance().pcController().race() ) )
	{
		GuiResourceString maxUnitsText( IDS_PRODUCTIONMAXUNITS );
		prompt += ". " + maxUnitsText.asString();
	}

    pInGameScreen_->cursorPromptText( prompt, not needsPromptUpdate_ );

	needsPromptUpdate_ = false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//virtual
void MachProductionIcon::doHandleMouseExitEvent( const GuiMouseEvent& mouseEvent )
{
    //Clear the cursor prompt string
    pInGameScreen_->clearCursorPromptText();

	MachGuiNewProductionIcon::doHandleMouseExitEvent( mouseEvent );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

MachLogProductionUnit& MachProductionIcon::productionUnit() const
{
	return *pProductionUnit_;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ostream& operator <<( ostream& o, const MachProductionIcon& t )
{

    o << "MachProductionIcon " << (void*)&t << " start" << std::endl;
    o << "MachProductionIcon " << (void*)&t << " end" << std::endl;

    return o;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




//////
// ******************************************* Code for MachIncSWLevelIcon *************************************
//////

typedef std::pair< SysPathName, SysPathName > SysPathNames;

/* ////////////////////////////////////////////// constructor /////////////////////////////////////////////////// */

MachIncSWLevelIcon::MachIncSWLevelIcon( GuiDisplayable *pParent,
										const Gui::Coord& rel,
										MachInGameScreen* pInGameScreen,
										MachLog::ObjectType machineType,
									    int subType,
									    int hwLevel )
: 	GuiIcon(	pParent, rel, SysPathNames("gui/misc/sw+.bmp", "gui/misc/sw+.bmp") ),
	pInGameScreen_( pInGameScreen ),
	subType_( subType ),
	hwLevel_( hwLevel ),
	machineType_( machineType )
{
	// Intentionally Empty
}

/* /////////////////////////////////////////////// destructor /////////////////////////////////////////////////// */

MachIncSWLevelIcon::~MachIncSWLevelIcon()
{
	// Intentionally Empty
}

/* ////////////////////////////////////////////////////////////////////////////////////////////////////////////// */

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MachIncSWLevelIcon::CLASS_INVARIANT
{
	INVARIANT( this != NULL );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//virtual
void MachIncSWLevelIcon::doBeDepressed( const GuiMouseEvent& )
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//virtual
void MachIncSWLevelIcon::doBeReleased( const GuiMouseEvent& )
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//virtual
void MachIncSWLevelIcon::doHandleMouseEnterEvent( const GuiMouseEvent& mouseEvent )
{
	GuiIcon::doHandleMouseEnterEvent( mouseEvent );

	GuiResourceString::Id stringId = MachLogActorStringIdRestorer::stringId( machineType_, subType_ );

   	stringId += hwLevel_;

    //Load the resource string
	GuiResourceString machName( stringId );

	GuiResourceString prompt( IDS_INCREASE_SW_PROMPT, machName.asString() );
	//Set the cursor prompt
    pInGameScreen_->cursorPromptText( prompt.asString() );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//virtual
void MachIncSWLevelIcon::doHandleMouseExitEvent( const GuiMouseEvent& mouseEvent )
{
    //Clear the cursor prompt string
    pInGameScreen_->clearCursorPromptText();

	GuiIcon::doHandleMouseExitEvent( mouseEvent );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//static
size_t MachIncSWLevelIcon::buttonHeight()
{
	// TODO : Remove hard coded values
	return 14;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//static
size_t MachIncSWLevelIcon::buttonWidth()
{
	return 42; // TODO : Remove hard coded value
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ostream& operator <<( ostream& o, const MachIncSWLevelIcon& t )
{

    o << "MachIncSWLevelIcon " << (void*)&t << " start" << std::endl;
    o << "MachIncSWLevelIcon " << (void*)&t << " end" << std::endl;

    return o;
}

//virtual
void MachProductionIcon::doHandleContainsMouseEvent( const GuiMouseEvent& mouseEvent )
{
	MachGuiNewProductionIcon::doHandleContainsMouseEvent( mouseEvent );

	if ( needsPromptUpdate_ )
	{
		displayCursorPromptText();
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* End PRODICON.CPP *************************************************/
