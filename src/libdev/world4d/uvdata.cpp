/*
 * U V D A T A . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "world4d/internal/uvdata.hpp"
#include "world4d/entity.hpp"
#include "world4d/entyplan.hpp"
#include "world4d/uvplan.hpp"
#include "world4d/uvtrans.hpp"

#include "stdlib/string.hpp"
#include "render/texture.hpp"
#include "render/texmgr.hpp"

PER_DEFINE_PERSISTENT(W4dUVTranslateData);

W4dUVTranslateData::W4dUVTranslateData( const string& textureName, const MexVec2& speedVec, W4dLOD maxLod )
:W4dAnimationData( textureName, maxLod ),
speedVec_ ( speedVec )
{

    TEST_INVARIANT;
}

W4dUVTranslateData::~W4dUVTranslateData()
{
    TEST_INVARIANT;

}

void W4dUVTranslateData::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const W4dUVTranslateData& t )
{

    o << "W4dUVTranslateData " << (void*)&t << " start" << std::endl;
    o << "W4dUVTranslateData " << (void*)&t << " end" << std::endl;

    return o;
}

//virtual
void W4dUVTranslateData::apply( W4dEntity* pEntity, const PhysAbsoluteTime& startTime)
{
	PRE(pEntity);
	PRE(pEntity->hasMesh());

	size_t nameLength = name().length();
	string textureName = name();

	if( textureName.substr(nameLength-4, 4) != ".bmp" )
	{
		textureName += ".bmp";
	}

    const RenTexture keyTexture = RenTexManager::instance().createTexture( textureName );

	W4dUVTranslation* pUVTranslate = _NEW( W4dUVTranslation(forever(), maxLod(), speedVec_, MexVec2(0,0)) );
	pUVTranslate->setFilterTexture( keyTexture );
	W4dUVPlanPtr uvPlanPtr = pUVTranslate;

	pEntity->entityPlanForEdit().uvPlan(uvPlanPtr, startTime);

}

W4dUVTranslateData::W4dUVTranslateData( PerConstructor con )
: W4dAnimationData( con )
{
}

void perWrite( PerOstream& str, const W4dUVTranslateData& t )
{
	const W4dAnimationData& data = t;
	perWrite( str, data );

	str << t.speedVec_;
}

void perRead( PerIstream& str, W4dUVTranslateData& t )
{
	W4dAnimationData& data = t;
	perRead( str, data );

	str >> t.speedVec_;
}


/* End UVDATA.CPP ***************************************************/
