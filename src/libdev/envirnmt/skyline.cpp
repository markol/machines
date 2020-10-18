/*
 * S K Y L I N E . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

#include "envirnmt/skyline.hpp"
#include "envirnmt/elevclut.hpp"
#include "envirnmt/elevmatp.hpp"
#include "world4d/generic.hpp"
#include "world4d/entyplan.hpp"
#include "world4d/manager.hpp"
#include "world4d/alphsimp.hpp"
#include "render/meshinst.hpp"
#include "render/mesh.hpp"
#include "phys/lsclplan.hpp"
#include "system/pathname.hpp"
#include "ctl/vector.hpp"

EnvSkyline::EnvSkyline
(
	W4dEntity* parent,
	const SysPathName& meshFile
):
	entity_(_NEW(W4dGeneric(parent, MexTransform3d()))),
	matTable_(NULL)
{
	PRE(parent);
	PRE(meshFile.existsAsFile());

	entity_->loadSingleMesh(meshFile, "skyline");
    TEST_INVARIANT;
}

EnvSkyline::~EnvSkyline()
{
    TEST_INVARIANT;
	_DELETE(matTable_);
}

void EnvSkyline::colourTable
(
	EnvElevationColourTable* clut,
	const EnvSatellite* controller
)
{
	PRE(clut);
	PRE(controller);
	PRE(entity_);

	const W4dGeneric* constEntity = entity_;
	matTable_ = clut;
	std::auto_ptr<RenMaterialVec> mats = constEntity->mesh().mesh()->materialVec();
	plan_ = _NEW(EnvElevationMaterialPlan(*(mats.get()), *controller, *matTable_));
	entity_->entityPlanForEdit().materialPlan(plan_, W4dManager::instance().time());
}

// Temporarily change the skyline to some fixed colour.
void EnvSkyline::overrideColour(const RenColour& col)
{
	W4dEntityPlan& ePlan = entity_->entityPlanForEdit();

	// Remove any existing plans, there should only be one.
    ePlan.clearMaterialPlans(ePlan.nMaterialPlans());

	// This would appear to be the easiest way to make a material plan which
	// applies an unchanging material to a mesh.  Unforunately, we need a
	// dummy linear scale plan.
    PhysLinearScalarPlan::ScalarVec times;
   	times.reserve(1);
    times.push_back(HUGE_VAL); 		// last forever

    PhysLinearScalarPlan::ScalarVec scales;
   	scales.reserve(2);
    scales.push_back(1);
    scales.push_back(1);

    PhysScalarPlanPtr lineScalarPlanPtr = _NEW( PhysLinearScalarPlan(times, scales) );

	const W4dGeneric* constEntity = entity_;
	std::auto_ptr<RenMaterialVec> mats = constEntity->mesh().mesh()->materialVec();
	ASSERT(mats->size() == 1, "Skyline can have only one material.");

	RenMaterial mat = mats->operator[](0);
	mat.diffuse(RenColour::black());
	mat.emissive(col);

	IAIN_STREAM("Original skyline " << mat << "\n");
	IAIN_STREAM("Skyline colour " << col << "\n");
	IAIN_STREAM("Skyline " << mat << "\n");
	IAIN_STREAM("Skyline material emissive=" << mat.emissive() << "\n");

    W4dMaterialPlanPtr matPlan = _NEW(W4dSimpleAlphaPlan(mat, 1, lineScalarPlanPtr, 1));
    ePlan.materialPlan(matPlan, W4dManager::instance().time());
}

// Put the colour back to normal.
void EnvSkyline::resetColour()
{
	W4dEntityPlan& ePlan = entity_->entityPlanForEdit();

	// Remove any existing plans, there should only be one.
    ePlan.clearMaterialPlans(ePlan.nMaterialPlans());

	// Revert to the original plan.
	ePlan.materialPlan(plan_, W4dManager::instance().time());
}

void EnvSkyline::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

/* End SKYLINE.CPP *************************************************/
