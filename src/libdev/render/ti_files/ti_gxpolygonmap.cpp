#define _INSTANTIATE_TEMPLATE_CLASSES

#include "ctl/map.hpp"
#include "ctl/vector.hpp"
#include "gxin/gxmat.hpp"

typedef ctl_vector<int> PolygonIndexes;
dummyFunction_gxpolygonmap()
{
  static PolygonIndexes dummypolygonindexes;
  static ctl_map<GXMat, PolygonIndexes, less<GXMat> > dummygxpolygonmap;
}
