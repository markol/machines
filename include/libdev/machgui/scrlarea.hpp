/*
 * S C R L A R E A . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachGuiScrollArea

    A brief description of the class should go in here
*/

#ifndef _MACHGUI_SCRLAREA_HPP
#define _MACHGUI_SCRLAREA_HPP

#include "base/base.hpp"
#include "gui/displaya.hpp"
#include "system/pathname.hpp"
#include "gui/icon.hpp"

class MachGuiScrollBar;
class GuiSimpleScrollableList;
class MachInGameScreen;
class MachGuiScrollBar;
class MachGuiScrollArea;

typedef std::pair< SysPathName, SysPathName > SysPathNames;

class MachGuiScrollButton : public GuiIcon
// cannonical from revoked
{
public:
	enum ScrollDir { UP, DOWN, TOP, BOTTOM };

	MachGuiScrollButton( GuiDisplayable *pParent,
						 const Gui::Coord& rel,
						 const SysPathNames& bitmaps,
						 GuiSimpleScrollableList*,
						 ScrollDir,
						 MachGuiScrollArea* );
	virtual ~MachGuiScrollButton();

	void update();

	static size_t reqWidth();
	static size_t reqHeight();

protected:
	virtual void doBeDepressed( const GuiMouseEvent& rel );
	virtual void doBeReleased( const GuiMouseEvent& rel );

	virtual const GuiBitmap& getBitmap() const;

private:
	// Operations revoked
	MachGuiScrollButton( const MachGuiScrollButton& );
	MachGuiScrollButton& operator =( const MachGuiScrollButton& );
	bool operator ==( const MachGuiScrollButton& ) const;

	// Data members...
	GuiSimpleScrollableList* pIcons_;
	ScrollDir scrollDir_;
	MachGuiScrollArea* pScrollArea_;
};

class MachGuiScrollBar : public GuiDisplayable
{
public:
	enum ScrollType { UPDOWN, TOPBOTTOM };

	MachGuiScrollBar(	MachGuiScrollArea *pParent,
						const Gui::Coord& rel,
						GuiSimpleScrollableList*,
						MachInGameScreen*,
						ScrollType );
	~MachGuiScrollBar();

	static size_t reqWidth();
	static size_t reqHeight();

	void update();

protected:
	virtual void doDisplay();

private:
	MachGuiScrollButton* pUpButton_;
	MachGuiScrollButton* pDownButton_;
	GuiSimpleScrollableList* pIcons_;
	MachInGameScreen* pInGameScreen_;
};

class MachGuiScrollArea	: public GuiDisplayable
// Canonical form revoked
{
public:
    MachGuiScrollArea( GuiDisplayable* pParent, const Gui::Box& area, MachInGameScreen* );
    ~MachGuiScrollArea( void );

	void initialise();

	void update();

protected:
	virtual GuiSimpleScrollableList* createList( GuiDisplayable* pParent, const Gui::Coord& rel, MachInGameScreen* ) = 0;
	virtual void doDisplay();

private:
	// Operations revoked
    MachGuiScrollArea( const MachGuiScrollArea& );
    MachGuiScrollArea& operator =( const MachGuiScrollArea& );
    bool operator ==( const MachGuiScrollArea& );

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachGuiScrollArea& t );

	GuiSimpleScrollableList* pList_;
	MachGuiScrollBar* pLHSScrollBar_;
	MachGuiScrollBar* pRHSScrollBar_;
	MachInGameScreen* pInGameScreen_;
};


#endif

/* End SCRLAREA.HPP *************************************************/
