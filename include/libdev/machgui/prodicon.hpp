/*
 * P R O D I C O N . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    MachProductionIcon

    Represents an icon in the build menu production queue buffer
*/

#ifndef _MACHGUI_PRODICON_HPP
#define _MACHGUI_PRODICON_HPP

#include "base/base.hpp"

#include "gui/icon.hpp"
#include "machlog/machlog.hpp"
#include "machphys/machphys.hpp"

//forward refs
class GuiDisplayable;

class MachGuiNewProductionIcon : public GuiButtonWithFilledBorder
{
public:

	MachGuiNewProductionIcon( GuiDisplayable *pParent, 
							  const Gui::Coord& rel,
							  const GuiBitmap&,
							  int index );

	virtual ~MachGuiNewProductionIcon();

	static Gui::Box 
	exteriorRelativeBoundary( const GuiBitmap& bitmap, const GuiBorderMetrics& m, const Gui::Coord& rel );

protected:
	virtual void doDisplayInteriorEnabled( const Gui::Coord& absCoord );

private:
	// Operations revoked
	MachGuiNewProductionIcon( const MachGuiNewProductionIcon& );
	MachGuiNewProductionIcon& operator =( const MachGuiNewProductionIcon& );
	bool operator ==( const MachGuiNewProductionIcon& ) const;

	GuiBitmap	bitmap_;
	int index_;
};




// ***************************************************************************************************** //

//forward refs
class GuiDisplayable;
class MachInGameScreen;
class MachGuiBuildProgressBar;
class MachLogProductionUnit;


class MachProductionIcon : public MachGuiNewProductionIcon
//orthodox canonical (revoked)
{
public:
    MachProductionIcon( GuiDisplayable* pParent,
                        MachInGameScreen* pInGameScreen,
						MachLogProductionUnit* pProductionUnit,
						int index );
    virtual ~MachProductionIcon();

    void CLASS_INVARIANT;

	static size_t buttonHeight();
	static size_t buttonWidth();
	
	MachLogProductionUnit& productionUnit() const;	//returns reference to associated MachLogProductionUnit

	void updateProgress( float percentageComplete );
	
protected:
	// inherited from GuiButton...
	virtual void doBeDepressed( const GuiMouseEvent& rel );
	virtual void doBeReleased( const GuiMouseEvent& rel );
	virtual void doHandleMouseEnterEvent( const GuiMouseEvent& );
	virtual void doHandleMouseExitEvent( const GuiMouseEvent& );
	virtual void doHandleContainsMouseEvent( const GuiMouseEvent& );

	void displayCursorPromptText();

private:
    // Operations deliberately revoked
    MachProductionIcon( const MachProductionIcon& );
    MachProductionIcon& operator =( const MachProductionIcon& );
    bool operator ==( const MachProductionIcon& );

    friend ostream& operator <<( ostream& o, const MachProductionIcon& t );

    //Data members
	MachGuiBuildProgressBar* pProgressBar_;
	MachInGameScreen* pInGameScreen_;
	MachLog::ObjectType machineType_;
    MachPhys::WeaponCombo weaponCombo_;
    int subType_; 
    int hwLevel_; 
	MachLogProductionUnit* pProductionUnit_;
	bool needsPromptUpdate_;
};



// ***************************************************************************************************** //

//forward refs
class GuiDisplayable;
class MachInGameScreen;

//orthodox canonical (revoked)
class MachIncSWLevelIcon : public GuiIcon
{
public:
    MachIncSWLevelIcon( GuiDisplayable* pParent, 
    					const Gui::Coord&, 
    					MachInGameScreen* pInGameScreen,
    					MachLog::ObjectType machineType, 
                        int subType, 
                        int hwLevel );
    virtual ~MachIncSWLevelIcon();

    void CLASS_INVARIANT;

	static size_t buttonHeight();
	static size_t buttonWidth();
	MachLogProductionUnit& productionUnit();

protected:
	virtual void doBeDepressed( const GuiMouseEvent& rel );
	virtual void doBeReleased( const GuiMouseEvent& rel );
	virtual void doHandleMouseEnterEvent( const GuiMouseEvent& );
	virtual void doHandleMouseExitEvent( const GuiMouseEvent& );
	
private:
    // Operations deliberately revoked
    MachIncSWLevelIcon( const MachIncSWLevelIcon& );
    MachIncSWLevelIcon& operator =( const MachIncSWLevelIcon& );
    bool operator ==( const MachIncSWLevelIcon& );

    friend ostream& operator <<( ostream& o, const MachIncSWLevelIcon& t );

	MachInGameScreen* pInGameScreen_;
	MachLog::ObjectType machineType_;
    int subType_; 
    int hwLevel_; 
};
                                

#endif

/* End PRODICON.HPP *************************************************/
