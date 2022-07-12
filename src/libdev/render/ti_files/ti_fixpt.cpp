#include "ctl/vector.hpp"
#include "ctl/vector.ctp"
#include "ctl/mmvector.hpp"
#include "ctl/mmvector.ctp"
#include "internal/fixptvec.hpp"

dummyFunctionFixedPoint()
{
	static ctl_vector<RenIColourFixPtS15_16>		vec1;
	static ctl_min_memory_vector<RenIVec3FixPtS0_7>	vec2;
}