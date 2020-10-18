/*
 * C M D D E F A U . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    MachGuiDefaultCommand

    Implement interaction for the Default command.
    This includes intelligent cursor, actor selection etc.
*/

#ifndef _MACHGUI_CMDDEFAU_HPP
#define _MACHGUI_CMDDEFAU_HPP

#include "base/base.hpp"

#include "machgui/command.hpp"

//Forward refs
class MachGuiIntelligentCursorOnActor;

//orthodox canonical (revoked)
class MachGuiDefaultCommand : public MachGuiCommand
{
public:
    //ctor.
    MachGuiDefaultCommand( MachInGameScreen* pInGameScreen );

    //dtor
    virtual ~MachGuiDefaultCommand();

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

    //True if this command should be visible based on selected actor set.
    //Ignores actor race in determining result.
    virtual bool actorsCanExecute() const;

    //True if this is a kind of command that actor can ever execute.
    virtual bool canActorEverExecute( const MachActor& actor ) const;

    //True if the interaction for the command is complete
    virtual bool isInteractionComplete() const;

    /////////////////////////////////////////////////

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachGuiDefaultCommand& t );

private:
    // Operation deliberately revoked
    MachGuiDefaultCommand( const MachGuiDefaultCommand& );
    MachGuiDefaultCommand& operator =( const MachGuiDefaultCommand& );
    bool operator ==( const MachGuiDefaultCommand& );

    /////////////////////////////////////////////////
    // Inherited from MachGuiCommand

    //Execute the command for pActor
    virtual bool doApply( MachActor* pActor, string* pReason );

    /////////////////////////////////////////////////

    //Data members
    //Performs intelligent cursor mapping when cursor is on an actor
    MachGuiIntelligentCursorOnActor* pIntelligentCursorOnActor_;
};


#endif

/* End CMDDEFAU.HPP **************************************************/
