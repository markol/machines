/*
 * F L A S H . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machphys/flash.hpp"

#include "stdlib/string.hpp"

#include "machphys/private/otherper.hpp"

#include "mathex/double.hpp"
#include "render/texture.hpp"
#include "render/texmgr.hpp"
#include "render/mesh.hpp"
#include "render/meshinst.hpp"
#include "render/ttfpoly.hpp"
#include "render/colour.hpp"
#include "render/device.hpp"
#include "render/material.hpp"

#include "world4d/root.hpp"
#include "world4d/entity.hpp"
#include "world4d/entyplan.hpp"
#include "world4d/visplan.hpp"
#include "world4d/gusplan.hpp"

#include "phys/lsclplan.hpp"

PER_DEFINE_PERSISTENT( MachPhysFlashDisc );

//public ctor
MachPhysFlashDisc::MachPhysFlashDisc(W4dEntity* pParent, const MexTransform3d& localTransform, const MATHEX_SCALAR& size, ColourType colour)
:W4dEntity( exemplar(size, colour), pParent, localTransform )
{
	// The current model is all emissive or black.  Hence, it should not need
	// lighting.  This could change if the model changes.
	doNotLight(true);

    //make invisible until required
    visible( false );

    TEST_INVARIANT;
}

//one time ctor
MachPhysFlashDisc::MachPhysFlashDisc( const MATHEX_SCALAR& size, ColourType colour )
:W4dEntity(MachPhysOtherPersistence::instance().pRoot(), MexTransform3d(), W4dEntity::NOT_SOLID)
{
	// The current model is all emissive or black.  Hence, it should not need
	// lighting.  This could change if the model changes.
	doNotLight(true);

    Ren::MeshPtr meshPtr = RenMesh::createEmpty();

	RenColour matColour;
	RenColour emissiveColour;
	static const RenTexture tex = RenTexManager::instance().createTexture("flar2_bt.bmp");

	switch(colour)
	{
		case MachPhysFlashDisc::YELLOW:
			matColour = RenColour(1.0, 1.0, 0.0, 0.7);
			emissiveColour = RenColour::yellow();
			break;
		case MachPhysFlashDisc::WHITE:
			matColour = RenColour(1.0, 1.0, 1.0, 0.7);
			emissiveColour = RenColour::white();
			break;
	}

	RenTTFRectangle ttf;
	ttf.width(size);
	ttf.height(size);
	ttf.depthOffset(-2.0);
	RenMaterial mat(matColour);
		mat.makeNonSharable();

	mat.texture(tex);
	mat.emissive(emissiveColour);
	ttf.material(mat);

	meshPtr->addTTFPolygon(ttf);
     //Add a mesh instance for it, with the fog distance as the visibility range
    RenMeshInstance* pMeshInstance = _NEW( RenMeshInstance( meshPtr ) );
    add( pMeshInstance, MexDouble( _STATIC_CAST( double, RenDevice::current()->fogEnd() ) ), W4dLOD( 0 ) );

    TEST_INVARIANT;
}

//static
const MachPhysFlashDisc& MachPhysFlashDisc::exemplar(const MATHEX_SCALAR& size, ColourType colour )
{
	//the size was defined in parmdata for the two types. so canot be altered any other way
    return MachPhysOtherPersistence::instance().flashDiscExemplar( size, colour);
}

MachPhysFlashDisc::MachPhysFlashDisc( PerConstructor con )
: W4dEntity( con )
{
}

MachPhysFlashDisc::~MachPhysFlashDisc()
{
    TEST_INVARIANT;

}

void MachPhysFlashDisc::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachPhysFlashDisc& t )
{

    o << "MachPhysFlashDisc " << (void*)&t << " start" << std::endl;
    o << "MachPhysFlashDisc " << (void*)&t << " end" << std::endl;

    return o;
}

//virtual
bool MachPhysFlashDisc::intersectsLine( const MexLine3d&, MATHEX_SCALAR*, Accuracy ) const
{
    return false;
}

PhysRelativeTime MachPhysFlashDisc::flash(const PhysAbsoluteTime& startTime , ColourType colour)
{
	PhysRelativeTime duration = 0.25;
    W4dEntityPlan& entityPlan = entityPlanForEdit();

	//scale plan
    PhysLinearScalarPlan::ScalarVec times;
    	times.reserve(1);
		times.push_back(duration);

    PhysLinearScalarPlan::ScalarVec scales;
    	scales.reserve(2);
        scales.push_back(1);
        scales.push_back(0.05);

   if(colour == MachPhysFlashDisc::YELLOW)
   {
		times.push_back(2*duration);
		times.push_back(3*duration);
		times.push_back(4*duration);

        scales.push_back(1);
        scales.push_back(0.05);
        scales.push_back(1);

        duration *= 4.0;
	}

	PhysScalarPlanPtr flashPlanPtr( _NEW( PhysLinearScalarPlan(times, scales) ) );

	W4dScalePlanPtr flashScalePlanPtr( _NEW( W4dGeneralUniformScalePlan(flashPlanPtr) ) );
    entityPlan.scalePlan( flashScalePlanPtr, startTime );

	//visibility plan
    W4dVisibilityPlanPtr flashVisibilityPlanPtr( _NEW( W4dVisibilityPlan( true ) ) );
	flashVisibilityPlanPtr->add(false, duration);

    entityPlan.visibilityPlan( flashVisibilityPlanPtr, startTime );

	return duration;
}

void perWrite( PerOstream& ostr, const MachPhysFlashDisc& flashDisc )
{
    const W4dEntity& base = flashDisc;

    ostr << base;
}

void perRead( PerIstream& istr, MachPhysFlashDisc& flashDisc )
{
    W4dEntity& base = flashDisc;

    istr >> base;
}

/* End FLASH.CPP *************************************************/

