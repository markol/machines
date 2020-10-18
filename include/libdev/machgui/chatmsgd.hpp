/*
 * C H A T M S G D . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachGuiInGameChatMessagesDisplay

    A brief description of the class should go in here
*/

#ifndef _MACHGUI_CHATMSGD_HPP
#define _MACHGUI_CHATMSGD_HPP

#include "base/base.hpp"
#include "gui/displaya.hpp"

struct MachGuiInGameChatMessagesDisplayImpl;

class MachGuiInGameChatMessagesDisplay : public GuiDisplayable
// Canonical form revoked
{
public:
    MachGuiInGameChatMessagesDisplay( GuiDisplayable*, const Gui::Box& );
    MachGuiInGameChatMessagesDisplay( GuiDisplayable*, const Gui::Box&, GuiDisplayable* );
    ~MachGuiInGameChatMessagesDisplay( void );

    void CLASS_INVARIANT;

	virtual void doDisplay();

	void forceUpdate();

protected:
	//virtual void doHandleMouseClickEvent( const GuiMouseEvent& rel );
	//virtual void doHandleMouseEnterEvent( const GuiMouseEvent& rel );
	//virtual void doHandleMouseExitEvent( const GuiMouseEvent& rel );
	//virtual void doHandleContainsMouseEvent( const GuiMouseEvent& rel );

	// Pass mouse position in as relative to this GuiDisplayable. Updates
	// mouse position to be relative to pPassEventsTo_.
	//bool passEventTo( GuiMouseEvent* );

	virtual bool processesMouseEvents() const;

private:
    friend ostream& operator <<( ostream& o, const MachGuiInGameChatMessagesDisplay& t );

    MachGuiInGameChatMessagesDisplay( const MachGuiInGameChatMessagesDisplay& );
    MachGuiInGameChatMessagesDisplay& operator =( const MachGuiInGameChatMessagesDisplay& );

	MachGuiInGameChatMessagesDisplayImpl* pImpl_;
};


#endif

/* End CHATMSGD.HPP *************************************************/
