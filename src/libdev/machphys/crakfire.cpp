/*
 * C R A K F I R E . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions
#include "mathex/point2d.hpp"

#include "machphys/crakfire.hpp"
#include "machphys/private/otherper.hpp"
#include "machphys/internal/crakfiri.hpp"

#include "stdlib/string.hpp"
#include "ctl/vector.hpp"
#include "mathex/point3d.hpp"
#include "system/pathname.hpp"
#include "phys/lsclplan.hpp"

#include "render/matvec.hpp"
#include "render/scale.hpp"
#include "render/mesh.hpp"
#include "render/meshinst.hpp"
#include "render/material.hpp"

#include "world4d/alphsimp.hpp"
#include "world4d/entity.hpp"
#include "world4d/entyplan.hpp"
#include "world4d/visplan.hpp"
#include "world4d/simplsca.hpp"
#include "world4d/link.hpp"
#include "world4d/entity.hpp"
#include "world4d/compplan.hpp"
#include "world4d/coldata.hpp"
#include "world4d/colpulse.hpp"
#include "world4d/garbcoll.hpp"

#include "machphys/plansurf.hpp"
#include "machphys/punch.hpp"

#include "sim/manager.hpp"
PER_DEFINE_PERSISTENT( MachPhysCrackFire );

//One-time ctor
MachPhysCrackFire::MachPhysCrackFire( const ChasmType& type )
:   W4dComposite( MachPhysOtherPersistence::instance().pRoot(), MexTransform3d(), W4dEntity::SOLID ),
	pImpl_( _NEW( MachPhysCrackFireImpl( 0 ) ) )
{
	// The current model is all black with emissive flames and things.  Hence,
	// it should not need lighting.  This could change if the model changes.
	doNotLight(true);

	CB_DEPIMPL(W4dLink*, pCrack_);
	CB_DEPIMPL(W4dLink*, pFire_);
	CB_DEPIMPL(ctl_vector< W4dLink* >, debris_);

	//Load the model
	switch( type )
	{
		case MachPhysCrackFire::BEE_BOMB_CHASM:

		    readCompositeFile( SysPathName( "models/weapons/bee/chasm.cdf" ) );
			break;

		case MachPhysCrackFire::GARILLA_PUNCH_CHASM:
		default:

		    readCompositeFile( SysPathName( "models/weapons/punch/chasm.cdf" ) );
			break;
	}


    if(!findLink("Crack23", &pCrack_))
    	pCrack_ = NULL;
	ASSERT(pCrack_, "Can't find Crack23 mesh");

    if(!findLink("Fire23", &pFire_))
    	pFire_ = NULL;
	ASSERT(pFire_, "Can't find Fire23 mesh");

	const size_t nDebris = 15;
	debris_.reserve( nDebris );

	for( size_t i = 1; i<= nDebris; ++i )
	{
		char textN[256];
//		itoa(i, textN, 10 );
        sprintf(textN, "%d", i);
		string debrisLinkName;

		if( i < 10 )
			debrisLinkName = "debris0" + string(textN);
		else
			debrisLinkName = "debris" + string(textN);
		W4dLink* pLink;
 	    if( findLink(debrisLinkName, &pLink) )
		{
			debris_.push_back( pLink );
		}
	}

	setMaterialFogMultipliers();

    TEST_INVARIANT;
}

//public ctor1
MachPhysCrackFire::MachPhysCrackFire( W4dEntity* pParent, const MexTransform3d& localTransform, const MATHEX_SCALAR& length, const ChasmType& type)
:   W4dComposite( exemplar( type ), pParent, localTransform ),
	pImpl_( _NEW( MachPhysCrackFireImpl( length ) ) )
{
	// The current model is all black with emissive flames and things.  Hence,
	// it should not need lighting.  This could change if the model changes.
	doNotLight(true);

	CB_DEPIMPL(W4dLink*, pCrack_);
	CB_DEPIMPL(W4dLink*, pFire_);
	CB_DEPIMPL(ctl_vector< W4dLink* >, debris_);

	pCrack_ = links()[exemplar( type ).pImpl_->pCrack_->id()];
	pFire_ = links()[exemplar( type ).pImpl_->pFire_->id()];

	ASSERT(pCrack_, "Can't find Crack23 mesh");
	ASSERT(pFire_, "Can't find Fire23 mesh");

	pCrack_->visible( false );
	pFire_->visible( false );

	size_t nDebris = exemplar( type ).pImpl_->debris_.size();
	debris_.reserve( nDebris );

	for( size_t i = 0; i< nDebris; ++i )
	{
		W4dComposite::W4dLinks& exemplarLinks = exemplar( type ).pImpl_->debris_;
		W4dLink* pLink = links()[exemplarLinks[i]->id()];
		debris_.push_back( pLink );
	}

	for( int i = 0; i< nDebris; ++i )
	{
		ASSERT( debris_[i] != NULL, "NULL debris" )
		debris_[i]->visible( false );
	}

    TEST_INVARIANT;
}

//public ctor2
MachPhysCrackFire::MachPhysCrackFire( W4dEntity* pParent, const MexPoint3d& startPoint,
	                                   const MexPoint3d& endPoint, const ChasmType& type )
:   W4dComposite( exemplar( type ), pParent, MexTransform3d() ),
	pImpl_( _NEW( MachPhysCrackFireImpl() ) )
{
	CB_DEPIMPL(MATHEX_SCALAR, length_);

	// The current model is all black with emissive flames and things.  Hence,
	// it should not need lighting.  This could change if the model changes.
	doNotLight(true);

	MexPoint3d end1 = startPoint;
	MexPoint3d end2 = endPoint;

	MexVec3 xAxis(end1, end2);
	length_ = xAxis.modulus();

	xAxis.makeUnitVector();

	MexVec3 zAxis(0, 0, 1);

	MexVec3 yAxis = zAxis;
	yAxis.crossProduct(xAxis);
	yAxis.makeUnitVector();

    zAxis = xAxis;
    zAxis.crossProduct(yAxis);
	zAxis.makeUnitVector();

	MexTransform3d localXform(xAxis, yAxis, zAxis, end1);
	localTransform( localXform );

	CB_DEPIMPL(W4dLink*, pCrack_);
	CB_DEPIMPL(W4dLink*, pFire_);
	CB_DEPIMPL(ctl_vector< W4dLink* >, debris_);

	pCrack_ = links()[exemplar( type ).pImpl_->pCrack_->id()];
	pFire_ = links()[exemplar( type ).pImpl_->pFire_->id()];

	ASSERT(pCrack_, "Can't find Crack23 mesh");
	ASSERT(pFire_, "Can't find Fire23 mesh");

	pCrack_->visible( false );
	pFire_->visible( false );

	size_t nDebris = exemplar( type ).pImpl_->debris_.size();
	debris_.reserve( nDebris );

	for( size_t i = 0; i< nDebris; ++i )
	{
		W4dComposite::W4dLinks& exemplarLinks = exemplar( type ).pImpl_->debris_;
		W4dLink* pLink = links()[exemplarLinks[i]->id()];
		debris_.push_back( pLink );
	}

	for( int i = 0; i< nDebris; ++i )
	{
		ASSERT( debris_[i] != NULL, "NULL debris" )
		debris_[i]->visible( false );
	}


    TEST_INVARIANT;
}



MachPhysCrackFire::~MachPhysCrackFire()
{
    TEST_INVARIANT;
	_DELETE( pImpl_ );
}

//static
const MachPhysCrackFire& MachPhysCrackFire::exemplar( const ChasmType& type )
{
    return MachPhysOtherPersistence::instance().crackFireExemplar( type );
}

void MachPhysCrackFire::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachPhysCrackFire& t )
{

    o << "MachPhysCrackFire " << (void*)&t << " start" << std::endl;
    o << "MachPhysCrackFire " << (void*)&t << " end" << std::endl;

    return o;
}

//virtual
bool MachPhysCrackFire::intersectsLine( const MexLine3d&, MATHEX_SCALAR*, Accuracy ) const
{
    return false;
}

MachPhysCrackFire::MachPhysCrackFire( PerConstructor con )
: W4dComposite( con ),
  pImpl_( NULL )
{
}

void perWrite( PerOstream& ostr, const MachPhysCrackFire& crackFire )
{
    const W4dComposite& base = crackFire;

    ostr << base;
	ostr << crackFire.pImpl_;
}

void perRead( PerIstream& istr, MachPhysCrackFire& crackFire )
{
    W4dComposite& base = crackFire;

    istr >> base;
	_DELETE( crackFire.pImpl_ );
    istr >> crackFire.pImpl_;
}

void MachPhysCrackFire::startCrackAndFire
(
	const PhysAbsoluteTime& startTime,
	const PhysRelativeTime& duration,
	const MATHEX_SCALAR& cScale,
	const MATHEX_SCALAR& fScale,
	const ChasmType& type,
	DisplayDebris  debris
)
{
	CB_DEPIMPL(W4dLink*, pCrack_);
	CB_DEPIMPL(W4dLink*, pFire_);
	CB_DEPIMPL(MATHEX_SCALAR, length_);
	CB_DEPIMPL(ctl_vector< W4dLink* >, debris_);

	MATHEX_SCALAR xScale = length_/2.0;
	MATHEX_SCALAR frameRate = MachPhysPunchBlast::punchFrameRate();

	// crack visibility plan
	PhysRelativeTime crackDuration;
	PhysRelativeTime crackAlphaDelay;
	PhysRelativeTime fireDuration;
	PhysRelativeTime fireAlphaDelay;

	if( duration < 0 )
	{
		crackDuration = 35.0/frameRate;
		crackAlphaDelay = 10.0/frameRate;
		fireDuration = 20.0/frameRate;
		fireAlphaDelay = 10.0/frameRate;
	}
	else
	{
		crackDuration = duration;
		crackAlphaDelay = duration/3.0;
		fireDuration = duration*2.0/3.0;
		fireAlphaDelay = crackAlphaDelay;
	}

    W4dVisibilityPlanPtr crackVisibilityPlanPtr( _NEW( W4dVisibilityPlan( true ) ) );
	crackVisibilityPlanPtr->add(false, crackDuration);

	pCrack_->entityPlanForEdit().visibilityPlan(crackVisibilityPlanPtr, startTime );

	// crack scale plan
    RenNonUniformScale a(xScale, 1, 1);
    RenNonUniformScale b(xScale, cScale, 1); //7.5, 1);
	W4dScalePlanPtr planPtr;

	if( type == BEE_BOMB_CHASM )
	{
    	planPtr = _NEW( W4dSimpleNonUniformScalePlan( a, a, crackDuration ) );
	}
	else
	{
		a = RenNonUniformScale(xScale, 0.01, 1);
    	planPtr = _NEW( W4dSimpleNonUniformScalePlan( a, b, crackDuration ) );
    }

	pCrack_->propogateScalePlan( planPtr, startTime, 1);

    // crack alpha plan
	//RenMaterialVec* pCrackMaterialVec =_CONST_CAST(const W4dEntity*, pCrack_)->mesh().mesh()->materialVec().release();
	RenMaterialVec* pCrackMaterialVec =_CONST_CAST(const W4dEntity*, _STATIC_CAST( W4dEntity*, pCrack_) )
        ->mesh().mesh()->materialVec().release();
	RenMaterial& matCrack = (*pCrackMaterialVec)[0];
	const size_t nMatCrack = pCrackMaterialVec->size();

	ASSERT( nMatCrack > 0, " no materials " );

	if( nMatCrack > 0 )
	{
		PhysLinearScalarPlan::ScalarVec times;
	    	times.reserve(2);
			times.push_back(crackAlphaDelay);
			times.push_back(crackDuration);

		ASSERT( crackDuration > crackAlphaDelay , " timing error" );

	    PhysLinearScalarPlan::ScalarVec scales;
	    	scales.reserve(3);
			scales.push_back(1.0);
			scales.push_back(1.0);
			scales.push_back(0.0);

	    PhysScalarPlanPtr lineScalarPlanPtr = _NEW( PhysLinearScalarPlan(times, scales) );

	    W4dMaterialPlanPtr crackMaterialAlphaPlanPtr = _NEW( W4dSimpleAlphaPlan( matCrack, nMatCrack, lineScalarPlanPtr, 0 ) );
	    pCrack_->entityPlanForEdit().materialPlan( crackMaterialAlphaPlanPtr, startTime );
	}
	// fire visibility plan
    W4dVisibilityPlanPtr fireVisibilityPlanPtr( _NEW( W4dVisibilityPlan( true ) ) );
	fireVisibilityPlanPtr->add(false, fireDuration);

	pFire_->entityPlanForEdit().visibilityPlan(fireVisibilityPlanPtr, startTime );

	// fire scale plan
	a = RenNonUniformScale(xScale, 1.0, 0.1);

	if( type == BEE_BOMB_CHASM )
	{

	    b = RenNonUniformScale(xScale, 1.0, fScale); //9.0);
	    W4dScalePlanPtr fireScalePlanPtr1( _NEW( W4dSimpleNonUniformScalePlan( a, b, fireDuration ) ) );
	   	pFire_->propogateScalePlan( fireScalePlanPtr1, startTime, 0);

	}
	else
	{
	    b = RenNonUniformScale(xScale, 1.0, fScale); //9.0);
	    W4dScalePlanPtr fireScalePlanPtr1( _NEW( W4dSimpleNonUniformScalePlan( a, b, fireAlphaDelay ) ) );
	   	pFire_->propogateScalePlan( fireScalePlanPtr1, startTime, 0);

	    W4dScalePlanPtr fireScalePlanPtr2( _NEW( W4dSimpleNonUniformScalePlan( b, a, fireDuration-fireAlphaDelay ) ) );
	   	pFire_->propogateScalePlan( fireScalePlanPtr2, startTime + fireAlphaDelay, 0);
    }
    // fire alpha plan
	//RenMaterialVec* pFireMaterialVec =_CONST_CAST(const W4dEntity*, pFire_)->mesh().mesh()->materialVec().release();
	RenMaterialVec* pFireMaterialVec =_CONST_CAST(const W4dEntity*, _STATIC_CAST( W4dEntity*, pFire_) )
        ->mesh().mesh()->materialVec().release();

	RenMaterial& matFire = (*pFireMaterialVec)[0];
	const size_t nMatFire = pFireMaterialVec->size();

	ASSERT( nMatFire > 0, " no materials " );
		PhysLinearScalarPlan::ScalarVec fTimes;
	    	fTimes.reserve(2);
			fTimes.push_back(fireAlphaDelay);
			fTimes.push_back(fireDuration);

	    PhysLinearScalarPlan::ScalarVec aScales;
	    	aScales.reserve(3);
			aScales.push_back(1.0);
			aScales.push_back(1.0);
			aScales.push_back(0.0);

	if ( nMatFire > 0 )
	{
	    W4dMaterialPlanPtr fireMaterialAlphaPlanPtr = _NEW( W4dSimpleAlphaPlan( matFire, nMatFire, _NEW( PhysLinearScalarPlan(fTimes, aScales) ), 0 ) );
	    pFire_->entityPlanForEdit().materialPlan( fireMaterialAlphaPlanPtr, startTime );
	}

	//debris scale alpha plan
	if( debris == DEBRIS_DISPLAY )
	{
		const size_t nDebris = debris_.size();

		for( int i=0; i<nDebris; ++i)
		{
			ASSERT( debris_[i] != NULL, "no debris" );

			//RenMaterialVec* pDebrisMaterialVec =_CONST_CAST(const W4dEntity*, debris_[i])->mesh().mesh()->materialVec().release();
			RenMaterialVec* pDebrisMaterialVec =_CONST_CAST(const W4dEntity*, _STATIC_CAST(W4dEntity*, debris_[i]) )
                ->mesh().mesh()->materialVec().release();
			RenMaterial& matDebris = (*pDebrisMaterialVec)[0];
			const size_t nMat = pDebrisMaterialVec->size();
			ASSERT( nMat > 0, " no materials " );
			if( nMat > 0 )
			{
			    W4dMaterialPlanPtr debrisMaterialAlphaPlanPtr = _NEW( W4dSimpleAlphaPlan( matDebris, nMat, _NEW( PhysLinearScalarPlan(fTimes, aScales) ), 0 ) );
				debris_[i]->entityPlanForEdit().materialPlan( debrisMaterialAlphaPlanPtr, startTime );

				debris_[i]->entityPlanForEdit().visibilityPlan(fireVisibilityPlanPtr, startTime );
			}
		}

		//start the explosion animation
		W4dCompositePlanPtr explodePlanPtr;
		if( findCompositePlan( "explode", &explodePlanPtr ) )
		{

	        plan( *explodePlanPtr, startTime, 1000000 );
		}
	}
}

//static
void MachPhysCrackFire::createCrackFires
(
	W4dEntity* pParent,
	const MexTransform3d& position,
	const MachPhysCrackFire::Points& points,
	const PhysAbsoluteTime& startTime,
	const PhysRelativeTime& duration,
	const MachPhysPlanetSurface& surface,
	const MATHEX_SCALAR& crackScale,
	const MATHEX_SCALAR& fireScale,
	DisplayDebris  debris,
	MachPhysCrackFire::ChasmType type
)
{
	uint nSection = points.size() - 1;

	for(uint j = 0; j<nSection; ++j)
	{
		const MATHEX_SCALAR x1 = points[j].x();
		const MATHEX_SCALAR y1 = points[j].y();

		const MATHEX_SCALAR x2 = points[j+1].x();
		const MATHEX_SCALAR y2 = points[j+1].y();

		MexPoint3d startPoint( x1, y1, 0 );
		MexPoint3d   endPoint( x2, y2, 0 );

		pParent->globalTransform().transform(&startPoint);
		pParent->globalTransform().transform(&endPoint);


		const MATHEX_SCALAR z1 = surface.terrainHeight(startPoint.x(), startPoint.y());
		const MATHEX_SCALAR z2 = surface.terrainHeight(endPoint.x(), endPoint.y());

		startPoint.z( z1 );
		  endPoint.z( z2 );

		pParent->globalTransform().transformInverse(&startPoint);
		pParent->globalTransform().transformInverse(&endPoint);

		position.transform( &startPoint );
		position.transform( &endPoint );

		MachPhysCrackFire* pCrackFire = _NEW( MachPhysCrackFire(pParent, startPoint, endPoint, type) );
		pCrackFire->startCrackAndFire( startTime, duration, crackScale, fireScale, type, debris );
	    W4dGarbageCollector::instance().add( pCrackFire, startTime + duration );
    }
}

static void setFogMultipliers( const W4dEntity* entity )
{
	PRE( entity );

	if( entity->hasMesh() )
	{
		RenMesh* meshPtr( _CONST_CAST( RenMesh*, &( *entity->mesh().mesh() ) ) );

		std::auto_ptr<RenMaterialVec> materialASet = meshPtr->materialVec();
		RenMaterialVec& materialSet = *materialASet;
	    for( RenMaterialVec::iterator i = materialSet.begin(); i != materialSet.end(); ++i )
	    {
	        if( (*i).hasFogMultiplier() )
	        {
				(*i).fogMultiplier( 1.5 );
			}
	    }

		meshPtr->materialVec( &materialSet );

	}
}

void MachPhysCrackFire::setMaterialFogMultipliers()
{
	setFogMultipliers( this );
	for( W4dComposite::W4dLinks::const_iterator it = links().begin(); it != links().end(); ++it )
	{
		setFogMultipliers( (*it) );
	}
}

/* End CRAKFIRE.CPP *************************************************/

