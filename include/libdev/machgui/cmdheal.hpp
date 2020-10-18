/*
 * C M D A T T A C . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    MachGuiHealCommand

    Implement interaction for the Heal command
*/

#ifndef _MACHGUI_CMDHEAL_HPP
#define _MACHGUI_CMDHEAL_HPP

#include "base/base.hpp"
#include "ctl/vector.hpp"
#include "mathex/point3d.hpp"

#include "machgui/command.hpp"

//Forward refs
class MachActor;

//orthodox canonical (revoked)
class MachGuiHealCommand : public MachGuiCommand
{
public:
    //ctor.
    MachGuiHealCommand( MachInGameScreen* pInGameScreen );

    //dtor
    virtual ~MachGuiHealCommand();

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

    //True iff there is an administer version of the command for squadrons
    //with an administrator. Default implementation returns false.
    virtual bool canAdminApply() const;

    //True if the interaction for the command is complete
    virtual bool isInteractionComplete() const;

    /////////////////////////////////////////////////
	// Hot key processing
	virtual bool processButtonEvent( const DevButtonEvent& );

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachGuiHealCommand& t );

private:
    // Operation deliberately revoked
    MachGuiHealCommand( const MachGuiHealCommand& );
    MachGuiHealCommand& operator =( const MachGuiHealCommand& );
    bool operator ==( const MachGuiHealCommand& );

    /////////////////////////////////////////////////
    // Inherited from MachGuiCommand

    //Execute the command for pActor
    virtual bool doApply( MachActor* pActor, string* pReason );

    //Execute the command as a sqaudron whose most intelligent administrator
    //is pAdministrator. If successful return true. Otherwise false, with
    //a prompt string indicating reason for failure in pReason.
    virtual bool doAdminApply( MachLogAdministrator* pAdministrator, string* pReason );
    //PRE( canAdminApply() );
    /////////////////////////////////////////////////

	// true if any member of a group is capable of healing the target machine
	bool atLeastOneCanHeal( const MachLogMachine* pTargetMachine ) const;

    enum Action { MOVE_TO_LOCATION, HEAL_OBJECT };

    //Apply the actions
    bool applyMove( MachActor* pActor, string* pReason );
    bool applyHealObject( MachActor* pActor, string* pReason );

    //Data members
    Action action_; //What to do
    MachActor* pDirectObject_; //The target of the Heal
    MexPoint3d location_; //The location to move to
    bool hadFinalPick_; //True when the final destination pick has been received
};


#endif

/* End CMDATTAC.HPP **************************************************/
