#include "ctl/vector.hpp"
#include "ctl/vector.ctp"
#include "render/surface.hpp"

dummyFunction_anim_curs()
{
	// Required by RenIAnimCursor2d.
	static ctl_vector<RenSurface> vec;
}