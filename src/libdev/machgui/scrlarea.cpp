/*
 * S C R L A R E A . C P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machgui/scrlarea.hpp"
#include "machgui/ingame.hpp"
#include "machgui/controlp.hpp"
#include "gui/scrolist.hpp"
#include "gui/painter.hpp"
#include "machgui/internal/mgsndman.hpp"

MachGuiScrollButton::MachGuiScrollButton( 	GuiDisplayable *pParent, 
											const Gui::Coord& rel, 
											const SysPathNames& bitmaps,
						 					GuiSimpleScrollableList* pIcons,
						 					MachGuiScrollButton::ScrollDir scrollDir,
						 					MachGuiScrollArea* pScrollArea )
:	GuiIcon( pParent, rel, bitmaps ),	
	pIcons_( pIcons ),
	scrollDir_( scrollDir ),
	pScrollArea_( pScrollArea )
{}

//virtual 
MachGuiScrollButton::~MachGuiScrollButton()
{}

//static 
size_t MachGuiScrollButton::reqWidth()
{
	return 17; // todo
}

//static 
size_t MachGuiScrollButton::reqHeight()
{
	return 50; // todo
}

//virtual 
void MachGuiScrollButton::doBeReleased( const GuiMouseEvent& )
{
	switch ( scrollDir_ )
	{
		case UP:
			if ( pIcons_->canScrollBackward() )
				pIcons_->scrollBackward();
			break;
		case DOWN:
			if ( pIcons_->canScrollFoward() )
				pIcons_->scrollFoward();
			break;
	 	case TOP:
			pIcons_->scrollToBegin();
			break;
	 	case BOTTOM:
			pIcons_->scrollToEnd();
			break;
	}

	pScrollArea_->update();
}

void MachGuiScrollButton::update()
{
	changed();
}

//virtual 
const GuiBitmap& MachGuiScrollButton::getBitmap() const
{
	static GuiBitmap scrollUpBmp( Gui::bitmap( SysPathName( "gui/corral/scrlup.bmp" ) ) );
	static GuiBitmap scrollUpHighlightBmp( Gui::bitmap( SysPathName( "gui/corral/scrlupH.bmp" ) ) );
	static GuiBitmap scrollDownBmp( Gui::bitmap( SysPathName(  "gui/corral/scrldn.bmp" ) ) );
	static GuiBitmap scrollDownHighlightBmp( Gui::bitmap( SysPathName( "gui/corral/scrldnH.bmp" ) ) );
	static GuiBitmap scrollTopBmp( Gui::bitmap( SysPathName( "gui/corral/scrlup2.bmp" ) ) );
	static GuiBitmap scrollTopHighlightBmp( Gui::bitmap( SysPathName( "gui/corral/scrlup2H.bmp" ) ) );
	static GuiBitmap scrollBottomBmp( Gui::bitmap( SysPathName(  "gui/corral/scrldn2.bmp" ) ) );
	static GuiBitmap scrollBottomHighlightBmp( Gui::bitmap( SysPathName( "gui/corral/scrldn2H.bmp" ) ) );

	GuiBitmap* pRetBitmap = NULL;

	switch ( scrollDir_ )
	{
		case UP:
			pRetBitmap = &(  pIcons_->canScrollBackward() ? scrollUpHighlightBmp : scrollUpBmp ); 
			break;
		case DOWN:
			pRetBitmap = &(  pIcons_->canScrollFoward() ? scrollDownHighlightBmp : scrollDownBmp ); 
			break;
		case TOP:
			pRetBitmap = &(  pIcons_->canScrollBackward() ? scrollTopHighlightBmp : scrollTopBmp ); 
			break;
		case BOTTOM:
			pRetBitmap = &(  pIcons_->canScrollFoward() ? scrollBottomHighlightBmp : scrollBottomBmp ); 
			break;
	}

	POST( pRetBitmap );

	return *pRetBitmap;
}

//virtual 
void MachGuiScrollButton::doBeDepressed( const GuiMouseEvent& )
{
	MachGuiSoundManager::instance().playSound( "gui/sounds/igclick.wav" );
}

MachGuiScrollBar::MachGuiScrollBar( MachGuiScrollArea *pParent,
									const Gui::Coord& rel,
									GuiSimpleScrollableList* pIcons,
									MachInGameScreen* pInGameScreen,
									ScrollType scrollType )
: 	GuiDisplayable( pParent, Gui::Box( rel.x(), rel.y(), rel.x() + reqWidth(), rel.y() + reqHeight() ) ),
	pIcons_( pIcons ),
	pInGameScreen_( pInGameScreen )
{
	SysPathNames upBmp( SysPathName( "gui/corral/scrlup.bmp" ), SysPathName( "gui/corral/scrlup.bmp" ) );
	SysPathNames downBmp( SysPathName( "gui/corral/scrldn.bmp" ), SysPathName( "gui/corral/scrldn.bmp" ) );
	
	pUpButton_	= _NEW( MachGuiScrollButton(this, 
											Gui::Coord( 0, 0 ), 
											upBmp, 
											pIcons, 
											scrollType == UPDOWN ? MachGuiScrollButton::UP : MachGuiScrollButton::TOP, 
											pParent ) );
	pDownButton_= _NEW( MachGuiScrollButton(this, 
											Gui::Coord( 0, MachGuiScrollButton::reqHeight() ), 
											downBmp, 
											pIcons, 
											scrollType == UPDOWN ? MachGuiScrollButton::DOWN : MachGuiScrollButton::BOTTOM, 
											pParent ) );
}

MachGuiScrollBar::~MachGuiScrollBar()
{}

//static 
size_t MachGuiScrollBar::reqWidth()
{
	return MachGuiScrollButton::reqWidth();
}

//static 
size_t MachGuiScrollBar::reqHeight()
{
	return MachGuiScrollButton::reqHeight() * 2;
}

void MachGuiScrollBar::update()
{
	pUpButton_->update();
	pDownButton_->update();
}

//virtual 
void MachGuiScrollBar::doDisplay()
{}

/////////////////////////////////////////////////////////

MachGuiScrollArea::MachGuiScrollArea( GuiDisplayable* pParent, const Gui::Box& area, MachInGameScreen* pInGameScreen )
:	GuiDisplayable( pParent, area ),
	pInGameScreen_( pInGameScreen )
{
	changed(); // Force this to be drawn

    TEST_INVARIANT;
}

MachGuiScrollArea::~MachGuiScrollArea()
{
    TEST_INVARIANT;

}

void MachGuiScrollArea::initialise()
{					
	DEBUG_STREAM( DIAG_NEIL, "MachGuiScrollArea::initialise()" << std::endl );

	SysPathNames upBmp( SysPathName( "gui/corral/scrlup.bmp" ), SysPathName( "gui/corral/scrlup.bmp" ) );
	SysPathNames downBmp( SysPathName( "gui/corral/scrldn.bmp" ), SysPathName( "gui/corral/scrldn.bmp" ) );
	
	pList_ = createList( this, Gui::Coord( MachGuiScrollBar::reqWidth(), 1 ), pInGameScreen_ );
	pLHSScrollBar_ = _NEW( MachGuiScrollBar( this, Gui::Coord(0,0), pList_, pInGameScreen_, MachGuiScrollBar::TOPBOTTOM ) );
	pRHSScrollBar_ = _NEW( MachGuiScrollBar( this, Gui::Coord( MachGuiScrollBar::reqWidth() + pList_->width(), 0 ), pList_, pInGameScreen_, MachGuiScrollBar::UPDOWN ) );
}

void MachGuiScrollArea::update()
{
	pLHSScrollBar_->update();
	pRHSScrollBar_->update();
}

//virtual 
void MachGuiScrollArea::doDisplay() 
{
	pInGameScreen_->controlPanel().redrawAreaImmediate( *this );

	Gui::Box outline = absoluteBoundary();

	outline.xMin( outline.minCorner().x() + MachGuiScrollButton::reqWidth() - 1 );
	outline.xMax( outline.maxCorner().x() - MachGuiScrollButton::reqWidth() + 1 );

	GuiPainter::instance().hollowRectangle( outline, MachGui::LIGHTPURPLE(), 1 );
}

void MachGuiScrollArea::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachGuiScrollArea& t )
{

    o << "MachGuiScrollArea " << (void*)&t << " start" << std::endl;
    o << "MachGuiScrollArea " << (void*)&t << " end" << std::endl;

    return o;
}

/* End SCRLAREA.CPP *************************************************/
