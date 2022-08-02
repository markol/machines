/*
 * C M D F R M S Q . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    MachGuiFormSquadronCommand

    Implement interaction for the FormSquadron command
*/

#ifndef _MACHGUI_CMDFRMSQ_HPP
#define _MACHGUI_CMDFRMSQ_HPP

#include "base/base.hpp"
#include "mathex/point3d.hpp"
#include "machgui/cmddefau.hpp"
#include "machgui/gui.hpp"

//Forward refs
class MachActor;

//orthodox canonical (revoked)
class MachGuiFormSquadronCommand : public MachGuiDefaultCommand
{
public:
    //ctor.
    MachGuiFormSquadronCommand( MachInGameScreen* pInGameScreen );

    //dtor
    virtual ~MachGuiFormSquadronCommand();

    /////////////////////////////////////////////////
    // Inherited from MachGuiCommand

    //Return new instance of this command
    virtual MachGuiCommand* clone() const;

    //The resource file id of the prompt to be displayed when the cursor moves
    //over the command icon
    virtual uint cursorPromptStringId() const;

    //The resource file id of the prompt to be displayed when the command is clicked
    virtual uint commandPromptStringid() const;

    //Called when the command becomes active
    //Default override does nothing
    virtual void start();

    //Called when the command ceases to be active
    //Default override does nothing
    virtual void finish();

    //reference to the up and down bitmap names for this command
    virtual const std::pair<string, string>& iconNames() const;

    //Set the type data associated with the command.
    virtual void typeData( MachLog::ObjectType objectType, int subType, uint level );

    //True if this is a kind of command that actor can ever execute.
    virtual bool canActorEverExecute( const MachActor& actor ) const;

    //True if the interaction for the command is complete
    virtual bool isInteractionComplete() const;

	virtual bool actorsCanExecute() const;

    /////////////////////////////////////////////////
	// Hot key processing
	virtual bool processButtonEvent( const DevButtonEvent& );

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachGuiFormSquadronCommand& t );

private:
    // Operation deliberately revoked
    MachGuiFormSquadronCommand( const MachGuiFormSquadronCommand& );
    MachGuiFormSquadronCommand& operator =( const MachGuiFormSquadronCommand& );
    bool operator ==( const MachGuiFormSquadronCommand& );

    /////////////////////////////////////////////////
    // Inherited from MachGuiCommand

    //Execute the command for pActor
    virtual bool doApply( MachActor* pActor, string* pReason );

    /////////////////////////////////////////////////

    //Data members
    MachGui::ControlPanelContext previousMenuContext_;
};


#endif

/* End CMDFRMSQ.HPP **************************************************/
