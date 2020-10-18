/*
 * C O R R A L . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved.
 */

#ifndef _MACH_CORRAL_HPP
#define _MACH_CORRAL_HPP

#include "gui/displaya.hpp"
#include "gui/icon.hpp"
#include "gui/iconseq.hpp"
#include "gui/scrolist.hpp"

#include "machphys/machphys.hpp"

#include "machgui/gui.hpp"
#include "machgui/scrlarea.hpp"

#include "world4d/observer.hpp"

#include "system/pathname.hpp"
#include "ctl/list.hpp"
#include "ctl/pvector.hpp"

//////////////////////////////////////////////////////////////////////

class MachCorralIcons;
class MachInGameScreen;
class MachActor;
class MachGuiHealthBar;

//##NA start//////////////////////////////////////////////////////////
class MachGuiNewCorralIcon
: public GuiButtonWithFilledBorder
{
public:

	MachGuiNewCorralIcon( GuiDisplayable *pParent, 
										const Gui::Coord& rel,
										const GuiBitmap& );

	virtual ~MachGuiNewCorralIcon();

	static Gui::Box 
	exteriorRelativeBoundary( const GuiBitmap& bitmap, const GuiBorderMetrics& m, const Gui::Coord& rel );

protected:
	virtual void doDisplayInteriorEnabled( const Gui::Coord& absCoord );

	GuiBitmap& corralBitmap();

private:
	MachGuiNewCorralIcon( const MachGuiNewCorralIcon& );
	MachGuiNewCorralIcon& operator =( const MachGuiNewCorralIcon& );
	bool operator ==( const MachGuiNewCorralIcon& ) const;

	GuiBitmap	bitmap_;
};
//##NA end////////////////////////////////////////////////////////////


class MachGuiCorralResource
// cannonical form revoked
: public MachGuiNewCorralIcon,
  public W4dObserver
{
public:
	MachGuiCorralResource( 	GuiDisplayable *pParent, 
							const Gui::Coord& rel,
							MachActor * pActor,
							MachInGameScreen* pInGameScreen );

	virtual ~MachGuiCorralResource();


	static size_t buttonWidth();
	static size_t buttonHeight();

	///////////////////////////////

	// true iff this observer is to exist in this subject's list of observers
	// following this call. This will typically be implemented using double dispatch.
	// The clientData is of interest only if event == CLIENT_SPECIFIC.Interpretation
	// is client defined.

	virtual bool beNotified( W4dSubject* pSubject,
	                         W4dSubject::NotificationEvent event, 
	                         int clientData = 0 );

    // Informs observer that an observed domain is being deleted.
    // This observer need not call the W4dDomain::detach() method - this
    // will be done automatically.
    virtual void domainDeleted( W4dDomain* pDomain );
	
protected:
	virtual void doBeDepressed( const GuiMouseEvent& rel );
	virtual void doBeReleased( const GuiMouseEvent& rel );
	virtual void doHandleMouseExitEvent( const GuiMouseEvent& );
	virtual void doHandleMouseEnterEvent( const GuiMouseEvent& );
	virtual void doHandleContainsMouseEvent( const GuiMouseEvent& );

	virtual void doDisplayInteriorEnabled( const Gui::Coord& absCoord );

private:
	MachGuiCorralResource( const MachGuiCorralResource& );
	MachGuiCorralResource& operator =( const MachGuiCorralResource& );
	bool operator ==( const MachGuiCorralResource& ) const;


	void updateHealthBar();

	MachCorralIcons *	pParent_;
	MachActor *			pActor_;
	MachGuiHealthBar *	pHealthBar_;
	bool				isObservingActor_;
	MachInGameScreen*	pInGameScreen_;
};

//////////////////////////////////////////////////////////////////////

class MachCorralIcons;

class MachGuiCorral	: public MachGuiScrollArea
// cannonical form revoked
{
public:
	MachGuiCorral( GuiDisplayable* pParent, const Gui::Coord&, MachInGameScreen* );
	virtual ~MachGuiCorral();

	static size_t reqWidth();
	static size_t reqHeight( MachInGameScreen* pInGameScreen );

	void clear();
					
	typedef ctl_list< MachActor * > Actors;

	const Actors& actors() const;

    //True if pActor is listed as selected
    bool isSelected( MachActor* pActor ) const;

    //Add pActor to the list of selected actors and add its icon
    void add( MachActor* pActor );
    //PRE( not isSelected( pActor ) );

    //Add actors to the list of selected actors and add their icons
	void add( const ctl_pvector<MachActor>& actors );

    //Remove pActor from the selected list, and remove its icon
    void remove( MachActor* pActor );
    //PRE( isSelected( pActor ) );

    //Remove actors from the selected list, and remove their icons
	void remove( const ctl_pvector<MachActor>& actors );
	
protected:
	virtual GuiSimpleScrollableList* createList( GuiDisplayable* pParent, const Gui::Coord& rel, MachInGameScreen* );
	
	Actors& actors();
		
private:
	// Operations revoked
	MachGuiCorral( const MachGuiCorral& );
	MachGuiCorral& operator =( const MachGuiCorral& );
	bool operator ==( const MachGuiCorral& ) const;

	void eraseAllActors();
	MachPhys::Race race() const;
	friend class MachCorralIcons;

	MachCorralIcons* pIcons_;
	Actors actors_;
	MachInGameScreen* pInGameScreen_;
};

/* //////////////////////////////////////////////////////////////// */

class MachCorralIcons : public GuiSimpleScrollableList
// cannonical from revoked
{
public:
	MachCorralIcons( MachGuiCorral * pParent, const Gui::Coord& rel, MachInGameScreen* pInGameScreen );
	virtual ~MachCorralIcons();

	static size_t reqWidth();
	static size_t reqHeight( MachInGameScreen* pInGameScreen );

	static size_t columns();

	void add( MachActor* pActor );
	void change();
	
protected:
	virtual void doDisplay();

private:
	// Operations revoked
	MachCorralIcons( const MachCorralIcons& );
	MachCorralIcons& operator =( const MachCorralIcons& );
	bool operator ==( const MachCorralIcons& ) const;
	
	//Data members...
	MachGuiCorral *	pCorral_;
	MachInGameScreen* pInGameScreen_;
};


#endif // #ifndef _MACH_CORRAL_HPP

/* End CORRAL.HPP ***************************************************/