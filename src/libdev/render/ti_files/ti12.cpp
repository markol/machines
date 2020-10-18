#define _INSTANTIATE_TEMPLATE_CLASSES

#include "stdlib/memory.hpp"
#include "render/matvec.hpp"
#include "render/material.hpp"

dummyFunction12()
{
	static ctl_min_memory_vector<RenMaterial> vec;
	static auto_ptr<RenMaterialVec> ptr;
}