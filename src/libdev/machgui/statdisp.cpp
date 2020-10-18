/*
 * S T A T D I S P . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include <stdlib.h>
#include "machgui/statdisp.hpp"
#include "system/pathname.hpp"
#include "machphys/machphys.hpp"
#include "gui/painter.hpp"
#include "gui/font.hpp"
#include "machgui/statbar.hpp"
#include "machgui/menutext.hpp"

float MachGuiStatisticsDisplay::initialTime_ = 0;
float MachGuiStatisticsDisplay::ratioComplete_ = 0;
float MachGuiStatisticsDisplay::rampDuration_ = 1.0;


MachGuiStatisticsDisplay::MachGuiStatisticsDisplay( GuiDisplayable *pParent, const Gui::Coord& topLeft, MachPhys::Race race )
:	GuiDisplayable( pParent, Gui::Boundary( topLeft, 75, 42 ) ),
	topLeft_( topLeft ),
	redraw_( true )
{
	SysPathName	backgroundBmpFile( "gui/menu/barBack.bmp" );
	ASSERT( backgroundBmpFile.existsAsFile(), backgroundBmpFile.c_str() );

	backgroundBmp_ = Gui::bitmap( backgroundBmpFile );

	string bar1, bar2, bar3, bar4;
	switch( race )
	{
		case MachPhys::N_RACES:
			bar1 = "gui/menu/redBar.bmp";
			bar2 = "gui/menu/blueBar.bmp";
			bar3 = "gui/menu/greenBar.bmp";
			bar4 = "gui/menu/yelloBar.bmp";
			break;
		case MachPhys::RED:		bar1 = bar2 = bar3 = bar4 = "gui/menu/redBar.bmp"; break;
		case MachPhys::YELLOW:	bar1 = bar2 = bar3 = bar4 = "gui/menu/yelloBar.bmp"; break;
		case MachPhys::BLUE:	bar1 = bar2 = bar3 = bar4 = "gui/menu/blueBar.bmp"; break;
		case MachPhys::GREEN:	bar1 = bar2 = bar3 = bar4 = "gui/menu/greenBar.bmp"; break;
	}
	uint x = 33; uint y = 3;	// relative to top left of stats display background
	statBars_.push_back( _NEW( MachGuiStatisticsBar( this, Gui::Coord(x, y), bar1 ) ) );
	uint height = (*statBars_.begin())->height() + 2;
	statBars_.push_back( _NEW( MachGuiStatisticsBar( this, Gui::Coord(x, y+=height), bar2 ) ) );
	statBars_.push_back( _NEW( MachGuiStatisticsBar( this, Gui::Coord(x, y+=height), bar3 ) ) );
	statBars_.push_back( _NEW( MachGuiStatisticsBar( this, Gui::Coord(x, y+=height), bar4 ) ) );

	stats_.push_back( 0 );
	stats_.push_back( 0 );
	stats_.push_back( 0 );
	stats_.push_back( 0 );

    TEST_INVARIANT;
}

MachGuiStatisticsDisplay::~MachGuiStatisticsDisplay()
{
	for( StatBars::iterator i = statBars_.begin(); i != statBars_.end(); i++ )
		_DELETE( *i );

	TEST_INVARIANT;

}

void MachGuiStatisticsDisplay::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

void MachGuiStatisticsDisplay::setStatistics( int stat1, int stat2, int stat3, int stat4 )
{
	stats_[0] = stat1;
	stats_[1] = stat2;
	stats_[2] = stat3;
	stats_[3] = stat4;

 	// find largest statistic
	// kludge - should be 0, but would mess	up range precondition
	int maxStatistic = 1;
	for( Stats::iterator stat = stats_.begin(); stat != stats_.end(); ++stat )
	{
		if( *stat > maxStatistic) maxStatistic = *stat;
	}
	// set range in terms of highest statistic
	for( StatBars::iterator bar = statBars_.begin(); bar != statBars_.end(); ++bar )
	{
		(*bar)->range( maxStatistic );
	}
}

void MachGuiStatisticsDisplay::update( void )
{
	// Force object to be redrawn ( will indirectly call doDisplay() )
	changed();
}

// virtual
void MachGuiStatisticsDisplay::doDisplay( void )
{
	PRE( statBars_.size() == stats_.size() );

	if( redraw() )
	{
		// Draw statistics bars on background bitmap, ramping up bars until actual value is reached
		GuiPainter::instance().blit( 	backgroundBmp_,
										Gui::Box( 0, 0, backgroundBmp_.width(),
						 				backgroundBmp_.height() ),
										absoluteBoundary().minCorner() );

		bool redraw = false;
		for( uint i = 0; i < statBars_.size(); ++i )
		{
			if( statBars_[i]->stat() < stats_[i] )
			{
 				float newStat = ratioComplete_*stats_[i];
				// Check for rounding errors
				if( newStat > statBars_[i]->range()	)
				{
					newStat = statBars_[i]->range();
				}
				statBars_[i]->stat( newStat );
				redraw = true;
			}
			// Special case - -ve numbers: we want to set the statistic to be -ve,
			// but not do the ramping
			else if( stats_[i] < 0 )
			{
				statBars_[i]->stat( stats_[i] );
			}
		}
		for( StatBars::iterator bar = statBars_.begin(); bar != statBars_.end(); ++bar )
		{
			(*bar)->draw(redraw);
		}
//		redraw_ = redraw;
		redraw_ = true; // Always redraw
	}
}

// static
void MachGuiStatisticsDisplay::setTime( double newTime )
{
	float interval = newTime - initialTime_;
	ratioComplete_ = interval/rampDuration_;
}

ostream& operator <<( ostream& o, const MachGuiStatisticsDisplay& t )
{

    o << "MachGuiStatisticsDisplay " << (void*)&t << " start" << std::endl;
    o << "MachGuiStatisticsDisplay " << (void*)&t << " end" << std::endl;

    return o;
}

/* End STATDISP.CPP *************************************************/
