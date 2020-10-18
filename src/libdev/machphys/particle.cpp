/*
 * V O R T B O M B . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "mathex/point2d.hpp"
#include "machphys/particle.hpp"
#include "machphys/random.hpp"
#include "machphys/private/otherper.hpp"

#include "world4d/root.hpp"
#include "world4d/entity.hpp"
#include "world4d/entyplan.hpp"
#include "world4d/visplan.hpp"
#include "world4d/gusplan.hpp"

#include "render/meshinst.hpp"
#include "render/mesh.hpp"
#include "render/spinpoly.hpp"
#include "render/colour.hpp"
#include "render/texture.hpp"
#include "render/texmgr.hpp"
#include "render/device.hpp"

#include "mathex/transf3d.hpp"
#include "mathex/double.hpp"

#include "system/pathname.hpp"

#include "ctl/vector.hpp"

#include "phys/rampacce.hpp"
#include "phys/asclplan.hpp"

PER_DEFINE_PERSISTENT( MachPhysParticles );

//One-time ctor
MachPhysParticles::MachPhysParticles()
:W4dEntity( MachPhysOtherPersistence::instance().pRoot(), MexTransform3d(), W4dEntity::NOT_SOLID )
{

    Ren::MeshPtr particleMeshPtr = RenMesh::createEmpty();

	createSTFParticle(particleMeshPtr);

     //Add a mesh instance for it, with the fog distance as the visibility range
    RenMeshInstance* pMeshInstance = _NEW( RenMeshInstance( particleMeshPtr ) );
    add( pMeshInstance, _STATIC_CAST( MexDouble, RenDevice::current()->fogEnd() ), W4dLOD( 0 ) );


    TEST_INVARIANT;
}


//public ctor
MachPhysParticles::MachPhysParticles(W4dEntity* pParent, const MexTransform3d& localTransform)
:   W4dEntity( exemplar(), pParent, localTransform )
{
    //make invisible until required
    visible( false );

    TEST_INVARIANT;
}

MachPhysParticles::~MachPhysParticles()
{
    TEST_INVARIANT;

}

//static
const MachPhysParticles& MachPhysParticles::exemplar()
{
    return MachPhysOtherPersistence::instance().particlesExemplar();
}



void MachPhysParticles::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachPhysParticles& t )
{

    o << "MachPhysParticles " << (void*)&t << " start" << std::endl;
    o << "MachPhysParticles " << (void*)&t << " end" << std::endl;

    return o;
}

//virtual
bool MachPhysParticles::intersectsLine( const MexLine3d&, MATHEX_SCALAR*, Accuracy ) const
{
    return false;
}

void MachPhysParticles::createSTFParticle(const Ren::MeshPtr& mesh)
{
	// One long thin spin to face.
	RenSpinTFPolygon stf;

	RenSpinTFPolygon::Vertices verts;
	verts.push_back(MexPoint2d( 0.75,  0.75));
	verts.push_back(MexPoint2d(-0.75,  0.75));
	verts.push_back(MexPoint2d(-0.75, -0.75));
	verts.push_back(MexPoint2d( 0.75, -0.75));
	stf.vertices(verts);

	RenSpinTFPolygon::Vertices uvs;
	uvs.push_back(MexPoint2d(1, 1));
	uvs.push_back(MexPoint2d(0, 1));
	uvs.push_back(MexPoint2d(0, 0));
	uvs.push_back(MexPoint2d(1, 0));
	stf.uv(uvs);

	RenMaterial mat(RenColour::black());
	mat.emissive(RenColour::black());
	RenTexture newTex = RenTexManager::instance().createTexture("ptop_bt.bmp");
	mat.texture(newTex);

	stf.material(mat);
	stf.spinAxis(MexPoint3d(0, 0, 0));
	mesh->addSpinTFPolygon(stf);
}

void perWrite( PerOstream& ostr, const MachPhysParticles& particles )
{
    const W4dEntity& base = particles;

    ostr << base;
}

void perRead( PerIstream& istr, MachPhysParticles& particles )
{
    W4dEntity& base = particles;

    istr >> base;
}

MachPhysParticles::MachPhysParticles( PerConstructor c )
:W4dEntity( c )
{
}

/* End VORTBOMB.CPP *************************************************/

