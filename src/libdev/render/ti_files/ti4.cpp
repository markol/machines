#define _INSTANTIATE_TEMPLATE_CLASSES

#include "ctl/mmvector.hpp"

class RenITriangleGroup;
class RenTTFPolygon;
class RenSpinTFPolygon;

dummyFunction4( void )
{
	static ctl_min_memory_vector<RenITriangleGroup*>	triangles;
	static ctl_min_memory_vector<RenTTFPolygon*>		ttfs;
	static ctl_min_memory_vector<RenSpinTFPolygon*>		stfs;
	static ctl_min_memory_vector<ushort>				shorts;
}