/*
 * C M D S T A N D . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    MachGuiStandGroundCommand

    Implement interaction for the Construct command
*/

#ifndef _MACHGUI_CMDSTAND_HPP
#define _MACHGUI_CMDSTAND_HPP

#include "base/base.hpp"

#include "machgui/command.hpp"
#include "machgui/gui.hpp"

//Forward refs
class MachLogAdministrator;

//orthodox canonical (revoked)
class MachGuiStandGroundCommand : public MachGuiCommand
{
public:
    //ctor.
    MachGuiStandGroundCommand( MachInGameScreen* pInGameScreen );

    //dtor
    virtual ~MachGuiStandGroundCommand();

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

    //Called when the command becomes active
    //Default override does nothing
    virtual void start();

    //Called when the command ceases to be active
    //Default override does nothing
    virtual void finish();

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

    friend ostream& operator <<( ostream& o, const MachGuiStandGroundCommand& t );

private:
    // Operation deliberately revoked
    MachGuiStandGroundCommand( const MachGuiStandGroundCommand& );
    MachGuiStandGroundCommand& operator =( const MachGuiStandGroundCommand& );
    bool operator ==( const MachGuiStandGroundCommand& );

    /////////////////////////////////////////////////
    // Inherited from MachGuiCommand

    //Execute the command for pActor
    virtual bool doApply( MachActor* pActor, string* pReason );

    //Execute the command as a sqaudron whose most intelligent administrator
    //is pAdministrator. If successful return true. Otherwise false, with
    //a prompt string indicating reason for failure in pReason.
    virtual bool doAdminApply( MachLogAdministrator* pAdministrator, string* pReason );
    //PRE( canAdminApply() );
};

#endif

/* End CMDSTAND.HPP **************************************************/
