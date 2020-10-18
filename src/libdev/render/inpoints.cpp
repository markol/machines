/*
 * I N P O I N T S . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

#include "render/internal/inpoints.hpp"
#include "render/internal/vtxdata.hpp"

RenIPoints::RenIPoints(size_t initial):
	ctl_min_memory_vector<RenIVertex>(initial)
{
}

/* End INPOINTS.CPP *************************************************/
