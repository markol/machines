/*
 * S T A T B A R . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachGuiStatisticsBar

    A brief description of the class should go in here
*/

#ifndef _MACHGUI_STATBAR_HPP
#define _MACHGUI_STATBAR_HPP

#include "base/base.hpp"
#include "gui/gui.hpp"
#include "stdlib/string.hpp"

class GuiDisplayable;
class MachGuiMenuText;

class MachGuiStatisticsBar
// Canonical form revoked
{
public:
    MachGuiStatisticsBar( 	GuiDisplayable* pParent, const Gui::Coord& topLeft,
    						const string& bitmapPathname, uint range = 100 );
    ~MachGuiStatisticsBar( void );

	// set/get maximum value that can be displayed by Statistics bar
	float range( void ) const;
	// Setting range causes stat bar to be redrawn
	void range( float newRange );

	// set/get current stat value
	float stat( void ) const;
	void stat( float newStat );

	// get bar dimensions
	uint height( void ) const;
	uint width( void ) const;

	// draws statistic bar on screen
	void draw( bool update = true );

	// Draws numerical value at the end of the bar
	void showStatistic( void );

    void CLASS_INVARIANT;

private:
	// test for stat if it will not fit in bounding box - will return the string 'limit' if it
	// it wont, else returns score
	static string testForOverflow( const string& score );

    friend ostream& operator <<( ostream& o, const MachGuiStatisticsBar& t );

    MachGuiStatisticsBar( const MachGuiStatisticsBar& );
    MachGuiStatisticsBar& operator =( const MachGuiStatisticsBar& );

	float range_; // maximum value to be displayed ( defaulted to 100 )
	float stat_;	// current value ( defaulted to 0)
	float ratioComplete_; // amount of bar that has been drawn
	GuiBitmap statsBarBmp_; // Bitmap representing bar
	Gui::Coord topLeft_; // top left coord of bar
	GuiDisplayable* pParent_; // Pointer to top level window
	bool showStatistic_; // flag to determine if stat value should be shown at end of bar.
	MachGuiMenuText* statsText_; // Statistic text shown at end of bar
};


#endif

/* End STATBAR.HPP **************************************************/
