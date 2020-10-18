/*
 * D I S P N O T I . H P P 
 * (c) Charybdis Limited, 1999. All Rights Reserved
 */

/*
    MachGuiDispositionChangeNotifiable

    A brief description of the class should go in here
*/

#ifndef _MACHGUI_DISPNOTI_HPP
#define _MACHGUI_DISPNOTI_HPP

#include "base/base.hpp"
#include "gui/restring.hpp"
#include "machlog/dispnoti.hpp"

class MachGuiStartupScreens;

class MachGuiDispositionChangeNotifiable : public MachLogDispositionChangeNotifiable
// Canonical form revoked
{
public:

    MachGuiDispositionChangeNotifiable( MachGuiStartupScreens* );

    virtual ~MachGuiDispositionChangeNotifiable( void );

	virtual void notifyGeneralDispositionChange();
	//one of these is generated for the actual specifics of a race
	virtual void notifySpecificDispositionChange( MachPhys::Race, MachPhys::Race );
	//This is called when a class changes disposition to move to enemy or neutral, if they started as ally
	virtual void notifyDispositionChangeToNoneAlly( MachPhys::Race, MachPhys::Race );
	//This is called when a class changes disposition to move to ally, if they started as non-ally
	virtual void notifyDispositionChangeToAlly( MachPhys::Race, MachPhys::Race );

    void CLASS_INVARIANT;

private:
    friend ostream& operator <<( ostream& o, const MachGuiDispositionChangeNotifiable& t );

	void doDisplay( GuiResourceString::Id id, MachPhys::Race race1, MachPhys::Race race2 );
	string getDisplayName( MachPhys::Race race );

    MachGuiDispositionChangeNotifiable( const MachGuiDispositionChangeNotifiable& );
    MachGuiDispositionChangeNotifiable& operator =( const MachGuiDispositionChangeNotifiable& );

	MachGuiStartupScreens* pStartupScreens_;

};


#endif

/* End DISPNOTI.HPP *************************************************/
