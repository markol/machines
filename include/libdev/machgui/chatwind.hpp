/*
 * C H A T W I N D . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachGuiCharWindow

    A brief description of the class should go in here
*/

#ifndef _MACHGUI_CHATWIND_HPP
#define _MACHGUI_CHATWIND_HPP

#include "base/base.hpp"
#include "gui/displaya.hpp"

class MachGuiStartupScreens;

class MachGuiChatWindow : public GuiDisplayable
// Canonical form revoked
{
public:
    MachGuiChatWindow( MachGuiStartupScreens*, const Gui::Box& );
    ~MachGuiChatWindow( void );

	static void addText( const string& text );

	static void clearAllText();

    void CLASS_INVARIANT;

protected:
	virtual void doDisplay();

	typedef ctl_vector< string > strings;

	static size_t& numLines();
	static strings& linesOfText();
	static size_t& chatWidth();

	static MachGuiChatWindow*& chatWindow();

private:
    friend ostream& operator <<( ostream& o, const MachGuiChatWindow& t );

    MachGuiChatWindow( const MachGuiChatWindow& );
    MachGuiChatWindow& operator =( const MachGuiChatWindow& );

	// Data members...
	MachGuiStartupScreens* pStartupScreens_;
};


#endif

/* End CHATWIND.HPP *************************************************/
