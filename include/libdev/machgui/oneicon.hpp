/*
 * O N E I C O N . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachGuiCorralSingleIconInfo

    A brief description of the class should go in here
*/

#ifndef _MACHGUI_ONEICON_HPP
#define _MACHGUI_ONEICON_HPP

#include "base/base.hpp"
#include "gui/displaya.hpp"
#include "world4d/observer.hpp"

class MachActor;
class MachGuiCorralResource;
class MachInGameScreen;
class MachGuiSelectInsideBuildingIcon;
class MachGuiInHeadIcon;

class MachGuiCorralSingleIconInfo : public GuiDisplayable
// Canonical form revoked
{
public:
    MachGuiCorralSingleIconInfo( GuiDisplayable* pParent, Gui::Coord rel, MachInGameScreen* pInGameScreen );
    ~MachGuiCorralSingleIconInfo();

    void CLASS_INVARIANT;

	static size_t reqWidth();
	static size_t reqHeight();

	void doDisplay();

	void setActor( MachActor* );

private:
    MachGuiCorralSingleIconInfo( const MachGuiCorralSingleIconInfo& );
    MachGuiCorralSingleIconInfo& operator =( const MachGuiCorralSingleIconInfo& );
    bool operator ==( const MachGuiCorralSingleIconInfo& );

    friend ostream& operator <<( ostream& o, const MachGuiCorralSingleIconInfo& t );

	MachActor *pActor_;
	MachInGameScreen* pInGameScreen_;
};

class MachGuiCorralSingleIcon : public GuiDisplayable, public W4dObserver
// cannonical form revoked
{
public:
	MachGuiCorralSingleIcon( GuiDisplayable* pParent, const Gui::Coord& rel, MachInGameScreen* pInGameScreen );
	virtual ~MachGuiCorralSingleIcon();

	static size_t reqWidth();
	static size_t reqHeight();
			
    void setActor( MachActor* pActor );
	void setActor( MachActor*, bool forceUpdate );

	void clear();

protected:	
	virtual void doDisplay();
		
private:
	// Operations revoked
	MachGuiCorralSingleIcon( const MachGuiCorralSingleIcon& );
	MachGuiCorralSingleIcon& operator =( const MachGuiCorralSingleIcon& );
	bool operator ==( const MachGuiCorralSingleIcon& ) const;

	virtual bool beNotified( W4dSubject* pSubject,
	                         W4dSubject::NotificationEvent event, 
	                         int clientData = 0 );
    virtual void domainDeleted( W4dDomain* pDomain );

	MachGuiCorralSingleIconInfo* pIconInfo_;
	MachGuiCorralResource* pIcon_;
	MachActor* pActor_;
	MachInGameScreen* pInGameScreen_;
	MachGuiSelectInsideBuildingIcon* pSelectInsideBuildingIcon_;
	MachGuiInHeadIcon* pInHeadIcon_;
	bool isObservingActor_;
};

#endif

/* End ONEICON.HPP **************************************************/
