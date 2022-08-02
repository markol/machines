/*
 * C M D N U K A T . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachGuiNukeAttackCommand

    Implement interaction for the Attack command
*/

#ifndef _MACHGUI_CMDNUKAT_HPP
#define _MACHGUI_CMDNUKAT_HPP

#include "base/base.hpp"
#include "ctl/vector.hpp"
#include "mathex/point3d.hpp"

#include "machgui/command.hpp"

//Forward refs
class MachActor;

//orthodox canonical (revoked)
class MachGuiNukeAttackCommand : public MachGuiCommand
{
public:
    //ctor.
    MachGuiNukeAttackCommand( MachInGameScreen* pInGameScreen );

    //dtor
    virtual ~MachGuiNukeAttackCommand();

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
    //with an administrator. Default implementation returns false. We use default implementation.
    // virtual bool canAdminApply() const;

    //True if the interaction for the command is complete
    virtual bool isInteractionComplete() const;

    /////////////////////////////////////////////////
	// Hot key processing
	virtual bool processButtonEvent( const DevButtonEvent& );

	static void update( const Actors& actors );
	static int& highestPercentageRecharged();

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachGuiNukeAttackCommand& t );

private:
    // Operation deliberately revoked
    MachGuiNukeAttackCommand( const MachGuiNukeAttackCommand& );
    MachGuiNukeAttackCommand& operator =( const MachGuiNukeAttackCommand& );
    bool operator ==( const MachGuiNukeAttackCommand& );

    /////////////////////////////////////////////////
    // Inherited from MachGuiCommand

    //Execute the command for pActor
    virtual bool doApply( MachActor* pActor, string* pReason );

    /////////////////////////////////////////////////

    enum Action { ATTACK_LOCATION, ATTACK_OBJECT, DO_NOTHING };

    //Apply the actions
    bool applyAttackLocation( MachActor* pActor, string* pReason );
    bool applyAttackObject( MachActor* pActor, string* pReason );

	void giveNotEnoughBMUsVoicemail() const;

	bool atLeastOneActorHasChargedNuke() const;

    //Data members
    Action action_; //What to do
    MachActor* pDirectObject_; //The target of the attack
    MexPoint3d location_; //The location to move to
    bool hadFinalPick_; //True when the final destination pick has been received
};


#endif

/* End CMDNUKAT.HPP **************************************************/
