/*
 * R E S O D I A L . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    MachGuiResourceDial

    Each instance represents one of the resource dials on the in game screen,
    depicting the current level of BMU, machine and constructions, etc.
    Its display consists of a background bitmap, with the current level of
    resource drawn as text over it.
*/

#ifndef _MACHGUI_RESODIAL_HPP
#define _MACHGUI_RESODIAL_HPP

#include "base/base.hpp"
#include "gui/image.hpp"
#include "gui/gui.hpp"

//Forward refs
class SysPathName;

//orthodox canonical (revoked)
class MachGuiResourceDial 
: public GuiImage
{
public:
    //ctor. Background bitmap path is bitmapName.
    //Parent is pParent.
    //Position of entire displayable relative to parent is relativePosition.
    //Offset to text from local origin is textOffset.
    MachGuiResourceDial( const SysPathName& bitmapName, GuiDisplayable* pParent,
                         const Gui::Coord& relativePosition, const Gui::Coord& textOffset );

    //dtor.
    virtual ~MachGuiResourceDial();

    //Redisplay with using newValue
    void updateValue( int newValue );

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachGuiResourceDial& t );

protected:

	virtual void doDisplay();

private:
    // Operations deliberately revoked
    MachGuiResourceDial( const MachGuiResourceDial& );
    MachGuiResourceDial& operator =( const MachGuiResourceDial& );
    bool operator ==( const MachGuiResourceDial& );

    //Data members
    int lastDisplayedValue_; //the last value displayed on an update
    Gui::Coord textOffset_; //Offset to draw the text value at
};


#endif

/* End RESODIAL.HPP *************************************************/
