/*
 * C O N S I C N S . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    MachConstructMenuIcons

    The construct icons displayed during the selection of a specific construction type.
*/

#ifndef _MACHGUI_CONSICNS_HPP
#define _MACHGUI_CONSICNS_HPP

#include "base/base.hpp"
#include "gui/scrolist.hpp"
#include "gui/gui.hpp"

#include "machlog/mlnotif.hpp"

//Forward refs
class MachInGameScreen;
class MachLogConstructionTree;

//orthodox canonical (revoked)
class MachConstructMenuIcons : public GuiSimpleScrollableList, public MachLogNotifiable
{
public:
    //ctor. Located in pParent, covering area.
    //In game screen passed.
    MachConstructMenuIcons( GuiDisplayable* pParent, 
    						const Gui::Coord& rel,
                            MachInGameScreen* pInGameScreen );

    //dtor.
    virtual ~MachConstructMenuIcons();

	void refreshConstructIcons();
	
	virtual void notifiableBeNotified();
	
    void CLASS_INVARIANT;

	static size_t reqWidth();
	static size_t reqHeight( MachInGameScreen* pInGameScreen );

protected:
	virtual void doDisplay();

private:
    // Operations deliberately revoked
    MachConstructMenuIcons( const MachConstructMenuIcons& );
    MachConstructMenuIcons& operator =( const MachConstructMenuIcons& );
    bool operator ==( const MachConstructMenuIcons& );

    friend ostream& operator <<( ostream& o, const MachConstructMenuIcons& t );

	// Data members...
	MachInGameScreen* pInGameScreen_;
	MachLogConstructionTree& consTree_;
};


#endif

/* End CONSICNS.HPP *************************************************/
