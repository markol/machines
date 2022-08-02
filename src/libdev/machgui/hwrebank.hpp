/*
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    MachHWResearchBank

    Contains the displayables in the Hardware Research Menu representing the lab's
    current research queue state.
*/

#ifndef _MACHGUI_HWREBANK_HPP
#define _MACHGUI_HWREBANK_HPP

#include "base/base.hpp"
#include "gui/displaya.hpp"
#include "world4d/observer.hpp"


//Forward refs
class MachHWResearchBankIcons;
class MachLogHardwareLab;
class MachInGameScreen;
class MachGuiResBufferScrollButton;
class DevKeyToCommandTranslator;

//orthodox canonical (revoked)
class MachHWResearchBank :
    public GuiDisplayable,
    public W4dObserver
{
public:
    //ctor. Owned by pParent, with area relativeBoundary.
    //The lab being edited is pHardwareLab.
    //The gui root is pInGameScreen.
    MachHWResearchBank( GuiDisplayable * pParent, const Gui::Boundary& relativeBoundary,
                        MachLogHardwareLab* pHardwareLab, MachInGameScreen* pInGameScreen );

    //dtor
    ~MachHWResearchBank();

    //Update the queue icons
    void updateQueueIcons();

    //Update progress bars
    void updateProgress();

	void updateScrollBars();

	// inherited from GuiDisplayable...
	virtual void doDisplay();

    //The hardware lab whose research queue is represented
    MachLogHardwareLab& hardwareLab();
	
	// Called every time there is a buttonEvent. Returns true if the buttonEvent was
	// processed (e.g. buttonEvent caused squad to be selected).
	bool processButtonEvent( const DevButtonEvent& buttonEvent );	
	
	//Remove from the bank any and all selected icons
	//This also removes the associated production units from this prodbank's associated factory's production line.
	void cancelSelectedQueueIcons();

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
	static size_t reqWidth();
	static size_t reqHeight();

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachHWResearchBank& t );

private:
    // Operation deliberately revoked
    MachHWResearchBank( const MachHWResearchBank& );
    MachHWResearchBank& operator =( const MachHWResearchBank& );
    bool operator ==( const MachHWResearchBank& );

	enum Command { 	CANCEL_SELECTED_RESEARCH, CANCEL_ALL_RESEARCH };
	
    //Data members
    MachLogHardwareLab* pHardwareLab_; //The lab being edited
    MachHWResearchBankIcons* pIcons_; //The icon sequence depicting the queue machine icons
    bool observingLab_;//True while the observer relation on the lab exists
	MachGuiResBufferScrollButton* pScrollLeft_;
	MachGuiResBufferScrollButton* pScrollRight_;
	DevKeyToCommandTranslator* pKeyTranslator_;
};


#endif

/* End  *************************************************/
