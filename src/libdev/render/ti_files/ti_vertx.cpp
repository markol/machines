//#define _INSTANTIATE_TEMPLATE_CLASSES

#include "ctl/mmvector.hpp"
#include "ctl/mmvector.ctp"
#include "internal/vtxdata.hpp"

dummyFunctionVertexVector( void )
{
	static ctl_min_memory_vector<RenIVertex> vec1;
}