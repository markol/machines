#define _INSTANTIATE_TEMPLATE_CLASSES
#include "ctl/vector.hpp"
#include <string.hpp>

void dummyTIObjectDataPop()
{
	static ctl_vector< string >		dummyCtlVectorString;
}