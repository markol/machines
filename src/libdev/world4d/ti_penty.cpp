#include "ctl/countptr.hpp"
#include "ctl/countptr.ctp"

// The files marked "accidental" shouldn't be necessary.  They must be included
// because Watcom is trying to instantiate everything it sees.
#include "ctl/vector.hpp" 			// accidental
#include "mathex/transf3d.hpp" 		// accidental
#include "phys/rampacce.hpp"		// accidental
#include "phys/motplan.hpp"			// accidental
#include "render/mesh.hpp"			// accidental
#include "render/matvec.hpp"		// accidental
#include "render/uvanim.hpp"		// accidental

#include "world4d/compplan.hpp"		// accidental
#include "world4d/valplan.hpp"		// accidental
#include "world4d/visplan.hpp"		// accidental
#include "world4d/matplan.hpp"		// accidental
#include "world4d/uvplan.hpp"		// accidental
#include "world4d/scalplan.hpp"		// accidental
#include "world4d/meshplan.hpp"		// accidental

#include "world4d/entity.hpp"

W4dDummyFunctionEntityPtrs()
{
	static CtlCountedPtr<W4dEntity>			ptr;			// subject.hpp
	static CtlConstCountedPtr<W4dEntity>	cPtr;			// subject.hpp
}
