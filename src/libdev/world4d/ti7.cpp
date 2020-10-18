#include "ctl/vector.hpp"
#include "ctl/vector.ctp"

#include "world4d/entity.hpp"
#include "internal/entityi.hpp"
#include "world4d/visplan.hpp"
#include "render/texture.hpp"

class W4dCompositePlan;

void W4dDummyFunction7()
{
    static ctl_vector<W4dEntityImpl::MeshData>	dummyData;		// entity.hpp
    static ctl_vector< CtlCountedPtr< W4dCompositePlan > >  dummy3;
    static ctl_vector <RenTexture >	dummy4;		// entity.hpp
}
