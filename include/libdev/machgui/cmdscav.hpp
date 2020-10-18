/*
 * C M D S C A V . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    MachGuiScavengeCommand

    Implement interaction for the Scavenge command
*/

#ifndef _MACHGUI_CMDSCAV_HPP
#define _MACHGUI_CMDSCAV_HPP

#include "base/base.hpp"

#include "ctl/pvector.hpp"

#include "mathex/point3d.hpp"

#include "world4d/observer.hpp"

#include "machgui/command.hpp"

//Forward refs
class MachActor;
class MachLogDebris;

//orthodox canonical (revoked)
class MachGuiScavengeCommand : public MachGuiCommand,
							   public W4dObserver
{
public:
    //ctor.
    MachGuiScavengeCommand( MachInGameScreen* pInGameScreen );

    //dtor
    virtual ~MachGuiScavengeCommand();

    /////////////////////////////////////////////////
    // Inherited from MachGuiCommand

    //Return new instance of this command
    virtual MachGuiCommand* clone() const;

    //The resource file id of the prompt to be displayed when the cursor moves
    //over the command icon
    virtual uint cursorPromptStringId() const;

    //The resource file id of the prompt to be displayed when the command is clicked
    virtual uint commandPromptStringid() const;

    //reference to the up and down bitmap names for this command
    virtual const std::pair<string, string>& iconNames() const;

    //Respond to pick at location on the terrain.
    //ctrl/shift/altPressed indicate which modifier keys were pressed at the time.
    virtual void pickOnTerrain( const MexPoint3d& location, bool ctrlPressed,
                                bool shiftPressed, bool altPressed );

    //respond to pick on pActor in the world view window.
    //ctrl/shift/altPressed indicate which modifier keys were pressed at the time.
    virtual void pickOnActor( MachActor* pActor, bool ctrlPressed,
                              bool shiftPressed, bool altPressed );

    //Respond to cursor at location on the terrain.
    //ctrl/shift/altPressed indicate which modifier keys are pressed.
    //Returns 2d cursor to be displayed.
    virtual MachGui::Cursor2dType cursorOnTerrain( const MexPoint3d& location, bool ctrlPressed,
                                bool shiftPressed, bool altPressed );

    //Respond to cursor on pActor in the world view window.
    //ctrl/shift/altPressed indicate which modifier keys are pressed.
    //Returns 2d cursor to be displayed.
    virtual MachGui::Cursor2dType cursorOnActor( MachActor* pActor, bool ctrlPressed,
                              bool shiftPressed, bool altPressed );

    //Set the type data associated with the command.
    virtual void typeData( MachLog::ObjectType objectType, int subType, uint level );

    //True if this is a kind of command that actor can ever execute.
    virtual bool canActorEverExecute( const MachActor& actor ) const;

    //True if the interaction for the command is complete
    virtual bool isInteractionComplete() const;

	// already have this debris in our vector
	bool debrisIsDuplicate( const MachLogDebris* pCandidateDebris ) const;

	virtual bool beNotified( W4dSubject* pSubject,
	                         W4dSubject::NotificationEvent event, int clientData );

	virtual void domainDeleted( W4dDomain* pDomain );


    /////////////////////////////////////////////////
	// Hot key processing
	virtual bool processButtonEvent( const DevButtonEvent& );

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachGuiScavengeCommand& t );

private:
    // Operation deliberately revoked
    MachGuiScavengeCommand( const MachGuiScavengeCommand& );
    MachGuiScavengeCommand& operator =( const MachGuiScavengeCommand& );
    bool operator ==( const MachGuiScavengeCommand& );

    /////////////////////////////////////////////////
    // Inherited from MachGuiCommand

    //Execute the command for pActor
    virtual bool doApply( MachActor* pActor, string* pReason );

    /////////////////////////////////////////////////

    enum Action { SCAVENGE };

    //Apply the actions
    bool applyScavengeObject( MachActor* pActor, string* pReason );

    typedef ctl_pvector< MachLogDebris > Suppliers;
    //Data members
    Action 		action_; //What to do
    bool 		hadFinalPick_; //True when the final destination pick has been received
	Suppliers 	suppliers_;	//The list of suppliers to send the resource carrier to
};


#endif

/* End CMDATTAC.HPP **************************************************/
