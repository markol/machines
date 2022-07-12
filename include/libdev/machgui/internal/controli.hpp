/*
 * C O N T R O L I . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachGuiControlPanelImpl

    A brief description of the class should go in here
*/

#ifndef _MACHGUI_CONTROLI_HPP
#define _MACHGUI_CONTROLI_HPP

#include "base/base.hpp"
#include "ctl/pvector.hpp"
#include "gui/gui.hpp"

class MachGuiControlPanelImpl
// Canonical form revoked
{
public:
    MachGuiControlPanelImpl();
    ~MachGuiControlPanelImpl();

    void CLASS_INVARIANT;

private:
    friend ostream& operator <<( ostream& o, const MachGuiControlPanelImpl& t );

    MachGuiControlPanelImpl( const MachGuiControlPanelImpl& );
    MachGuiControlPanelImpl& operator =( const MachGuiControlPanelImpl& );

	friend class MachGuiControlPanel;

    //Data members...
	struct RedrawArea
	{
		Gui::Box area_;
		size_t count_;
	};
	typedef ctl_pvector< RedrawArea > RedrawAreas;

	RedrawAreas redrawAreas_;
	GuiBitmap 	decals_[8];
	Gui::Coord 	decalCoord_[8];
};


#endif

/* End CONTROLI.HPP *************************************************/
