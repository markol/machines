/*
 * F A C T B U F F . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachGuiFactoryBuffer

    A brief description of the class should go in here
*/

#ifndef _MACHGUI_FACTBUFF_HPP
#define _MACHGUI_FACTBUFF_HPP

#include "base/base.hpp"
#include "world4d/observer.hpp"
#include "gui/displaya.hpp"
 
//Forward refs
class MachProductionIcons;
class MachLogFactory;
class MachInGameScreen;

//orthodox canonical (revoked)
class MachGuiFactoryBuffer :
    public GuiDisplayable,
    public W4dObserver
{
public:
    //ctor. Owned by pParent, with area relativeBoundary.
    //The factory being edited is pFactory.
    //The gui root is pInGameScreen.
    MachGuiFactoryBuffer( GuiDisplayable * pParent, const Gui::Boundary& relativeBoundary,
                          MachLogFactory* pFactory, MachInGameScreen* pInGameScreen );

    //dtor
    ~MachGuiFactoryBuffer();

    //The required height for the bank
    static int requiredHeight();

    //Update the queue icons
    void updateQueueIcons();

    //Update progress bars
    void updateProgress();

	// inherited from GuiDisplayable...
	virtual void doDisplay();

    //The bank's factory
    MachLogFactory& factory();

    ////////////////////////////////////////////////////////////////////////////////
    // Inherited from W4dObserver

	//true iff this observer is to exist in this subject's list of observers
	//following this call. This will typically be implemented using double dispatch.
	//The clientData is of interest only if event == CLIENT_SPECIFIC.Interpretation
	//is client defined.
	virtual bool beNotified( W4dSubject* pSubject,
	                         W4dSubject::NotificationEvent event, int clientData );

    //Informs observer that an observed domain is being deleted.
    //This observer need not call the W4dDomain::detach() method - this
    //will be done automatically.
    virtual void domainDeleted( W4dDomain* pDomain );

    ////////////////////////////////////////////////////////////////////////////////

    void CLASS_INVARIANT;

private:
    // Operation deliberately revoked
    MachGuiFactoryBuffer( const MachGuiFactoryBuffer& );
    MachGuiFactoryBuffer& operator =( const MachGuiFactoryBuffer& );
    bool operator ==( const MachGuiFactoryBuffer& );

    friend ostream& operator <<( ostream& o, const MachGuiFactoryBuffer& t );

    //Data members
    MachLogFactory* pFactory_; //The factory being edited
    MachProductionIcons* pIcons_; //The icon sequence depicting the queue machine icons
    bool observingFactory_;//True while the observer relation on the factory exists
};

#endif

/* End FACTBUFF.HPP *************************************************/
