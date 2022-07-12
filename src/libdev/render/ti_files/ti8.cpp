#ifndef _WIN32
    #define _WIN32
#endif

#include "stdlib/memory.hpp"
#include "stdlib/memory.ctp"

#include "ctl/mmvector.hpp"
#include "ctl/mmvector.ctp"
#include <d3d.h>

dummyFunction8()
{
	static auto_ptr_array< D3DTLVERTEX > dummy1;
	static ctl_min_memory_vector<D3DLVERTEX> vertices2;
}
