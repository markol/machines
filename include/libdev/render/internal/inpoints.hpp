/*
 * I N P O I N T S . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

#ifndef _RENDER_INPOINTS_HPP
#define _RENDER_INPOINTS_HPP

#include "base/base.hpp"
#include "ctl/mmvector.hpp"
class RenIVertex;

class RenIPoints : public ctl_min_memory_vector<RenIVertex>
{
public:
	RenIPoints(size_t initial);
};

#endif

/* End INPOINTS.HPP *************************************************/
