/*
 * M O D E O B S V . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

#include "render/internal/modeobsv.hpp"
//#include "render/internal/d3d.hpp"
#include "render/internal/displayi.hpp"
#include "render/display.hpp"

RenIDisplayModeObserver::RenIDisplayModeObserver(RenDisplay* d):
	display_(d)
{
	PRE(d);
	display_->displayImpl().addObserver(this);
}

RenIDisplayModeObserver::~RenIDisplayModeObserver()
{
	display_->displayImpl().removeObserver(this);
}

/* End MODEOBSV.CPP *************************************************/
