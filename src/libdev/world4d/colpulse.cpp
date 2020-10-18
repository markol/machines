/*
 * C O L P U L S E . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "world4d/colpulse.hpp"

#include "stdlib/string.hpp"
#include "system/pathname.hpp"
#include "ctl/vector.hpp"

#include "render/mesh.hpp"
#include "render/meshinst.hpp"
#include "render/colour.hpp"
#include "render/matvec.hpp"
#include "render/material.hpp"
#include "render/texture.hpp"

#include "world4d/entity.hpp"
#include "world4d/entyplan.hpp"

W4dColourPulsePlan::W4dColourPulsePlan( const ColourPulseDataVec& colourPulseDataVec, const RenMaterialVec& matVec)
:   W4dMaterialPlan( 1000000, 0 ),
	colourPulseDataVec_( colourPulseDataVec ),
	materialVecPtr_( _NEW(RenMaterialVec( matVec.size() ) ) )
{
	size_t nMaterials = matVec.size();

	pulsingTexturePositions_.reserve( nMaterials );
	for(size_t i=0; i<nMaterials; ++i)
	{
		materialVecPtr_->push_back( matVec[i] );
		pulsingTexturePositions_.push_back(9999);
	}

	size_t nTextures = colourPulseDataVec_.size();
	for( size_t i=0; i< nTextures; ++i )
	{
		const RenTexture& keyTex = colourPulseDataVec_[i]->keyTexture();
		for(size_t j = 0; j < nMaterials; ++j)
		{
			RenTexture tex = (*materialVecPtr_)[j].texture();

			if( tex == keyTex )
				pulsingTexturePositions_[j] = i;
		}
	}

    TEST_INVARIANT;
}

W4dColourPulsePlan::W4dColourPulsePlan( const RenMaterialVec& matVec, const RenColour& fromColour, const RenColour& toColour, const PhysRelativeTime& duration )
:   W4dMaterialPlan( duration, 0 ),
	materialVecPtr_( _NEW(RenMaterialVec( matVec.size() ) ) )
{
	size_t nMaterials = matVec.size();

	pulsingTexturePositions_.reserve( nMaterials );
	for(size_t i=0; i<nMaterials; ++i)
	{
		materialVecPtr_->push_back( matVec[i] );
		pulsingTexturePositions_.push_back(0);
	}

	colourPulseDataVec_.reserve(1);
	W4dColourPulseData data(  "", fromColour, toColour, duration );
	colourPulseDataVec_.push_back( &data );

    TEST_INVARIANT;
}


W4dColourPulsePlan::~W4dColourPulsePlan()
{
    TEST_INVARIANT;

}

void W4dColourPulsePlan::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const W4dColourPulsePlan& t )
{

    o << "W4dColourPulsePlan " << (void*)&t << " start" << std::endl;
    o << "W4dColourPulsePlan " << (void*)&t << " end" << std::endl;

    return o;
}

//virtual
bool W4dColourPulsePlan::isLODDefined( W4dLOD lodId ) const
{
    return lodId <= maxLOD();
}

//virtual
const Ren::MaterialVecPtr& W4dColourPulsePlan::materialVec
(
    const PhysRelativeTime& timeOffset, W4dLOD lodId
) const
{
    PRE( isLODDefined( lodId ) );

	size_t nMaterials = materialVecPtr_->size();

	for(size_t i = 0; i<nMaterials; ++i)
	{
		size_t iData = pulsingTexturePositions_[i];
		if( iData != 9999)
		{
			RenMaterial& pulsingMaterial = (*_CONST_CAST(W4dColourPulsePlan*, this)->materialVecPtr_)[i];
						 pulsingMaterial.makeNonSharable();

			if( pulsingMaterial.emissive() != RenColour::black())
			{
				double alpha = pulsingMaterial.emissive().a();
				RenColour newColour = pulsingColour(timeOffset, iData);
				newColour.a( alpha );
				pulsingMaterial.emissive( newColour );
			}
			else
			{
				double alpha = pulsingMaterial.diffuse().a();
				RenColour newColour = pulsingColour(timeOffset, iData);
				newColour.a( alpha );
				pulsingMaterial.diffuse( newColour );
			}
		}
	}

    //Return the material vector
    return materialVecPtr_;
}

RenColour W4dColourPulsePlan::pulsingColour( const PhysRelativeTime timeOffset, size_t iDataSet ) const
{
	PRE( timeOffset >= 0 );

	PhysRelativeTime period_ = colourPulseDataVec_[iDataSet]->duration();
    RenColour fromColour_ = colourPulseDataVec_[iDataSet]->fromColour() ;
    RenColour toColour_ = colourPulseDataVec_[iDataSet]->toColour();

	PhysRelativeTime offset = timeOffset - int( timeOffset/period_) * period_;
	const PhysRelativeTime halfPeriod = 0.5*period_;
	offset -= halfPeriod;
	offset = fabs( offset );

	MATHEX_SCALAR r = fromColour_.r() + offset * (toColour_.r() - fromColour_.r())/halfPeriod;
	MATHEX_SCALAR g = fromColour_.g() + offset * (toColour_.g() - fromColour_.g())/halfPeriod;
	MATHEX_SCALAR b = fromColour_.b() + offset * (toColour_.b() - fromColour_.b())/halfPeriod;

	return RenColour(r, g, b);
}

//static
void W4dColourPulsePlan::makePlan( W4dEntity* pEntity, const ColourPulseDataVec& dataVec, const PhysAbsoluteTime& startTime, uint nRepetations, uint animId )
{
    const RenMeshInstance& meshInst = _CONST_CAST(const W4dEntity*, pEntity)->mesh();
	const Ren::MaterialVecPtr& materialVecPtr =	meshInst.materialVec();
	W4dMaterialPlanPtr matPlanPtr;

	if( materialVecPtr.isDefined() )
	{
		matPlanPtr = _NEW( W4dColourPulsePlan( dataVec, *materialVecPtr) );
	}
	else
	{
		RenMaterialVec* pAnimMaterialVec = meshInst.mesh()->materialVec().release();
		matPlanPtr = _NEW( W4dColourPulsePlan( dataVec, *pAnimMaterialVec) );
		_DELETE( pAnimMaterialVec );
	}

	pEntity->entityPlanForEdit().materialPlan(matPlanPtr, startTime, nRepetations, animId);
}

//static
bool W4dColourPulsePlan::changeColour( W4dEntity* pEntity, const RenTexture& texture, const RenColour& colour )
{
	bool found = false;

    const RenMeshInstance& meshInst = _CONST_CAST(const W4dEntity*, pEntity)->mesh();
    const Ren::MaterialVecPtr& materialVecPtr = meshInst.materialVec();
	RenMaterialVec* pAnimMaterialVec;

	if( materialVecPtr.isDefined() )
	{
		pAnimMaterialVec = _NEW( RenMaterialVec( *materialVecPtr ) );
	}
	else
	{
		pAnimMaterialVec =meshInst.mesh()->materialVec().release();
	}

	size_t	nMat = pAnimMaterialVec->size();

	ASSERT( nMat > 0, " no material " );

	if( nMat > 0 )
	{
		Ren::MaterialVecPtr materialVecPtr = _NEW( RenMaterialVec( nMat) );

		for( size_t i=0; i<nMat; ++i)
		{
			RenMaterial& mat = (*pAnimMaterialVec)[i];
						 mat.makeNonSharable();

			if( mat.texture() == texture )
			{
				found = true;
				if( mat.emissive() != RenColour::black())
				{
					double alpha = mat.emissive().a();
					RenColour newColour = colour;
					newColour.a( alpha );
					mat.emissive( newColour );
				}
				else
				{
					double alpha = mat.diffuse().a();
					RenColour newColour = colour;
					newColour.a( alpha );
					mat.diffuse( newColour );
				}
			}
			materialVecPtr->push_back( mat );
		}

		//apply the new materialVecPtr to the mesh
		//_CONST_CAST(RenMeshInstance, meshInst).materialVec( materialVecPtr );
		_CONST_CAST(RenMeshInstance& , meshInst).materialVec( materialVecPtr );
	}
	//delete the old to avoid mem leak
	_DELETE( pAnimMaterialVec );

	return found;
}

//static
void W4dColourPulsePlan::changeAllColour( W4dEntity* pEntity,
                                          const RenColour& fromColour,
                                          const RenColour& toColour,
                                          const PhysRelativeTime& period,
                                          const PhysAbsoluteTime& startTime,
                                          uint nPeriods, uint animId )
{
    const RenMeshInstance& meshInst = _CONST_CAST(const W4dEntity*, pEntity)->mesh();
    const Ren::MaterialVecPtr& materialVecPtr = meshInst.materialVec();

	RenMaterialVec* pAnimMaterialVec;

	if( materialVecPtr.isDefined() )
	{
		pAnimMaterialVec = _NEW( RenMaterialVec( *materialVecPtr ) );
	}
	else
	{
		pAnimMaterialVec =meshInst.mesh()->materialVec().release();
	}

	size_t	nMat = pAnimMaterialVec->size();

	ASSERT( nMat > 0, " no material " );

	if( nMat > 0 )
	{
		ctl_pvector< W4dColourPulseData > dataVec;
		dataVec.reserve(nMat);

		for( size_t i=0; i<nMat; ++i)
		{
			const string& texturePathName = (*pAnimMaterialVec)[i].texture().name();
			SysPathName pathName( texturePathName );
			const string& textureName = pathName.filename();
			W4dColourPulseData* pPulseData = _NEW( W4dColourPulseData( textureName, fromColour, toColour, period ) );
			dataVec.push_back ( pPulseData );
		}

		W4dMaterialPlanPtr matPlanPtr = _NEW( W4dColourPulsePlan( dataVec, *pAnimMaterialVec) );
		pEntity->entityPlanForEdit().materialPlan(matPlanPtr, startTime, nPeriods, animId);
	}

	//delete the old to avoid mem leak
	_DELETE( pAnimMaterialVec );
}

/* End COLPULSE.CPP *************************************************/
