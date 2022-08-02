/*
 * S L I D E B A R . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachGuiSlideBar

    A brief description of the class should go in here
*/

#ifndef _MACHGUI_SLIDEBAR_HPP
#define _MACHGUI_SLIDEBAR_HPP

#include "base/base.hpp"
#include "gui/displaya.hpp"
#include "machgui/focusctl.hpp"

class MachGuiStartupScreens;

class MachGuiSlideBar : public GuiDisplayable, public MachGuiFocusCapableControl
// Canonical form revoked
{
public:
    MachGuiSlideBar( MachGuiStartupScreens*, GuiDisplayable* pParent, Gui::Coord topLeft, size_t width, 
    				 float minVal, float maxVal );
	// POST( value() >= minValue_ );
	// POST( value() <= maxValue_ );

    MachGuiSlideBar( MachGuiStartupScreens*, GuiDisplayable* pParent, Gui::Coord topLeft, size_t width );
	// POST( minValue_ == 0.0 );
	// POST( maxValue_ == 1.0 );
	// POST( value() == minValue_ );

    ~MachGuiSlideBar();

	// Set the range of values the slide bar can produce.
	void minMax( float minV, float maxV );
	// PRE( minV < maxV );

	// Get value slide bar is pointing at ( between min and max values ).
	float value() const;

	// Change the value, this will cause the slide bar to be redrawn.
	void value( float );
	//PRE( newVal >= minValue_ and newVal <= maxValue_ );

	// Called when the slide button is moved into a new position.
	virtual void valueChanged( float value );

	static size_t reqHeight();

	virtual bool doHandleNavigationKey( NavKey, MachGuiFocusCapableControl** );

	virtual void hasFocus( bool newValue );

    void CLASS_INVARIANT;

protected:
	virtual void doDisplay();

	virtual void doHandleMouseClickEvent( const GuiMouseEvent& rel );
	virtual void doHandleMouseExitEvent( const GuiMouseEvent& rel );
	virtual void doHandleMouseEnterEvent( const GuiMouseEvent& rel );
	virtual void doHandleContainsMouseEvent( const GuiMouseEvent& rel );

	void barMoved( Gui::XCoord newBarPos );

private:
    friend ostream& operator <<( ostream& o, const MachGuiSlideBar& t );

    MachGuiSlideBar( const MachGuiSlideBar& );
    MachGuiSlideBar& operator =( const MachGuiSlideBar& );

	float barPos_;
	float minValue_;
	float maxValue_;
	bool highlighted_;
};


#endif

/* End SLIDEBAR.HPP *************************************************/
