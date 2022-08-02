#ifndef _GUI_SCROLISI
#define _GUI_SCROLISI

#include "ctl/pvector.hpp"
#include "gui/scrolist.hpp"

class GuiSimpleScrollableListImpl
{
	private:
		bool canScrollFoward_;
		bool canScrollBackward_;
		size_t scrollOffset_;
		size_t scrollInc_;
		size_t horizontalSpacing_;
		size_t verticalSpacing_;
		size_t numPositions_;
		GuiSimpleScrollableList::Observers observers_;
		
		friend class GuiSimpleScrollableList;
};

#endif	// _GUI_SCROLISI