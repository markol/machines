#define _INSTANTIATE_TEMPLATE_CLASSES
#include "ctl/map.hpp"
#include "gxin/gxpolyvert3.hpp"
#include "render/render.hpp"

typedef ctl_map<GXPolyVert3,Ren::VertexIdx,less<GXPolyVert3> > IndexesMap;

dummyFunction_gxindexmap()
{
  static IndexesMap dummygxindexesmap;
}
