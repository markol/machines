
#include "gui/icon.hpp"
#include "gui/painter.hpp"

//////////////////////////////////////////////////////////////////////


GuiIcon::GuiIcon( GuiDisplayable *pParent,
						const Gui::Coord& rel,
						const std::pair< SysPathName, SysPathName >& bmps )
: GuiBitmapButtonWithFilledBorder( pParent, rel,
									GuiBorderMetrics( 1, 1, 1 ),
									GuiFilledBorderColours( Gui::BLACK(), Gui::LIGHTGREY(), Gui::DARKGREY(), Gui::RED() ),
									Gui::bitmap( bmps.first ),
									Gui::Coord( 1, 1 ) )
{
	// Intentionally Empty
}

GuiIcon::GuiIcon( 	GuiDisplayable *pParent,
				  	const Gui::Coord& rel,
				  	const SysPathName& bmp )
: GuiBitmapButtonWithFilledBorder( 	pParent, rel,
									GuiBorderMetrics( 1, 1, 1 ),
									GuiFilledBorderColours( Gui::BLACK(), Gui::LIGHTGREY(), Gui::DARKGREY(), Gui::RED() ),
									Gui::bitmap( bmp ),
									Gui::Coord( 1, 1 ) )
{
	// Intentionally Empty
}

GuiIcon::~GuiIcon()
{
	// Intentionally Empty
}

/* //////////////////////////////////////////////////////////////// */

/* End ICON.CPP *****************************************************/
