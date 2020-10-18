/*
 * B U I L D M C I . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    MachBuildMachineIcon

    The icon that appears in the factory build menu.
    Clicking the icon adds an item to the factory's production queue.
*/

#ifndef _MACHGUI_BUILDMCI_HPP
#define _MACHGUI_BUILDMCI_HPP

#include "base/base.hpp"
#include "gui/icon.hpp"
#include "machlog/machlog.hpp"
#include "machphys/machphys.hpp"

//Forward refs
class GuiDisplayable;
class MachInGameScreen;
class MachLogFactory;
class MachProductionBank;
class MachLogResearchItem;

//orthodox canonical (revoked)
class MachBuildMachineIcon : public GuiIcon
{
public:
    //ctor. Will add a machine of type machineType and subclass subType at
    //hardware level hwLevel to the production queue.
    //pProductionBank represents the state of the production queue.
    //The factory whose production queue is being edited is pFactory.
    //Owning GuiDisplayable is pParent.
    //Has access to pInGameScreen.
    MachBuildMachineIcon( GuiDisplayable* pParent, MachInGameScreen* pInGameScreen,
                          MachProductionBank* pProductionBank, MachLogFactory* pFactory,
                          const MachLogResearchItem& item );

    //dtor.
    virtual ~MachBuildMachineIcon( void );

    void CLASS_INVARIANT;

	static size_t reqWidth();
	static size_t reqHeight();

protected:
	// inherited from GuiButton...
	virtual void doBeDepressed( const GuiMouseEvent& rel );
	virtual void doBeReleased( const GuiMouseEvent& rel );
	virtual void doHandleMouseEnterEvent( const GuiMouseEvent& );
	virtual void doHandleMouseExitEvent( const GuiMouseEvent& );

private:
    // Operations deliberately revoked
    MachBuildMachineIcon( const MachBuildMachineIcon& );
    MachBuildMachineIcon& operator =( const MachBuildMachineIcon& );
    bool operator ==( const MachBuildMachineIcon& );

    friend ostream& operator <<( ostream& o, const MachBuildMachineIcon& t );



    //Data members
    MachInGameScreen* pInGameScreen_; //The ingame screen
    MachProductionBank* pProductionBank_; //Represents the production queue
    MachLogFactory* pFactory_; //The factory being edited
	const MachLogResearchItem& researchItem_;
};


#endif

/* End BUILDMCI.HPP *************************************************/
