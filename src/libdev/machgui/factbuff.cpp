/*
 * F A C T B U F F . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machgui/factbuff.hpp"
#include "machgui/prodicns.hpp"
#include "machgui/prodicon.hpp"
#include "machgui/gui.hpp"

#include "machlog/machlog.hpp"
#include "machlog/machine.hpp"
#include "machlog/factory.hpp"
#include "gui/gui.hpp"

MachGuiFactoryBuffer::MachGuiFactoryBuffer(	GuiDisplayable* pParent,
											const Gui::Boundary& relativeBoundary,
											MachLogFactory* pFactory,
											MachInGameScreen* pInGameScreen	)
:   GuiDisplayable( pParent, relativeBoundary ),
    pFactory_( pFactory ),
    pIcons_( NULL ),
    observingFactory_( false )
{

    //Construct the icon sequence depicting the queue
    Gui::Box iconsArea( 20, 8, MachProductionIcon::buttonWidth() * 3 + 29,
                               MachProductionIcon::buttonHeight() + 12);
    pIcons_ = _NEW( MachProductionIcons( this, iconsArea, pFactory, pInGameScreen ) );

    //Become an observer of the factory
    pFactory_->attach( this );
    observingFactory_ = true;

    //Create and display a build progress indicator if required
    updateProgress();

    TEST_INVARIANT;
}

MachGuiFactoryBuffer::~MachGuiFactoryBuffer()
{
    TEST_INVARIANT;

    //Cease observing the factory
    if( observingFactory_ )
        pFactory_->detach( this );
}

void MachGuiFactoryBuffer::CLASS_INVARIANT
{
	INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachGuiFactoryBuffer& t )
{

    o << "MachGuiFactoryBuffer " << (void*)&t << " start" << std::endl;
    o << "MachGuiFactoryBuffer " << (void*)&t << " end" << std::endl;

    return o;
}

//static
int MachGuiFactoryBuffer::requiredHeight()
{
    // TODO missing actorIconHeight
    //return MachGui::actorIconHeight() + 12;
    return  12;
}

void MachGuiFactoryBuffer::updateQueueIcons()
{
    pIcons_->updateIcons();
}

void MachGuiFactoryBuffer::updateProgress()
{
/*    //If the factory is building, we'll need a progress bar
    MachLog::ObjectType objectType;
    int subType;
    MachLogMachine::Level hwLevel;
    MachLogMachine::Level swLevel;
	MachPhys::WeaponCombo	wc;

    //NB the arguments are for return value, passed by non-const reference.
    if( pFactory_->currentlyBuilding( objectType, subType, hwLevel, swLevel, wc) )
    {
        //Ensure we have a progress bar
        if( pProgressBar_ == NULL )
        {
            Gui::Box area( 22, 2, MachGui::actorIconWidth() + 22, 5 + 2 );
            pProgressBar_ = _NEW( GuiFilledProgressBar( this, area, Gui::BLACK(), Gui::GREEN() ) );
        }

        //Sets its current level
        pProgressBar_->percentageComplete( pFactory_->percentComplete() );

        //Draw it
        pProgressBar_->display();
    }
    else if( pProgressBar_ != NULL )
    {
        //No progress bar required
        _DELETE( pProgressBar_ );
        pProgressBar_ = NULL;
		changed();
    }	 */
}

//virtual
void MachGuiFactoryBuffer::doDisplay()
{}

MachLogFactory& MachGuiFactoryBuffer::factory()
{
    return *pFactory_;
}

//virtual
void MachGuiFactoryBuffer::domainDeleted( W4dDomain* )
{
    //Do nothing
}

//virtual
bool MachGuiFactoryBuffer::beNotified( W4dSubject*, W4dSubject::NotificationEvent event, int clientData )
{
    //Since we only ever observe the factory, it must be the subject.
    switch( event )
    {
        case W4dSubject::DELETED:
        {
            observingFactory_ = false;
            break;
        }

        case W4dSubject::CLIENT_SPECIFIC:
        {
            switch( clientData )
            {
                case MachLog::MACHINE_BUILT:
                {
                    //Update the production queue icon display
                    updateQueueIcons();
                    break;
                }
            }
            break;
        }
    }

    return observingFactory_;
}

/* End FACTBUFF.CPP *************************************************/
