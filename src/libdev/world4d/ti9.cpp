#include "ctl/vector.hpp"
#include "ctl/vector.ctp"

#include "ctl/pvector.hpp"

#include "mathex/quatern.hpp"
#include "render/material.hpp"
#include "render/texture.hpp"
#include "world4d/linvplan.hpp"
#include <string.hpp>
#include "internal/animdata.hpp"
#include "internal/texdata.hpp"

class W4dGeneric;
class W4dGenericComposite;
class W4dCycleTextureData;
class W4dColourPulseData;

W4dDummyFunction9()
{
	// These are used by W4dComposite.
	static ctl_vector<pair<size_t, MexQuaternion> >	dummy1;
	static ctl_vector<pair<size_t, MexVec3> >				dummy2;
    
    static  ctl_vector< RenMaterial >     dummy3;

    static  W4dLinearFloatValuePlan         dummy4( 0, 1, 10 );

	static ctl_vector<pair< string, W4dGeneric* > >		dummy5;
	static ctl_vector<pair<string, W4dGenericComposite*> >	dummy6;
	static ctl_pvector< W4dAnimationData > dummy7;
    static ctl_vector< RenTexture >   dummy8;
    static ctl_vector< ctl_vector< RenTexture > >   dummy9;
    static ctl_pvector< W4dCycleTextureData >   dummy10;
    static ctl_pvector< W4dColourPulseData >   dummy11;
}
