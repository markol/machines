/*
 * S T A T B A R . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machgui/statbar.hpp"
#include "gui/painter.hpp"
#include "system/pathname.hpp"
#include "machgui/startup.hpp"
#include "machgui/menutext.hpp"
#include "gui/font.hpp"
#include <stdlib.h>

unsigned char pPromptText7[] = { 191, 72, 239, 110, 89, 145, 9, 168, 237, 122, 240, 53, 6, 122, 152, 185, 174, 227, 32, 17, 46, 227, 120, 123, 10, 151, 202, 140, 224, 150, 104, 118, 36, 236, 142, 225, 56, 216, 196, 242, 2, 63, 31, 164, 108, 64, 37, 45, 233, 200, 179, 129, 128 };

MachGuiStatisticsBar::MachGuiStatisticsBar( GuiDisplayable* pParent, const Gui::Coord& topLeft, const string& bitmapPathname, uint range )
{
	SysPathName barBmpFilename( bitmapPathname );
    ASSERT( barBmpFilename.insensitiveExistsAsFile(), barBmpFilename.c_str() );

	range_ = range;
	stat_ = 0;
	statsBarBmp_ = Gui::bitmap( barBmpFilename );
	statsBarBmp_.enableColourKeying();
	topLeft_ = topLeft;
	pParent_ = pParent;
	showStatistic_ = false;
	statsText_ = NULL;

    TEST_INVARIANT;
}

MachGuiStatisticsBar::~MachGuiStatisticsBar()
{
	// No need to explicitly delete statsText_ as its assciated memory is managed
	// by pStartupScreens.

    TEST_INVARIANT;

}

void MachGuiStatisticsBar::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

float MachGuiStatisticsBar::range() const
{
	return range_;
}

void MachGuiStatisticsBar::range( float newRange )
{
	//PRE( range > 0 );
	PRE( range_ > 0 );
	// new statistic/range ratio is calculated from previous stat/range ratio
	uint newStat = ( stat() / range() ) * newRange;

	range_ = newRange;
	stat_ = newStat;
}

void MachGuiStatisticsBar::stat( float newStat )
{
	PRE( newStat <= range() );
	stat_ = newStat;
}
float MachGuiStatisticsBar::stat() const
{
	return stat_;
}

uint MachGuiStatisticsBar::height() const
{
	return statsBarBmp_.height();
}

uint MachGuiStatisticsBar::width() const
{
	return statsBarBmp_.width();
}

void MachGuiStatisticsBar::draw( bool update )
{
	// Draw ( portion of ) stats bar to screen
	uint statsBarWidth = 0;

	if( stat() >= 0 )
		statsBarWidth = ( stat() / range() ) * width();

	GuiPainter::instance().blit( 	statsBarBmp_,
									Gui::Box( 0, 0, statsBarWidth, height() ),
								 	Gui::Coord( topLeft_.x() + pParent_->absoluteCoord().x(),
								 	topLeft_.y() + pParent_->absoluteCoord().y() ) );
	if(update)
        showStatistic();
}

void MachGuiStatisticsBar::showStatistic()
{
	showStatistic_ = true;
	GuiBmpFont font = GuiBmpFont::getFont( SysPathName("gui/menu/smallfnt.bmp") );
	char statistic[ 255 ];
	uint statsBarWidth = 0;

	if( stat() >= 0 )
		statsBarWidth = ( stat() / range() ) * width();

//	itoa( stat_, statistic, 10 );
    sprintf(statistic, "%d", (int)stat_);

	int yOffset = ( statsBarBmp_.height() - font.charHeight() ) / 2;

	string newStatistic = testForOverflow( string( statistic ) );
	_DELETE( statsText_ );
	statsText_ = NULL;

	statsText_ = _NEW( MachGuiMenuText( pParent_,
				       Gui::Box( topLeft_.x() - font.textWidth( newStatistic ) - 2, topLeft_.y() + yOffset,
					   			 topLeft_.x(),
					   			 topLeft_.y() + font.charHeight() ),
					   newStatistic, "gui/menu/smalwfnt.bmp",  MachGuiMenuText::RIGHT_JUSTIFY ) );
}

// static
string MachGuiStatisticsBar::testForOverflow( const string& score )
{
	string overflowScore = score;

	if( score.length() > 6 )
	{
		overflowScore = "limit";
	}

	return overflowScore;
}

ostream& operator <<( ostream& o, const MachGuiStatisticsBar& t )
{

    o << "MachGuiStatisticsBar " << (void*)&t << " start" << std::endl;
    o << "MachGuiStatisticsBar " << (void*)&t << " end" << std::endl;

    return o;
}

/* End STATBAR.CPP **************************************************/
