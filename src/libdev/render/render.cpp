#include "render/render.hpp"

#include "system/pathname.hpp"
#include "render/device.hpp"
#include "render/texmgr.hpp"
#include "render/uvtrans.hpp"
#include "render/uvanim.hpp"
#include "render/scale.hpp"
#include "render/ttfpoly.hpp"
#include "render/ttftris.hpp"
#include "render/internal/trigroup.hpp"
#include "render/internal/matmgr.hpp"
#include "render/internal/meshload.hpp"
#include "render/internal/meshfact.hpp"
#include "mathex/coordsys.hpp"


// static
static bool renInitialised = false;


// static
ostream& Ren::out()
{
	PRE(RenDevice::current());
	return RenDevice::current()->out();
}

// Impose a construction order on various Singletons in the render
// library.  The real purpose is to impose a destruction order on the
// same objects.  In order of destruction, we need: meshes (and hence
// all materials and all textures); mesh factories; material manager;
// texture manager; RenDisplay; D3D drivers.
void Ren::initialise()
{
	PRE(MexCoordSystem::instance().isSet());
//    ASSERT( sizeof( Ren::VertexIdx ) == sizeof( WORD ), "Ren::VertexIdx is the wrong size" );

	renInitialised = true;

	RenTexManager::instance();
	RenIMatManager::instance();
	RenID3DMeshLoader::instance();
	RenIMeshFactory::instance();

    //  Register all of the derived classes we wish to read
    //  via base class pointers

    PER_REGISTER_DERIVED_CLASS( RenIDistinctGroup );
    PER_REGISTER_DERIVED_CLASS( RenILineGroup );
    PER_REGISTER_DERIVED_CLASS( RenUVUnityTransform );
    PER_REGISTER_DERIVED_CLASS( RenUVTranslation );
    PER_REGISTER_DERIVED_CLASS( RenUnityScale );
    PER_REGISTER_DERIVED_CLASS( RenUniformScale );
    PER_REGISTER_DERIVED_CLASS( RenNonUniformScale );
    PER_REGISTER_DERIVED_CLASS( RenTTFRectangle );
    PER_REGISTER_DERIVED_CLASS( RenTTFTriangles );
}

bool Ren::initialised()
{
	return renInitialised;
}

ostream& operator<<(ostream& o, const Ren::Rect& r)
{
	o << "rect: (" << r.originX << "," << r.originY << ") " << r.width << "x" << r.height;
	return o;
}
