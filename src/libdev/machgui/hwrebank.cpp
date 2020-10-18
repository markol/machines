/*
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "stdlib/string.hpp"
#include "machgui/hwrebank.hpp"
#include "machgui/hwrbicns.hpp"
#include "machgui/gui.hpp"
#include "machgui/ingame.hpp"
#include "machgui/controlp.hpp"
#include "machlog/machlog.hpp"
#include "machlog/hwlab.hpp"
#include "machlog/resitem.hpp"
#include "gui/gui.hpp"
#include "gui/progress.hpp"
#include "gui/icon.hpp"
#include "device/butevent.hpp"
#include "device/keytrans.hpp"
#include "machgui/internal/mgsndman.hpp"

class MachGuiResBufferScrollButton : public GuiIcon
// cannonical from revoked
{
public:
	enum ScrollDir { LEFT, RIGHT };

	MachGuiResBufferScrollButton(	GuiDisplayable *pParent,
								const Gui::Coord& rel,
								const SysPathName& bitmaps,
								MachHWResearchBankIcons*,
						 		ScrollDir,
						 		MachInGameScreen*,
						 		MachHWResearchBank* );
	virtual ~MachGuiResBufferScrollButton();

	static size_t reqWidth()
	{
		return 17; // TODO: remove hard coding
	}

	void update();

protected:
	virtual void doBeDepressed( const GuiMouseEvent& )
	{
		MachGuiSoundManager::instance().playSound( "gui/sounds/igclick.wav" );
	}

	virtual void doBeReleased( const GuiMouseEvent& );

	virtual const GuiBitmap& getBitmap() const;

private:
	// Operations revoked
	MachGuiResBufferScrollButton( const MachGuiResBufferScrollButton& );
	MachGuiResBufferScrollButton& operator =( const MachGuiResBufferScrollButton& );
	bool operator ==( const MachGuiResBufferScrollButton& ) const;

	// Data members...
	MachHWResearchBankIcons* pHWResearchIcons_;
	MachInGameScreen* pInGameScreen_;
	ScrollDir scrollDir_;
	MachHWResearchBank* pHWResearchBank_;
};

/* ////////////////////////////////////////////// constructor /////////////////////////////////////////////////// */

MachGuiResBufferScrollButton::MachGuiResBufferScrollButton( 	GuiDisplayable *pParent, 
																const Gui::Coord& rel, 
																const SysPathName& bitmap,
								 								MachHWResearchBankIcons* pHWResearchIcons,
								 								ScrollDir scrollDir,
								 								MachInGameScreen* pInGameScreen,
								 								MachHWResearchBank* pHWResearchBank )
:	GuiIcon( pParent, rel, bitmap ),	
	pHWResearchIcons_( pHWResearchIcons ),
	scrollDir_( scrollDir ),
	pInGameScreen_( pInGameScreen ),
	pHWResearchBank_( pHWResearchBank )
{
// deliberately left blank
}

/* /////////////////////////////////////////////// destructor /////////////////////////////////////////////////// */

//virtual 
MachGuiResBufferScrollButton::~MachGuiResBufferScrollButton()
{
// deliberately left blank
}

/* ////////////////////////////////////////////////////////////////////////////////////////////////////////////// */

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//virtual 
void MachGuiResBufferScrollButton::doBeReleased( const GuiMouseEvent& )
{
	switch ( scrollDir_ )
	{
		case LEFT:
			if ( pHWResearchIcons_->canScrollBackward() )
				pHWResearchIcons_->scrollBackward();
			break;
		case RIGHT:
			if ( pHWResearchIcons_->canScrollFoward() )
				pHWResearchIcons_->scrollFoward();
			break;
	}

	pHWResearchBank_->updateScrollBars();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MachGuiResBufferScrollButton::update()
{
	bool canScroll = pHWResearchIcons_->canScrollFoward() or pHWResearchIcons_->canScrollBackward(); 

	if ( canScroll !=isVisible() )
	{
		isVisible( canScroll );
		
		// Clean up control panel if this is being removed.
		if ( not canScroll )
		{
			pInGameScreen_->controlPanel().redrawArea( *this );
		}
	}

	changed();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//virtual 
const GuiBitmap& MachGuiResBufferScrollButton::getBitmap() const
{
	static GuiBitmap scrollLeftBmp( Gui::bitmap( SysPathName( "gui/misc/scrolll.bmp" ) ) );
	static GuiBitmap scrollLeftHighlightBmp( Gui::bitmap( SysPathName( "gui/misc/scrolllh.bmp" ) ) );
	static GuiBitmap scrollRightBmp( Gui::bitmap( SysPathName( "gui/misc/scrollr.bmp" ) ) );
	static GuiBitmap scrollRightHighlightBmp( Gui::bitmap( SysPathName( "gui/misc/scrollrh.bmp" ) ) );

  	if ( scrollDir_ == LEFT )
	{
		if ( pHWResearchIcons_->canScrollBackward() )
		{
			return scrollLeftHighlightBmp;
		}
		else
		{
			return scrollLeftBmp; 
		}
	}	
	else
	{
		if ( pHWResearchIcons_->canScrollFoward() )
		{
			return scrollRightHighlightBmp;
		}
		else
		{
			return scrollRightBmp;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




//////
// ******************************************* Code for MachHWResearchBank *************************************
//////


/* ////////////////////////////////////////////// constructor /////////////////////////////////////////////////// */

MachHWResearchBank::MachHWResearchBank
(
    GuiDisplayable * pParent, const Gui::Boundary& relativeBoundary,
    MachLogHardwareLab* pHardwareLab, MachInGameScreen* pInGameScreen
)
:   GuiDisplayable( pParent, relativeBoundary, GuiDisplayable::LAYER3 ),
    pHardwareLab_( pHardwareLab ),
    pIcons_( NULL ),
    observingLab_( false )
{

    //Construct the icon sequence depicting the queue
    Gui::Box iconsArea( MachGuiResBufferScrollButton::reqWidth(), 0, 
    					MachHWResearchBankIcons::reqWidth() + MachGuiResBufferScrollButton::reqWidth(),
	                    MachHWResearchBankIcons::reqHeight() );

    pIcons_ = _NEW( MachHWResearchBankIcons( this, iconsArea, pHardwareLab_, pInGameScreen ) );

    //Become an observer of the lab
    pHardwareLab_->attach( this );
    observingLab_ = true;

    //Create and display a build progress indicator if required
    updateProgress();

	pScrollLeft_ = _NEW( MachGuiResBufferScrollButton( this, Gui::Coord(0,0), SysPathName( "gui/misc/scrolll.bmp" ), pIcons_, MachGuiResBufferScrollButton::LEFT, pInGameScreen, this ) );
	pScrollRight_ = _NEW( MachGuiResBufferScrollButton( this, Gui::Coord(MachGuiResBufferScrollButton::reqWidth() + MachHWResearchBankIcons::reqWidth(),0), SysPathName( "gui/misc/scrollr.bmp" ), pIcons_, MachGuiResBufferScrollButton::RIGHT, pInGameScreen, this ) );

	pKeyTranslator_ = _NEW( DevKeyToCommandTranslator() );
	pKeyTranslator_->addTranslation( DevKeyToCommand( DevKey::BACK_SPACE, CANCEL_SELECTED_RESEARCH, DevKeyToCommand::RELEASED, DevKeyToCommand::RELEASED, DevKeyToCommand::RELEASED ) );
   	pKeyTranslator_->initEventQueue();
	
	updateQueueIcons();

    TEST_INVARIANT;
}

/* /////////////////////////////////////////////// destructor /////////////////////////////////////////////////// */

MachHWResearchBank::~MachHWResearchBank()
{
    TEST_INVARIANT;

    //Cease observing the factory
    if( observingLab_ )
        pHardwareLab_->detach( this );

}

/* ////////////////////////////////////////////////////////////////////////////////////////////////////////////// */

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MachHWResearchBank::CLASS_INVARIANT
{
	INVARIANT( this != NULL );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MachHWResearchBank::updateQueueIcons()
{
    pIcons_->updateIcons();
	updateScrollBars();
	updateProgress();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MachHWResearchBank::updateScrollBars()
{
	pScrollLeft_->update();
	pScrollRight_->update();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MachHWResearchBank::updateProgress()
{
    //If the hw lab is researching, we'll need a progress bar
    MachLogResearchItem* pCurrentItem;
    if( pHardwareLab_->currentlyResearching( &pCurrentItem ) )
    {
        //Sets its current level
        double percentComplete = 100.0 * 
            (double( pCurrentItem->amountResearched( pHardwareLab_->race() ) ) /
             double( pCurrentItem->researchCost() ));

       	pIcons_->updateProgress( percentComplete ); 
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MachHWResearchBank::cancelSelectedQueueIcons()
{
	pIcons_->cancelSelectedIcons();
	pIcons_->updateIcons();
	updateScrollBars();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//virtual
void MachHWResearchBank::doDisplay()
{
	pScrollLeft_->update();
	pScrollRight_->update();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

MachLogHardwareLab& MachHWResearchBank::hardwareLab()
{
    PRE( pHardwareLab_ != NULL );

    return *pHardwareLab_;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//virtual
void MachHWResearchBank::domainDeleted( W4dDomain* )
{
    //Do nothing
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool MachHWResearchBank::processButtonEvent( const DevButtonEvent& buttonEvent )
{
	// do not process key release events
	
	if ( buttonEvent.action() == DevButtonEvent::PRESS )
		return false;


	typedef DevKeyToCommand::CommandId CommandId;
	
	CommandId commandId;
	bool processed = pKeyTranslator_->translate( buttonEvent, &commandId );
			
	if ( processed )
	{
		switch ( commandId )
		{
			case CANCEL_SELECTED_RESEARCH:
				cancelSelectedQueueIcons();
				break;
			default:
				ASSERT ( true, "Received unexpected command ID in MachHWResearchBank::processButtonEvent" );
				break;
		}
	}

	return processed;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//virtual
bool MachHWResearchBank::beNotified
(
    W4dSubject*, W4dSubject::NotificationEvent event, int clientData
)
{
    //Since we only ever observe the factory, it must be the subject.
    switch( event )
    {
        case W4dSubject::DELETED:
        {
            observingLab_ = false;
            break;
        }

        case W4dSubject::CLIENT_SPECIFIC:
        {
            switch( clientData )
            {
                case MachLog::HW_RESEARCH_COMPLETE:
                {
                    //Update the research queue icon display
                    updateQueueIcons();
                    break;
                }
            }
            break;
        }
    }

    return observingLab_;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//static 
size_t MachHWResearchBank::reqWidth()
{
	return ( MachGuiResBufferScrollButton::reqWidth() + MachHWResearchBankIcons::reqWidth() + MachGuiResBufferScrollButton::reqWidth() );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//static 
size_t MachHWResearchBank::reqHeight()
{
	return ( MachHWResearchBankIcons::reqHeight() );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ostream& operator <<( ostream& o, const MachHWResearchBank& t )
{

    o << "MachHWResearchBank " << (void*)&t << " start" << std::endl;
    o << "MachHWResearchBank " << (void*)&t << " end" << std::endl;

    return o;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* End *************************************************/
