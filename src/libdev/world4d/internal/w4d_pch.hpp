// This contains everything that we want to include in a pre-compiled
// header file for this library, i.e., header files that are used
// universally.  The PCH mechanism appears to clash with templates, so
// there is no ctl stuff here.

#include "mathex/transf3d.hpp"
#include "mathex/point3d.hpp"
#include "world4d/entity.hpp"
