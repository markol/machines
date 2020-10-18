/*
 * E M A I L . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved.
 */

#include "machgui/email.hpp"


/* //////////////////////////////////////////////////////////////// */

MachMenuBar::MachMenuBar( MachInGameScreen * pParent, const Gui::Coord& rel )
: //GuiDisplayable( pParent, Gui::Box( 0, 0, rel.x(), rel.y() ) )
  GuiDisplayable( _REINTERPRET_CAST(GuiDisplayable*, pParent), Gui::Box( 0, 0, rel.x(), rel.y() ) )
{
    // TODO check cast above
	// Intentionally Empty
}

MachEmailArrivalBar::MachEmailArrivalBar( MachInGameScreen * pParent,
											const Gui::Coord& rel )
: //GuiDisplayable( pParent, Gui::Box( 0, 0, rel.x(), rel.y() ) )
  GuiDisplayable( _REINTERPRET_CAST(GuiDisplayable*, pParent), Gui::Box( 0, 0, rel.x(), rel.y() ) )
{
    // TODO check cast above
	// Intentionally Empty
}

/* End EMAIL.CPP ***************************************************/
