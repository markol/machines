#include "ctl/vector.hpp"
#include "ctl/vector.ctp"
 
#include "ctl/list.hpp"
#include "ctl/list.ctp"
 
#include "mathex/transf3d.hpp"
#include "mathex/point3d.hpp"

#include "world4d/entity.hpp"

class W4dEntity;

W4dDummyFunction1()
{
     static ctl_vector<MexPoint3d> dummyPoints;
     ctl_vector< pair<W4dEntity*, MexTransform3d> > dummyStoredLinkPositions;
}
