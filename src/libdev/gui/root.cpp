
#include "gui/manager.hpp"

/* //////////////////////////////////////////////////////////////// */

GuiRoot::GuiRoot( const Gui::Box& abs )
: GuiDisplayable( abs )
{
	// Intentionally Empty
}

GuiRoot::~GuiRoot()
{
	// Intentionally Empty
}

/* //////////////////////////////////////////////////////////////// */

void GuiRoot::becomeRoot()
{
	GuiManager& manager = GuiManager::instance();
	
	if ( manager.hasRoot() )
		manager.root().doBecomeNotRoot();

	manager.changeRoot( this );

	doBecomeRoot();
		
	changed( true );

	POST( &GuiManager::instance().root() == this );
}

bool GuiRoot::recursivelyHasChild( GuiDisplayable * pChild ) const
{
	bool result = pChild != this;
	POST( iff( pChild != this, result ) );
	return result;
}
	
/* //////////////////////////////////////////////////////////////// */

/* End ROOT.CPP *****************************************************/
