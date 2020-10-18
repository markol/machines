/*
 *  M U L T I T E X . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "world4d/internal/multitex.hpp"
#include "world4d/entity.hpp"
#include "world4d/entyplan.hpp"
#include "world4d/visplan.hpp"
#include "world4d/internal/texdata.hpp"
#include "world4d/matfrmpl.hpp"

#include "render/mesh.hpp"
#include "render/meshinst.hpp"
#include "render/material.hpp"
#include "render/matvec.hpp"
#include "render/texmgr.hpp"

#include "ctl/pvector.hpp"

PER_DEFINE_PERSISTENT(W4dCycleMultiTextureData);

W4dCycleMultiTextureData::W4dCycleMultiTextureData(const size_t& nData)
:W4dAnimationData( "MultiTextureData", 0 )
{
	cycleTextureDataVec_.reserve( nData );
    TEST_INVARIANT;
}

W4dCycleMultiTextureData::~W4dCycleMultiTextureData()
{
    TEST_INVARIANT;

	for( ctl_pvector< W4dCycleTextureData >::iterator i = cycleTextureDataVec_.begin(); i != cycleTextureDataVec_.end(); ++i )
		_DELETE( *i );
}


void W4dCycleMultiTextureData::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const W4dCycleMultiTextureData& t )
{

    o << "W4dCycleMultiTextureData " << (void*)&t << " start" << std::endl;
    o << "W4dCycleMultiTextureData " << (void*)&t << " end" << std::endl;

    return o;
}


void W4dCycleMultiTextureData::add( W4dCycleTextureData* pData )
{
	cycleTextureDataVec_.push_back( pData );
}

//virtual
void W4dCycleMultiTextureData::apply( W4dEntity* pEntity, const PhysAbsoluteTime& startTime)
{
	PRE(pEntity);
	PRE(pEntity->hasMesh());

	//create the keyTextures
	ctl_vector< ctl_vector<RenTexture> > textures2d = keyTextures2d();
	ASSERT( textures2d.size() == cycleTextureDataVec_.size(), "" );

	const size_t nData = cycleTextureDataVec_.size();

	//get the basic materialVec
	Ren::ConstMeshPtr animMeshPtr = _CONST_CAST(const W4dEntity*, pEntity)->mesh().mesh();
	RenMaterialVec* pAnimMaterialVec =animMeshPtr->materialVec().release();

	//find the animation keys in pAnimMaterialVec
	uint nMaterials = pAnimMaterialVec->size();

	ctl_vector<uint> offsets;
	ctl_vector<uint> texturesSetNumbers;

	offsets.reserve(nMaterials);
	texturesSetNumbers.reserve(nMaterials);

	for( uint i=0; i<nMaterials; ++i)
	{
		offsets.push_back(9999);
		texturesSetNumbers.push_back(9999);;
	}

	for( uint iData = 0; iData < nData; ++iData )
	{
		const ctl_vector<RenTexture>& textures = textures2d[iData];

		for(uint animFrame = 0; animFrame < nMaterials; ++animFrame)
		{
			const RenTexture& tex = ((*pAnimMaterialVec)[animFrame]).texture();

			ctl_vector<RenTexture>::const_iterator it = find( textures.begin(), textures.end(), tex );
			if( it != textures.end() )
			{
				uint frameOffset = it - textures.begin();
				offsets[animFrame] = frameOffset;
				texturesSetNumbers[animFrame] =  iData;
			}
		}
	}

	//find the minimum number of frames
	uint nFrames = 1;

	for( uint iData = 0; iData < nData; ++iData )
	{
		const ctl_vector<RenTexture>& iTextures = textures2d[iData];
		size_t iFrames = iTextures.size();
		const size_t iFrameReps = cycleTextureDataVec_[iData]->nRepetations_;
		iFrames *= iFrameReps;

		bool isExcluded = false;
		for( uint jData = iData+1; jData < nData; ++jData )
		{
			const ctl_vector<RenTexture>& jTextures = textures2d[jData];
			size_t jFrames = jTextures.size();
			const size_t jFrameReps = cycleTextureDataVec_[jData]->nRepetations_;
			jFrames *= jFrameReps;

			if( jFrames%iFrames == 0 )
			{
				isExcluded = true;
			}
		}
		if( not isExcluded )

		nFrames *= iFrames;
	}

	MaterialVecPtrs *pMaterials = _NEW( MaterialVecPtrs );
	pMaterials->reserve(nFrames);

	for( size_t i = 0; i != nFrames; ++i )
	{
		RenMaterialVec* pMaterialsVec = _NEW( RenMaterialVec( nMaterials) );

		for(size_t j=0; j< nMaterials; ++j)
		{
			RenMaterial mat = (*pAnimMaterialVec)[j];
			uint offset = offsets[j];
			uint jTexturesSetNumber	= texturesSetNumbers[j];
			if( offset != 9999 and jTexturesSetNumber != 9999)
			{
				uint jTextures = textures2d[jTexturesSetNumber].size();
				uint nFrameReps = cycleTextureDataVec_[jTexturesSetNumber]->nRepetations_;
				jTextures *= nFrameReps;
				uint frameNumber = (i + offset)%jTextures;
				frameNumber /= nFrameReps;
				mat.texture(textures2d[jTexturesSetNumber][frameNumber]);
			}
			pMaterialsVec->push_back(mat);
		}

		pMaterials->push_back( Ren::MaterialVecPtr( pMaterialsVec ) );

	}

	//Store in counted pointer
	MaterialVecPtrsPtr materialsPtr = pMaterials;

	static const PhysRelativeTime oneYear = 31536000;
	W4dMaterialFramePlan  *pMaterialFramePlan =
	                     _NEW( W4dMaterialFramePlan(materialsPtr, 0, oneYear, 1, 0) );

	W4dMaterialPlanPtr materialPlanPtr( pMaterialFramePlan );
	pEntity->entityPlanForEdit().materialPlan( materialPlanPtr, startTime );

	_DELETE(pAnimMaterialVec);
}

ctl_vector< ctl_vector<RenTexture> > W4dCycleMultiTextureData::keyTextures2d() const
{
	size_t nData = cycleTextureDataVec_.size();
	ctl_vector< ctl_vector<RenTexture> >  textures2d;
	textures2d.reserve( nData );

    for( int i = 0; i < nData; ++i )
	{
		ctl_vector<RenTexture> textures = cycleTextureDataVec_[i]->keyTextures();
		textures2d.push_back( textures );
	}

    return textures2d;
}

W4dCycleMultiTextureData::W4dCycleMultiTextureData( PerConstructor con )
: W4dAnimationData( con )
{
}

void perWrite( PerOstream& str, const W4dCycleMultiTextureData& t )
{
	const W4dAnimationData& data = t;
	perWrite( str, data );

	str << t.cycleTextureDataVec_;
}

void perRead( PerIstream& str, W4dCycleMultiTextureData& t )
{
	W4dAnimationData& data = t;
	perRead( str, data );

	for( ctl_pvector< W4dCycleTextureData >::iterator i = t.cycleTextureDataVec_.begin(); i != t.cycleTextureDataVec_.end(); ++i )
		_DELETE( *i );

	str >> t.cycleTextureDataVec_;
}

/* End MULTITEX.CPP **************************************************/
