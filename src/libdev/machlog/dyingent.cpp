/*
 * D Y I N G E N T . C P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions
#include "mathex/point2d.hpp"
#include "sim/manager.hpp"
#include "phys/cspace2.hpp"
#include "machlog/dyingent.hpp"
#include "machlog/internal/dyingeni.hpp"
#include "machlog/actor.hpp"
#include "machlog/planet.hpp"
#include "machlog/races.hpp"
#include "machlog/constron.hpp"

#define CB_MachLogDyingEntityEvent_DEPIMPL() \
	PRE( pImpl_ )	\
	CB_DEPIMPL( MachLogDyingEntityEvent::Polygons	,polygons_ ) \
	CB_DEPIMPL( CtlConstCountedPtr< W4dEntity >		,physObjectPtr_ ) \
	CB_DEPIMPL( MachLogConstruction*				,pConstruction_ ) \
	CB_DEPIMPL( bool								,insideBuilding_ );

    
MachLogDyingEntityEvent::MachLogDyingEntityEvent( const CtlConstCountedPtr< W4dEntity >& physObjectPtr,
    					const Polygons* pFromPolygons,
    					const PhysRelativeTime& goDeadTime,
						InsideBuilding insideBuilding,
						MachLogConstruction* pConstruction )

: SimDiscreteEvent( goDeadTime + SimManager::instance().currentTime() ),
	pImpl_( _NEW( MachLogDyingEntityEventImpl ) )
{
	CB_MachLogDyingEntityEvent_DEPIMPL();
	//HAL_STREAM(" going dead time gap " << goDeadTime << std::endl );
	//#ifndef NDEBUG
	physObjectPtr_ = physObjectPtr;
	pConstruction_ = pConstruction;
	insideBuilding_ = insideBuilding == INSIDE_BUILDING;

	if( pFromPolygons == NULL )
	{
		 HAL_STREAM(" no polygons\n" );
	}
	else
	{
		HAL_STREAM( pFromPolygons->size() << std::endl );
	}
	//#endif
	PRE( ( insideBuilding_ and pConstruction_ ) or not insideBuilding_ );
	//build array of polygons here....
	if( pFromPolygons and pFromPolygons->size() )
	{
        polygons_.reserve( pFromPolygons->size() );
		for( Polygons::const_iterator i = pFromPolygons->begin(); i!= pFromPolygons->end(); ++i )
		{
			//HAL_STREAM(" polygon id " << (*i) << std::endl );
			polygons_.push_back( *i );
		}
	}

    TEST_INVARIANT;
}

MachLogDyingEntityEvent::MachLogDyingEntityEvent( const CtlConstCountedPtr< W4dEntity >& physObjectPtr,
						PolygonId singlePolygonOnly,
    					const PhysRelativeTime& goDeadTime ,
						InsideBuilding insideBuilding,
						MachLogConstruction* pConstruction )
: SimDiscreteEvent( goDeadTime + SimManager::instance().currentTime() ),
	pImpl_( _NEW( MachLogDyingEntityEventImpl ) )

{
	CB_MachLogDyingEntityEvent_DEPIMPL();
	//build array of polygons here....
	physObjectPtr_ = physObjectPtr;
	pConstruction_ = pConstruction;
	insideBuilding_ = insideBuilding == INSIDE_BUILDING;
	//HAL_STREAM(" going dead time gap " << goDeadTime << std::endl );
	HAL_STREAM(" single Polygon Id " << singlePolygonOnly << std::endl );
	PRE( ( insideBuilding_ and pConstruction_ ) or not insideBuilding_ );
//	PolygonId* pPolygonId = _NEW( PolygonId( singlePolygonOnly ) );
//	HAL_STREAM(" created new PolygonId address (void*) " << (void*)pPolygonId << std::endl );
	polygons_.push_back( singlePolygonOnly );
	//HAL_STREAM(" **polygons.begin() " << (**polygons_.begin() ) << std::endl );
	HAL_STREAM(" polygons.front() " << (polygons_.front() ) << std::endl );

    TEST_INVARIANT;
}

MachLogDyingEntityEvent::~MachLogDyingEntityEvent()
{
	CB_MachLogDyingEntityEvent_DEPIMPL();
	HAL_STREAM("MLDyingEntityEvent::DTOR\n" );

	if( not polygons_.empty() )
	{
        //Work out which config space to remove the polygons from
    	PhysConfigSpace2d* pConfigSpace = NULL;
		if( insideBuilding_ and pConstruction_ )
		{
			MachLogConstruction& mlc = pConstruction_->asConstruction();
			pConfigSpace = &mlc.interiorConfigSpace();
		}
		else 
			pConfigSpace = &MachLogPlanet::instance().configSpace();

    	//remove polygons here
        for( Polygons::iterator i = polygons_.begin(); i != polygons_.end(); ++i )
		{
			HAL_STREAM(" asking the config space to remove id " << (*i) << std::endl );
			if( pConfigSpace->exists( *i ) )
			{
				pConfigSpace->remove( (*i) );
				HAL_STREAM(" removed.\n" );
			}
		}
	}

	HAL_STREAM(" done..physObject will now go out of scope\n" );
	_DELETE( pImpl_ );
    TEST_INVARIANT;
}

//virtual
void MachLogDyingEntityEvent::execute( )
{
	//HAL_STREAM("MLDyingEntity::execute\n" );
//	_DELETE( this );
}

void MachLogDyingEntityEvent::CLASS_INVARIANT
{
}

ostream& operator <<( ostream& o, const MachLogDyingEntityEvent& t )
{

    o << "MachLogDyingEntityEvent " << (void*)&t << " start" << std::endl;
	t.doOutputOperator( o );
    o << "MachLogDyingEntityEvent " << (void*)&t << " end" << std::endl;

    return o;
}

//virtual
void MachLogDyingEntityEvent::doOutputOperator( ostream& o ) const
{
	CB_DEPIMPL( const Polygons, polygons_ );
	o << " Polygons.size() " << polygons_.size() << std::endl;
	if( polygons_.size() )
		for( MachLogDyingEntityEvent::Polygons::const_iterator i = polygons_.begin(); i != polygons_.end(); ++i )
			o << "  Polygon " << *i << std::endl;
}

/* End DYINGENT.CPP *************************************************/
