/*
 * S T A T D I S P . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachGuiStatisticsDisplay

    A brief description of the class should go in here
*/

#ifndef _MACHGUI_STATDISP_HPP
#define _MACHGUI_STATDISP_HPP

#include "gui/displaya.hpp"
#include "base/base.hpp"
#include "stdlib/string.hpp"
#include "ctl/pvector.hpp"
#include "ctl/vector.hpp"
#include "machphys/machphys.hpp"
#include "gui/gui.hpp"

class MachGuiStatisticsBar;

class MachGuiStatisticsDisplay : public GuiDisplayable
// Canonical form revoked
{
public:
    MachGuiStatisticsDisplay( GuiDisplayable *pParent, const Gui::Coord& topLeft, MachPhys::Race race );
  
    ~MachGuiStatisticsDisplay( void );

 	// Set the values to be displayed
	void setStatistics( int stat1, int stat2, int stat3, int stat4 );

	// forces display to be redrawn if necessary
	void update();

	// Determine if display needs updating
	bool redraw() { return redraw_; }

	// Redraws the display
	virtual void doDisplay();

	static void setStartTime( double time ) { initialTime_ = time; }

	static void setTime( double time );

    void CLASS_INVARIANT;

private:
    friend ostream& operator <<( ostream& o, const MachGuiStatisticsDisplay& t );

    MachGuiStatisticsDisplay( const MachGuiStatisticsDisplay& );
    MachGuiStatisticsDisplay& operator =( const MachGuiStatisticsDisplay& );

	typedef ctl_pvector< MachGuiStatisticsBar > StatBars;
	typedef ctl_vector< int > Stats;

	Gui::Coord 		topLeft_; // top left of the statistics display
	GuiBitmap 		backgroundBmp_;	// bitmap for display background
	StatBars		statBars_; // vector of stat bars
	Stats			stats_; // vector of stat values
	bool 			redraw_;  // flag to determine if display needs to be redrawn
	static float	rampDuration_; // Time taken to perform ramping up of bars
	static float 	initialTime_;	// Initial starting time 
	static float 	ratioComplete_; // Amount of bar to be shown 
};


#endif

/* End STATDISP.HPP *************************************************/
