/*
 * J E T R I N G M . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions
#include "mathex/point2d.hpp"

#include "machphys/jetringm.hpp"
#include "machphys/private/otherper.hpp"
#include "machphys/jetringp.hpp"

#include "mathex/vec3.hpp"
#include "mathex/transf3d.hpp"
#include "phys/rampacce.hpp"
#include "render/scale.hpp"
#include "world4d/entity.hpp"
#include "world4d/entyplan.hpp"
#include "world4d/visplan.hpp"
#include "world4d/link.hpp"

#include "system/pathname.hpp"

PER_DEFINE_PERSISTENT( MachPhysJetRing );

//One-time ctor
MachPhysJetRing::MachPhysJetRing()
:   W4dComposite( MachPhysOtherPersistence::instance().pRoot(), MexTransform3d(), W4dEntity::SOLID ),
	pTorch_( NULL )
{
	//Load the model
    readCompositeFile( SysPathName( "models/construc/torch/torch.cdf" ) );

	if( !findLink( "torch", &pTorch_ ) )
		pTorch_ = NULL;

	ASSERT( pTorch_ != NULL, " torch not found " );

    TEST_INVARIANT;
}

//public ctor
MachPhysJetRing::MachPhysJetRing( W4dEntity* pParent, const MexTransform3d& localTransform )
:   W4dComposite( exemplar(), pParent, localTransform ),
	pTorch_( NULL )
{
	visible( false );
	pTorch_ = links()[ exemplar().pTorch_->id() ];

    TEST_INVARIANT;
}

MachPhysJetRing::~MachPhysJetRing()
{
    TEST_INVARIANT;

}

//static
const MachPhysJetRing& MachPhysJetRing::exemplar()
{
    return MachPhysOtherPersistence::instance().jetRingExemplar();
}

void MachPhysJetRing::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachPhysJetRing& t )
{

    o << "MachPhysJetRing " << (void*)&t << " start" << std::endl;
    o << "MachPhysJetRing " << (void*)&t << " end" << std::endl;

    return o;
}

//virtual
bool MachPhysJetRing::intersectsLine( const MexLine3d&, MATHEX_SCALAR*, Accuracy ) const
{
    return false;
}

MachPhysJetRing::MachPhysJetRing( PerConstructor con )
: W4dComposite( con )
{
}

void perWrite( PerOstream& ostr, const MachPhysJetRing& jetRing )
{
    const W4dComposite& base = jetRing;

    ostr << base;
	ostr << jetRing.pTorch_;
}

void perRead( PerIstream& istr, MachPhysJetRing& jetRing )
{
    W4dComposite& base = jetRing;

    istr >> base;
	istr >> jetRing.pTorch_;
}

void MachPhysJetRing::startGlow( const PhysAbsoluteTime& startTime, const MexPoint3d& targetOffsetGlobal )
{

    W4dVisibilityPlanPtr visibilityPlanPtr( _NEW( W4dVisibilityPlan( true ) ) );
	visibilityPlanPtr->add(false, 360000);

    entityPlanForEdit().visibilityPlan( visibilityPlanPtr, startTime, 0, MachPhys::CONSTRUCTION_CONSTRUCTING );

	//scale plan
	MexVec3 aVec(pTorch_->globalTransform().position(), targetOffsetGlobal );
	aVec.z( 0 );

	MATHEX_SCALAR scale = aVec.modulus()/5.0;

	pTorch_->temporaryScale( RenNonUniformScale(scale, 1, 1), W4dEntity::NOT_PROPOGATE );

	PhysMotionPlanPtr torchPlanPtr( _NEW( MachPhysJetRingPlan( this, targetOffsetGlobal, 200.0 ) ) );
    pTorch_->entityPlanForEdit().absoluteMotion( torchPlanPtr, startTime, 10000, MachPhys::CONSTRUCTION_CONSTRUCTING );

}
/* End JETRINGM.CPP *************************************************/

