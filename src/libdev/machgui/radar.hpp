/*
 * R A D A R . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachGuiRadar

    A brief description of the class should go in here
*/

#ifndef _MACHGUI_RADAR_HPP
#define _MACHGUI_RADAR_HPP

#include "base/base.hpp"
#include "gui/displaya.hpp"

class MachGuiRadarImpl;
class MachActor;
class MachLog1stPersonHandler;

class MachGuiRadar : public GuiDisplayable
// Canonical form revoked
{
public:
    MachGuiRadar( GuiDisplayable* pParent, const Gui::Coord& relPos );
    ~MachGuiRadar();

    void CLASS_INVARIANT;

	void actor( MachActor* );
	void resetActor();

	void logHandler( MachLog1stPersonHandler* );
	void resetLogHandler();

	void initialise();

	void loadBitmaps();

	void unloadBitmaps();

protected:
	virtual void doDisplay();

	void displayHealthArmour();
	void displayRadarBlips();
	void displayMotionDirection();
	void displayAnimatedRadarFrame();

	static GuiBitmap* machineImage();
	static GuiBitmap* constructionImage();
	static GuiBitmap* podImage();
	static GuiBitmap* missileEmplacementImage();
	static GuiBitmap& debrisImage();
	static GuiBitmap& artefactImage();
	static GuiBitmap& oreImage();
	static GuiBitmap* arrowImage();

private:
    friend ostream& operator <<( ostream& o, const MachGuiRadar& t );

    MachGuiRadar( const MachGuiRadar& );
    MachGuiRadar& operator =( const MachGuiRadar& );

	// Data members...
	MachGuiRadarImpl* pImpl_;
};


#endif

/* End RADAR.HPP ****************************************************/
