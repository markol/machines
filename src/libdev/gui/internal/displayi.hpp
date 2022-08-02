#ifndef _GUI_DISPLAYI
#define _GUI_DISPLAYI

#include "ctl/vector.hpp"

class GuiDisplayableImpl
{
private:
	GuiDisplayable*				pParent_;
	Gui::Box					relativeBox_;
	Gui::Box					absoluteBox_;
	bool						isVisible_;
	int 						changed_;
	bool						redrawEveryFrame_;
	GuiDisplayable::Children	children_[ GuiDisplayable::NUMLAYERS ];
	GuiDisplayable::Children	allChildren_;
	bool						useFastSecondDisplay_;

	friend class GuiDisplayable;
};

#endif	// _GUI_DISPLAYI