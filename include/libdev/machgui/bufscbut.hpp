/*
 * B U F S C B U T . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachGuiBufferScrollButton

    A brief description of the class should go in here
*/

#ifndef _BUFSCBUT_HPP
#define _BUFSCBUT_HPP

#include "base/base.hpp"

#include "gui/icon.hpp"

//Forward refs
class MachProductionIcons;
class MachInGameScreen;
class MachProductionBank;

class MachGuiBufferScrollButton : public GuiIcon
// cannonical from revoked
{
public:
	enum ScrollDir { LEFT, RIGHT };

	MachGuiBufferScrollButton(	GuiDisplayable *pParent,
								const Gui::Coord& rel,
								const SysPathName& bitmap,
								MachProductionIcons*,
						 		ScrollDir,
						 		MachInGameScreen*,
						 		MachProductionBank* );
								
	virtual ~MachGuiBufferScrollButton();
	
	void CLASS_INVARIANT;

	static size_t width()
	{
		return 17; // TODO: remove hard coding
	}

	void update();

protected:
	virtual void doBeDepressed( const GuiMouseEvent& );
	virtual void doBeReleased( const GuiMouseEvent& );

	virtual const GuiBitmap& getBitmap() const;

private:
	// Operations revoked
	MachGuiBufferScrollButton( const MachGuiBufferScrollButton& );
	MachGuiBufferScrollButton& operator =( const MachGuiBufferScrollButton& );
	bool operator ==( const MachGuiBufferScrollButton& ) const;

	friend ostream& operator <<( ostream& o, const MachGuiBufferScrollButton& t );
	
	// Data members...
	MachProductionIcons* pProductionIcons_;
	MachInGameScreen* pInGameScreen_;
	ScrollDir scrollDir_;
	MachProductionBank* pProductionBank_;
};


#endif

/* End BUFSCBUT.HPP *************************************************/
