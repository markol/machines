/*
 * G E N S T A T S . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachGuiGeneralStatistics

    A brief description of the class should go in here
*/

#ifndef _MACHGUI_GENSTATS_HPP
#define _MACHGUI_GENSTATS_HPP

#include "base/base.hpp"
#include "gui/gui.hpp"
#include "gui/displaya.hpp"
#include "machgui/statistics.hpp"
#include "machphys/machphys.hpp"
#include "machgui/statdisp.hpp"

class MachGuiGeneralStatistics 
: 	public MachGuiStatistics, 
 	public GuiDisplayable
// Canonical form revoked
{
public:
    MachGuiGeneralStatistics( GuiDisplayable *pParent, Gui::Coord topLeft, uint titleString, MachPhys::Race race );
    MachGuiGeneralStatistics( GuiDisplayable *pParent, Gui::Coord topLeft, uint titleString, MachPhys::Race race, const string& player );
    ~MachGuiGeneralStatistics( void );

    void CLASS_INVARIANT;

	virtual void update();

	virtual void setStatistics( int, int, int, int );

	virtual void doDisplay( void );

private:
	// Truncate player's name if it will not fit in bounding box.
	static string truncate( const string& name );

    friend ostream& operator <<( ostream& o, const MachGuiGeneralStatistics& t );

    MachGuiGeneralStatistics( const MachGuiGeneralStatistics& );
    MachGuiGeneralStatistics& operator =( const MachGuiGeneralStatistics& );

	MachGuiStatisticsDisplay statDisplay_;
};


#endif

/* End GENSTATS.HPP *************************************************/
