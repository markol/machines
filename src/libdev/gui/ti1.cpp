

#define _INSTANTIATE_TEMPLATE_CLASSES

#include "stdlib/string.hpp"
#include "ctl/list.hpp"
#include "ctl/vector.hpp"
#include "device/keyboard.hpp"
#include "gui/font.hpp"
#include "gui/internal/fontcore.hpp"

class GuiDisplayable;

void Gui_ti1()
{
	static ctl_vector< GuiDisplayable * >	dummy1;
	static ctl_list< GuiDisplayable * >		dummy2;
	static ctl_list< DevKey::ScanCode >		dummy3;
	static ctl_vector< GuiBmpFontCoreCharData > dummy10;
	static ctl_vector< GuiBmpFont > 		dummy11;
}

/* End **************************************************************/
