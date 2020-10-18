/*
 * M E S H P L A N. C P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

#include "world4d/root.hpp"
#include "world4d/meshplan.hpp"
#include "world4d/generic.hpp"
#include "mathex/double.hpp"

PER_DEFINE_PERSISTENT( W4dMeshPlan );

//////////////////////////////////////////////////////////////////////////////////////////

W4dMeshPlan::W4dMeshPlan( void )
:	pRoot_( NULL )
{
	//Set the extension size of the collection
	intervals_.reserve( 6 );

	//Construct a root entity to store the W4dGenerics we add for each set of meshes
	pRoot_ = _NEW( W4dRoot( W4dRoot::W4dRootId( 2 ) ) );
	POST( pRoot_ != NULL );

}
//////////////////////////////////////////////////////////////////////////////////////////

W4dMeshPlan::~W4dMeshPlan( void )
{

	//Delete the root along with all the W4dGenerics
	if( pRoot_ ) _DELETE( pRoot_ );
}
//////////////////////////////////////////////////////////////////////////////////////////

void W4dMeshPlan::add( const SysPathName& pathName, const PhysRelativeTime& endTime,
					   const MexTransform3d& transform )
{
	//Checks
	PRE( endTime > 0.0 );
	PRE( intervals_.size() == 0  or  intervals_.back().second < endTime );

	//Construct a W4dGeneric from the lod file
	W4dGeneric* pGeneric = _NEW( W4dGeneric( pRoot_, transform ) );
	pGeneric->loadLODFile( pathName );

	//Add the entity and the time as a pair to the collection
	intervals_.push_back( MeshInterval( pGeneric, endTime ) );
}
//////////////////////////////////////////////////////////////////////////////////////////

void W4dMeshPlan::renderAtRange( const W4dCamera& camera, MATHEX_SCALAR range,
								 W4dEntity* pEntity,
                                 const PhysRelativeTime& timeOffset, 
                                 const W4dComplexity& complexity ) const
{
	PRE( intervals_.size() != 0 );

	if( timeOffset >= 0.0  and  timeOffset <= intervals_.back().second )
	{
		//Find the appropriate interval
		for( MeshIntervals::const_iterator i = intervals_.begin();
		     i != intervals_.end(); ++i )
		{
			if( timeOffset <= (*i).second )
			{
				//Do this one. Attach the entity to the argument entity temporarily
				W4dEntity* pMeshEntity = (*i).first;
				POST( pMeshEntity != NULL );
				pMeshEntity->attachTo( pEntity, pMeshEntity->localTransform() );

				//Now render it
				pMeshEntity->renderAtRange( camera, range, complexity );

				//Reattach to the root
				pMeshEntity->attachTo( pRoot_, pMeshEntity->localTransform() );

				//That's all
				break;
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////////////////////////

PhysRelativeTime W4dMeshPlan::duration( void ) const
{
    return (intervals_.size() == 0 ? 0 : intervals_.back().second );
}

void perWrite( PerOstream& ostr, const W4dMeshPlan& plan )
{
    W4dMeshPlan::MeshInterval    i;
    ostr << i;
    
	ostr << plan.pRoot_;
	ostr << plan.intervals_;
}

void perRead( PerIstream& istr, W4dMeshPlan& plan )
{
	istr >> plan.pRoot_;
	istr >> plan.intervals_;
}

//////////////////////////////////////////////////////////////////////////////////////////
