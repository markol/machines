/*
 *  M U L T I C O L . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include <memory>

#include "world4d/internal/multicol.hpp"
#include "world4d/entity.hpp"
#include "world4d/entyplan.hpp"
#include "world4d/coldata.hpp"
#include "world4d/colpulse.hpp"

#include "render/mesh.hpp"
#include "render/meshinst.hpp"
#include "render/material.hpp"
#include "render/matvec.hpp"

PER_DEFINE_PERSISTENT(W4dMultiColourPulseData);

W4dMultiColourPulseData::W4dMultiColourPulseData(const size_t& nData)
:W4dAnimationData( "ColourPulseData", 0 )
{
	colourPulseDataVec_.reserve( nData );
    TEST_INVARIANT;
}

W4dMultiColourPulseData::~W4dMultiColourPulseData()
{
    TEST_INVARIANT;

	for( ctl_pvector< W4dColourPulseData >::iterator i = colourPulseDataVec_.begin(); i != colourPulseDataVec_.end(); ++i )
		_DELETE( *i );
}


void W4dMultiColourPulseData::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const W4dMultiColourPulseData& t )
{

    o << "W4dMultiColourPulseData " << (void*)&t << " start" << std::endl;
    o << "W4dMultiColourPulseData " << (void*)&t << " end" << std::endl;

    return o;
}


void W4dMultiColourPulseData::add( W4dColourPulseData* pData )
{
	colourPulseDataVec_.push_back( pData );
}

//virtual
void W4dMultiColourPulseData::apply( W4dEntity* pEntity, const PhysAbsoluteTime& startTime)
{
	PRE(pEntity);
	PRE(pEntity->hasMesh());

	Ren::ConstMeshPtr animMeshPtr = _CONST_CAST(const W4dEntity*, pEntity)->mesh().mesh();
	std::auto_ptr< RenMaterialVec > pAnimMaterialVec =animMeshPtr->materialVec();

    W4dMaterialPlanPtr matPlanPtr = _NEW( W4dColourPulsePlan( colourPulseDataVec_, *pAnimMaterialVec) );

	pEntity->entityPlanForEdit().materialPlan(matPlanPtr, startTime);


}

W4dMultiColourPulseData::W4dMultiColourPulseData( PerConstructor con )
: W4dAnimationData( con )
{
}

void perWrite( PerOstream& str, const W4dMultiColourPulseData& t )
{
	const W4dAnimationData& data = t;
	perWrite( str, data );

	str << t.colourPulseDataVec_;
}

void perRead( PerIstream& str, W4dMultiColourPulseData& t )
{
	W4dAnimationData& data = t;
	perRead( str, data );

	for( ctl_pvector< W4dColourPulseData >::iterator i = t.colourPulseDataVec_.begin(); i != t.colourPulseDataVec_.end(); ++i )
		_DELETE( *i );

	str >> t.colourPulseDataVec_;
}

/* End MULTICOL.CPP **************************************************/
