/*
 * C M D D E S T R . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    MachGuiSelfDestructCommand

    Implement interaction for the SelfDestruct command
*/

#ifndef _MACHGUI_CMDDESTR_HPP
#define _MACHGUI_CMDDESTR_HPP

#include "base/base.hpp"
#include "ctl/pvector.hpp"
#include "mathex/point3d.hpp"
#include "machgui/cmddefau.hpp"
#include "machgui/commbank.hpp"

//Forward refs
class MachActor;

class MachGuiSelfDestructCommandIcon : public MachCommandIcon
{
public:
	MachGuiSelfDestructCommandIcon(   GuiDisplayable *pParent,
									  const Gui::Coord& rel,
		                              const MachGuiCommand& command,
									  MachInGameScreen * pInGameScreen );
	~MachGuiSelfDestructCommandIcon();

  	virtual const GuiBitmap& getBitmap() const;

	typedef ctl_pvector<MachGuiSelfDestructCommandIcon> MachGuiSelfDestructCommandIcons;

	static MachGuiSelfDestructCommandIcons& icons();
	static void refreshAllIcons();
};

//orthodox canonical (revoked)
class MachGuiSelfDestructCommand : public MachGuiDefaultCommand
{
public:
    //ctor.
    MachGuiSelfDestructCommand( MachInGameScreen* pInGameScreen );

    //dtor
    virtual ~MachGuiSelfDestructCommand();

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

    //Set the type data associated with the command.
    virtual void typeData( MachLog::ObjectType objectType, int subType, uint level );

    //True if this is a kind of command that actor can ever execute.
    virtual bool canActorEverExecute( const MachActor& actor ) const;

    //True if the interaction for the command is complete
    virtual bool isInteractionComplete() const;

	// Hot key processing
	virtual bool processButtonEvent( const DevButtonEvent& );

    /////////////////////////////////////////////////

	enum ButtonState { BTN_HIDDEN, BTN_SHOWN, BTN_ACTIVE };
	static ButtonState buttonState();
	static bool hadFinalPick();

	static void resetButtonState();

	typedef ctl_pvector< MachActor > Actors;

	static void update( const Actors& actors );

	static void stopPressed();

	virtual bool actorsCanExecute() const;

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachGuiSelfDestructCommand& t );

private:
    // Operation deliberately revoked
    MachGuiSelfDestructCommand( const MachGuiSelfDestructCommand& );
    MachGuiSelfDestructCommand& operator =( const MachGuiSelfDestructCommand& );
    bool operator ==( const MachGuiSelfDestructCommand& );

	friend class MachGuiSelfDestructCommandIcon;

	static ButtonState& privateButtonState();
	static bool& privateHadFinalPick();
	static int& privateRotateFrame();
	static double& privateRotateFrameTime();

    /////////////////////////////////////////////////
    // Inherited from MachGuiCommand

    //Execute the command for pActor
    virtual bool doApply( MachActor* pActor, string* pReason );
};


#endif

/* End CMDATTAC.HPP **************************************************/
