/*
 * D A M A G E . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions
#include "machphys/damage.hpp"
#include "machphys/internal/damagei.hpp"
#include "render/texture.hpp"
#include "machphys/effects.hpp"
#include "machphys/random.hpp"
#include "machphys/machphys.hpp"
#include "machphys/stflames.hpp"
#include "machphys/smokplum.hpp"
#include "machphys/splat.hpp"
#include "machphys/constron.hpp"

#include "mathex/vec3.hpp"
#include "mathex/eulerang.hpp"
#include "mathex/degrees.hpp"
#include "mathex/line3d.hpp"
#include "mathex/transf3d.hpp"
#include "mathex/point3d.hpp"

#include "world4d/entity.hpp"
#include "world4d/composit.hpp"
#include "world4d/garbcoll.hpp"

#include "sim/manager.hpp"

#include "render/render.hpp"
#include "render/meshinst.hpp"
#include "render/mesh.hpp"
#include "render/vertex.hpp"
#include "render/scale.hpp"
#include "render/colour.hpp"

#include "ctl/vector.hpp"

PER_DEFINE_PERSISTENT( MachPhysEntityDamage );

MachPhysEntityDamage::MachPhysEntityDamage( W4dEntity* pTarget):
pImpl_( _NEW( MachPhysEntityDamageImpl( pTarget ) ) )
{
	update();

    TEST_INVARIANT;
}

MachPhysEntityDamage::MachPhysEntityDamage( MachPhysConstruction* pTarget):
pImpl_( _NEW( MachPhysEntityDamageImpl( pTarget ) ) )
{
	update();

    TEST_INVARIANT;
}

MachPhysEntityDamage::MachPhysEntityDamage( PerConstructor )
{
}

MachPhysEntityDamage::~MachPhysEntityDamage()
{
	_DELETE( pImpl_ );
    TEST_INVARIANT;

}

void MachPhysEntityDamage::CLASS_INVARIANT
{
	INVARIANT( pImpl_ != NULL );
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachPhysEntityDamage& t )
{

    o << "MachPhysEntityDamage " << (void*)&t << " start" << std::endl;
    o << "MachPhysEntityDamage " << (void*)&t << " end" << std::endl;

    return o;
}

void MachPhysEntityDamage::update()
{
	CB_DEPIMPL( W4dEntity*, pTarget_ );
	CB_DEPIMPL( MachPhysConstruction*, ptargetConstruction_);
	CB_DEPIMPL( MachPhysGroundSplat*, pSplat_);
	CB_DEPIMPL( MachPhysSmokePlume*, pSmoke1_);
	CB_DEPIMPL( MachPhysSmokePlume*, pSmoke2_);
	CB_DEPIMPL( double, damageLevel_);
	CB_DEPIMPL( PhysAbsoluteTime, lastSmoke1Time_);
	CB_DEPIMPL( PhysAbsoluteTime, lastSmoke2Time_);
	CB_DEPIMPL( PhysRelativeTime, smokeDuration_);
	CB_DEPIMPL( MexTransform3d, transform_);
	CB_DEPIMPL( MachPhysPuffType, smoke1Type_);
	CB_DEPIMPL( MachPhysPuffType, smoke2Type_);
	CB_DEPIMPL( MachPhysEntityDamageImpl::TargetType, targetType_);
	CB_DEPIMPL( MachPhysSTFFlame*, pFlame1_);
	CB_DEPIMPL( MachPhysSTFFlame*, pFlame2_);
	CB_DEPIMPL( MachPhysSTFFlame*, pFlame3_);
	CB_DEPIMPL( MexAlignedBox3d,  targetVolume_);
	CB_DEPIMPL( ctl_vector<MexTransform3d>, flameTransforms_);
	CB_DEPIMPL( ctl_vector<MATHEX_SCALAR>, flameWidths_);
	CB_DEPIMPL( ctl_vector<MATHEX_SCALAR>, flameHeights_);

	PRE(pTarget_);

	PhysAbsoluteTime now = SimManager::instance().currentTime();

	if( (damageLevel_ < 40 or damaged() ) && pSmoke1_ &&  now > (lastSmoke1Time_ + smokeDuration_) )
	{
		_DELETE(pSmoke1_);
		pSmoke1_= NULL;
	}
	if(damageLevel_ < 50 && pFlame1_ )
	{
		_DELETE(pFlame1_ );
		pFlame1_ = NULL;
	}
	if( (damageLevel_ < 60 or damaged() ) && pSplat_ )
	{
		 _DELETE(pSplat_ );
		 pSplat_ = NULL;
	}
	if(damageLevel_ < 65 && pFlame3_ )
	{
		_DELETE(pFlame3_ );
		pFlame3_ = NULL;
	}
	if( (damageLevel_ < 75 or damaged() ) && pSmoke2_ && now > (lastSmoke2Time_ + smokeDuration_))
	{
		_DELETE(pSmoke2_);
		pSmoke2_ = NULL;
	}
	if(damageLevel_ < 85 && pFlame2_ )
	{
		_DELETE(pFlame2_ );
		pFlame2_ = NULL;
	}

	MATHEX_SCALAR sizeX = targetVolume_.xLength();
	MATHEX_SCALAR sizeY = targetVolume_.yLength();
	MATHEX_SCALAR sizeZ = targetVolume_.zLength();
	MexPoint3d centroid = targetVolume_.centroid();

	//do the following adjustment for thin, tall objects such as a beacon to make the damage effect look natural
	const MATHEX_SCALAR maxHeight = 2.0* std::max(sizeX, sizeY);
	if(sizeZ > maxHeight ) sizeZ = maxHeight;


	//The vertices of the building get progressively darker starting at 1% damage thru' to 100% damage. This damage is shown on or 		near to the point of impact of the weapon(s) causing the damage.
   	if( damageLevel_> 0 )
	  	darkening();

	if( damageLevel_ >= 40 )
	{
		if(pSmoke1_==NULL)
		{

		    uint nWisps = 5;
		    MATHEX_SCALAR maxHeight = sizeZ+4.0;

		    MATHEX_SCALAR wispSize = std::max(sizeX, sizeY);
						  wispSize *= 0.5;

		    MATHEX_SCALAR zDepthOffset = 0.0;
			MexTransform3d smokeXform = transform_;
			smokeXform.translate(MexPoint3d(centroid.x(), centroid.y(), sizeZ*0.4) );
			pSmoke1_ = _NEW( MachPhysSmokePlume(
				   						        pTarget_->pParent(), smokeXform.position(),
				   						        maxHeight,
				   						        zDepthOffset,
				   						        nWisps,
				   						        wispSize,
				   						        smoke1Type_,
				   						        smokeDuration_ ) );

	    	lastSmoke1Time_ = SimManager::instance().currentTime();
		    pSmoke1_->startSmokePlume( lastSmoke1Time_ );
		}

		if(now > (lastSmoke1Time_ + smokeDuration_) )
		{
	    	pSmoke1_->startSmokePlume( now );
			lastSmoke1Time_ = now;
		}
	}

	if( damageLevel_ >= 50 )
	{
		//flame1 start
		PhysRelativeTime duration = 60;
		if( pFlame1_ == NULL and flameWidths_[0]*flameHeights_[0] != 0)
			pFlame1_ = _NEW(MachPhysSTFFlame( pTarget_, flameTransforms_[0], FLAME_1, flameWidths_[0], flameHeights_[0], MexVec3(0,0,1), duration ) );

		if( pFlame1_ != NULL )
	        pFlame1_->startFlame( now );

	}

	if( damageLevel_ >= 60 )
	{
		//Dark 'ground splat' appears.
		if( pSplat_== NULL )
		{
			MexTransform3d splatXform = transform_;
			MexPoint3d splatPosition = splatXform.position();
			splatPosition.z( splatPosition.z() + 0.1 );
			splatXform.position( splatPosition );

			pSplat_ = _NEW( MachPhysGroundSplat( pTarget_->pParent(), splatXform ) );
			pSplat_->visible(true);
			const double scale = 1.3/10.0;
			pSplat_->temporaryScale( RenNonUniformScale(sizeX*scale, sizeY*scale, sizeZ*scale), W4dEntity::NOT_PROPOGATE );

		}
	}

	if( damageLevel_ >= 65 )
	{
		//Flame3 start
		PhysRelativeTime duration = 60;

		if( pFlame3_ == NULL and flameWidths_[2]*flameHeights_[2] != 0)
			pFlame3_ = _NEW(MachPhysSTFFlame( pTarget_, flameTransforms_[2], FLAME_1, flameWidths_[2], flameHeights_[2], MexVec3(0,0,1), duration ) );

        if( pFlame3_ != NULL ) pFlame3_->startFlame( now );
	}

	if( damageLevel_ >= 75 )
	{
		//Smoke # 2 initiated. Colour of smoke to be dark rust red.
		if(pSmoke2_==NULL)
		{

		    uint nWisps = 5;
		    MATHEX_SCALAR maxHeight = sizeZ+4.0;

		    MATHEX_SCALAR wispSize = std::max(sizeX, sizeY);
						  wispSize *= 0.5;

		    MATHEX_SCALAR zDepthOffset = 0.0;
			MexTransform3d smokeXform = transform_;
			smokeXform.translate(MexPoint3d(centroid.x(), centroid.y(), sizeZ*0.65) );
			pSmoke2_ = _NEW( MachPhysSmokePlume(
				   						        pTarget_->pParent(), smokeXform.position(),
				   						        maxHeight,
				   						        zDepthOffset,
				   						        nWisps,
				   						        wispSize,
				   						        smoke2Type_,
				   						        smokeDuration_ ) );

	    	lastSmoke2Time_ = SimManager::instance().currentTime();
		    pSmoke2_->startSmokePlume( lastSmoke2Time_- 5.0 );
		}

		if(now > (lastSmoke2Time_ + smokeDuration_) )
		{
	    	pSmoke2_->startSmokePlume( now-5.0 );
			lastSmoke2Time_ = now;
		}
	}

	if( damageLevel_ >= 85 )
	{
		//flame2 starts

		PhysRelativeTime duration = 60;
		if ( pFlame2_ == NULL and flameWidths_[1]*flameHeights_[1] != 0)
			pFlame2_ = _NEW(MachPhysSTFFlame( pTarget_, flameTransforms_[1], FLAME_1, flameWidths_[1], flameHeights_[1], MexVec3(0,0,1), duration ) );

		if( pFlame2_ != NULL ) pFlame2_->startFlame( now );
	}
}

void MachPhysEntityDamage::damageLevel( const double& percent )
{
	pImpl_->damageLevel_ = percent;
	update();
}

const double& MachPhysEntityDamage::damageLevel() const
{
	return pImpl_->damageLevel_;
}

void MachPhysEntityDamage::darkening()
{
	MATHEX_SCALAR darkness = 1.0 - pImpl_->damageLevel_/100.0;
		if(darkness < 0.01) darkness = 0.01;

	pImpl_->pTarget_->filterColour( RenColour(darkness) );
}

bool MachPhysEntityDamage::damaged() const
{
	return pImpl_->damageLevel_ > 100;
}
/*
void MachPhysEntityDamage::damageReferencePoint(const MexPoint3d& refPoint)
{
	refPoint_ = refPoint;
}

MexPoint3d MachPhysEntityDamage::referencePointOnEntity() const
{

	MexPoint3d refPointOnEntity;
	MexPoint3d refPointLocal = refPoint_;

	pLargestEntity_->globalTransform().transformInverse( &refPointLocal );

	Ren::ConstMeshPtr mesh = _CONST_CAST(const W4dEntity*, pLargestEntity_)->mesh().mesh();
	const uint vtxNo = mesh->nVertices();

	double distance = 100000000.0; //just a large number

	for(uint i=0; i<vtxNo; ++i)
	{
		Ren::VertexIdx idx = (Ren::VertexIdx)i;
		RenVertex v = mesh->vertex(idx);
		MexLine3d line( refPointLocal, v.position() );
		double lineLength = line.length();
		if(lineLength < distance)
		{
			distance = lineLength;
			refPointOnEntity = v.position();
		}
	}

	return refPointOnEntity;
}

*/
void perWrite( PerOstream& str, const MachPhysEntityDamage& t )
{
    str << t.pImpl_;
}

void perRead( PerIstream& str, MachPhysEntityDamage& t )
{
    str >> t.pImpl_;
}

void MachPhysEntityDamage::smoke1Type( const MachPhysPuffType& type1 )
{
	pImpl_->smoke1Type_ = type1;
}

const MachPhysPuffType& MachPhysEntityDamage::smoke1Type(void) const
{
	return pImpl_->smoke1Type_;
}

void MachPhysEntityDamage::smoke2Type( const MachPhysPuffType& type2 )
{
	pImpl_->smoke2Type_ = type2;
}

const MachPhysPuffType& MachPhysEntityDamage::smoke2Type(void) const
{
	return pImpl_->smoke2Type_;
}


/* End DAMAGE.CPP ***************************************************/

