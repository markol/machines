#include "ctl/countptr.hpp"
#include "ctl/countptr.ctp"
#include "internal/intmap.hpp"
#include "internal/vtxmat.hpp"
#include "render/mesh.hpp"
#include "render/uvanim.hpp"
#include "render/matvec.hpp"

void renDummyFunctionVertexMatCPtr()
{
    static CtlCountedPtr<RenIVertexIntensities> dummy1;
    static CtlCountedPtr<RenIVertexMaterials>	dummy2;
}