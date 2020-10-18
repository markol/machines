#include "ctl/countptr.hpp"
#include "ctl/countptr.ctp"
#include "ctl/vector.hpp"
#include "ctl/vector.ctp"

// The files marked "accidental" shouldn't be necessary.  They must be included
// because Watcom is trying to instantiate everything it sees.
#include "mathex/transf3d.hpp" 		// accidental
#include "phys/rampacce.hpp"		// accidental

#include "phys/motplan.hpp"
#include "render/matvec.hpp"
#include "world4d/objtrack.hpp"
#include "internal/animdata.hpp"
#include "world4d/colodata.hpp"

W4dDummyFunction93()
{
    static CtlCountedPtr< W4dColourPlanData > colourPlanDataDummy;
    static  CtlCountedPtr< W4dObjectTracker >    dummy1;

    typedef CtlCountedPtr< RenMaterialVec > TMaterialVecPtr;
    typedef ctl_vector< TMaterialVecPtr > TMaterialVecPtrs;
    typedef CtlCountedPtr< TMaterialVecPtrs > TMaterialVecPtrsPtr;

    static CtlCountedPtr< TMaterialVecPtrs > dummy2;
    static ctl_vector< TMaterialVecPtrs > dummy3;

    static TMaterialVecPtrsPtr dummy4;
    static ctl_vector< TMaterialVecPtrsPtr > dummy5;

	typedef	CtlCountedPtr< W4dAnimationData > TAnimationDataPtr;
	typedef ctl_vector< TAnimationDataPtr > TAnimationDataPtrs;

	static 	TAnimationDataPtr dummy6;
	static 	TAnimationDataPtrs dummy7;
}
