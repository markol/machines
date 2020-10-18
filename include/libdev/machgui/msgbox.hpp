/*
 * M S G B O X . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachGuiMessageBox

    A brief description of the class should go in here
*/

#ifndef _MACHGUI_MSGBOX_HPP
#define _MACHGUI_MSGBOX_HPP

#include "base/base.hpp"
#include "gui/displaya.hpp"
#include "gui/restring.hpp"
#include "machgui/animatio.hpp"

class MachGuiStartupScreens;
class GuiImage;

class MachGuiMessageBoxResponder
// Canonical form
{
public:
	MachGuiMessageBoxResponder();
	virtual ~MachGuiMessageBoxResponder();

	virtual bool okPressed();
	virtual bool cancelPressed();
};

class MachGuiMessageBox	: public GuiDisplayable
// Canonical form revoked
{
public:
	enum MBType { MBOK, MBOKCANCEL, MBYESNO };

    MachGuiMessageBox( MachGuiStartupScreens*, uint stringResId, MBType );
	MachGuiMessageBox( MachGuiStartupScreens*, uint stringResId, MBType, const GuiStrings& );
    ~MachGuiMessageBox( void );

	
    void CLASS_INVARIANT;

	void update();
	const GuiBitmap& image( void ) const;

protected:
	virtual void doDisplay();

private:
    friend ostream& operator <<( ostream& o, const MachGuiMessageBox& t );

    MachGuiMessageBox( const MachGuiMessageBox& );
    MachGuiMessageBox& operator =( const MachGuiMessageBox& );

	void displayButtons( MachGuiStartupScreens* );
	static SysPathName animationFile( MBType );

	// Data members...
	MachGuiAnimations animations_;
	MBType mbType_;
	MachGuiStartupScreens* pStartupScreens_;
};


#endif

/* End MSGBOX.HPP ***************************************************/
