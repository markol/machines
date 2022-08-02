
#ifndef _GUI_ICON_HPP
#define _GUI_ICON_HPP

#include "gui/gui.hpp"
#include "gui/button.hpp"

#include "system/pathname.hpp"

/* //////////////////////////////////////////////////////////////// */

class GuiIcon : public GuiBitmapButtonWithFilledBorder
{
public:

	GuiIcon(GuiDisplayable *pParent,
			const Gui::Coord& rel,
			const std::pair< SysPathName, SysPathName >& bitmap );

	GuiIcon(GuiDisplayable *pParent,
			const Gui::Coord& rel,
			const SysPathName& bmp );

protected:
	virtual ~GuiIcon();

private:
	GuiIcon( const GuiIcon& );
	GuiIcon& operator =( const GuiIcon& );
	bool operator ==( const GuiIcon& ) const;
};

/* //////////////////////////////////////////////////////////////// */

#endif	// #ifndef _GUI_ICON_HPP
