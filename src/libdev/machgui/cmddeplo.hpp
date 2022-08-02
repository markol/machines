/*
 * C M D D E P L O . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    MachGuiDeployCommand

    Implement interaction for the Deploy command
*/

#ifndef _MACHGUI_CMDDEPLO_HPP
#define _MACHGUI_CMDDEPLO_HPP

#include "base/base.hpp"
#include "mathex/point3d.hpp"

#include "machgui/command.hpp"

//Forward refs
class MachActor;

//orthodox canonical (revoked)
class MachGuiDeployCommand : public MachGuiCommand
{
public:
    //ctor.
    MachGuiDeployCommand( MachInGameScreen* pInGameScreen );

    //dtor
    virtual ~MachGuiDeployCommand();

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

    /////////////////////////////////////////////////
	// Hot key processing
	virtual bool processButtonEvent( const DevButtonEvent& );

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachGuiDeployCommand& t );

private:
    // Operation deliberately revoked
    MachGuiDeployCommand( const MachGuiDeployCommand& );
    MachGuiDeployCommand& operator =( const MachGuiDeployCommand& );
    bool operator ==( const MachGuiDeployCommand& );

    /////////////////////////////////////////////////
    // Inherited from MachGuiCommand

    //Execute the command for pActor
    virtual bool doApply( MachActor* pActor, string* pReason );

    /////////////////////////////////////////////////

    //Data members
    MexPoint3d location_; //Location to deploy the contents
    bool hadFinalPick_; //true when the location has been picked
};


#endif

/* End CMDDEPLO.HPP **************************************************/
