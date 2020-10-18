#ifndef _GUI_MANAGEI
#define _GUI_MANAGEI

#include "gui/manager.hpp"

class GuiRoot;
class GuiDisplayable;
 
class GuiManagerImpl
{
private:
	GuiManagerImpl( const GuiColourScheme& colourScheme )
	:	colourScheme_( colourScheme )
	{}
	
	//Data members 
	GuiColourScheme			colourScheme_;
	GuiRoot *				pRoot_;
	GuiDisplayable *		pMouseFocus_;
	GuiDisplayable *		pKeyboardFocus_;
	GuiDisplayable *		pCharacterFocus_;

	friend class GuiManager; 
};
		
#endif	// _GUI_MANAGEI