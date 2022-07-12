/*
 * C O M P M G R . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include <algorithm>

#include "machphys/compmgr.hpp"
#include "machphys/codynlig.hpp"
#include "machphys/cosky.hpp"
#include "machphys/covaptra.hpp"

//
// Ids associtaed with Complexity items are hard coded
// ids from 10000 to 10199 are reserved for this class
// 10000-10019 boolean items
// 10020-10039 choice items
//
// Correspondance table:
//
//	10000 	dynamic lights 		(MachPhysDynamicLightsComplexity)
//	10001 	vapour trails 		(MachPhysVapourTrailsComplexity)
// 	10020 	sky 				(MachPhysSkyComplexity)
//

// static
MachPhysComplexityManager& MachPhysComplexityManager::instance()
{
    static MachPhysComplexityManager instance_;
    return instance_;
}

static ItemId dynamicLightsId()
{
	return 10000;
}

static ItemId vapourTrailsId()
{
	return 10001;
}

static ItemId skyId()
{
	return 10020;
}

MachPhysComplexityManager::MachPhysComplexityManager()
: vapourTrailsEnabled_( true )
{
	booleanItems_.reserve( 1 );
	booleanItems_.push_back( _NEW( MachPhysDynamicLightsComplexity( dynamicLightsId(), true ) ) );
	booleanItems_.push_back( _NEW( MachPhysVapourTrailsComplexity( vapourTrailsId(), vapourTrailsEnabled_ ) ) );

	choiceItems_.reserve( 1 );
	choiceItems_.push_back( _NEW( MachPhysSkyComplexity( skyId() ) ) );

    TEST_INVARIANT;
}

MachPhysComplexityManager::~MachPhysComplexityManager()
{
    TEST_INVARIANT;

	for( BooleanItems::iterator it = booleanItems_.begin(); it != booleanItems_.end(); ++it )
		_DELETE( (*it) );

	for( ChoiceItems::iterator it = choiceItems_.begin(); it != choiceItems_.end(); ++it )
		_DELETE( (*it) );
}

#ifndef NDEBUG
static bool validId( uint id )
{
	return ( id>=10000 and id<=10199 );
}
#endif

const MachPhysComplexityManager::BooleanItems& MachPhysComplexityManager::booleanItems() const
{
	return booleanItems_;
}
const MachPhysComplexityManager::ChoiceItems& MachPhysComplexityManager::choiceItems() const
{
	return choiceItems_;
}

void MachPhysComplexityManager::changeBooleanItem( const uint& id, bool enabled )
{
	ASSERT_INFO(id);
	PRE(validId( id ) );

	bool idFound=false;
	for( BooleanItems::iterator it = booleanItems_.begin(); it != booleanItems_.end() and not idFound; ++it )
	{
		MachPhysComplexityBooleanItem *pItem = *it;
		if( pItem->id() == id )
		{
			pItem->changeState( enabled );
			pItem->update();
			idFound=true;
		}
	}
	ASSERT(idFound, "id not found");
}

void MachPhysComplexityManager::changeChoiceItem( const uint& id, uint choice )
{
	ASSERT_INFO(id);
	PRE(validId( id ) );

	bool idFound=false;
	for( ChoiceItems::iterator it = choiceItems_.begin(); it != choiceItems_.end() and not idFound; ++it )
	{
		MachPhysComplexityChoiceItem *pItem = *it;
		if( pItem->id() == id )
		{
			pItem->changeState( choice );
			pItem->update();
			idFound=true;
		}
	}
	ASSERT(idFound, "id not found");
}

// static
static void updateItem( MachPhysComplexityItem* item)
{
	item->update();
}

void MachPhysComplexityManager::updateSceneParameters()
{
	for_each( booleanItems_.begin(), booleanItems_.end(), updateItem );
	for_each( choiceItems_.begin(), choiceItems_.end(), updateItem );
}


bool MachPhysComplexityManager::hasPlanetSurface() const
{
	return pPlanetSurface_!=NULL;
}

void MachPhysComplexityManager::planetSurface( MachPhysPlanetSurface* planetSurface )
{
	pPlanetSurface_=planetSurface;
}

MachPhysPlanetSurface* MachPhysComplexityManager::planetSurface()
{
	return pPlanetSurface_;
}

bool MachPhysComplexityManager::vapourTrailsEnabled() const
{
	return vapourTrailsEnabled_;
}

void MachPhysComplexityManager::vapourTrailsEnabled( bool enabled )
{
	vapourTrailsEnabled_=enabled;
}


void MachPhysComplexityManager::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachPhysComplexityManager& t )
{

    o << "MachPhysComplexityManager " << (void*)&t << " start" << std::endl;
    o << "MachPhysComplexityManager " << (void*)&t << " end" << std::endl;

    return o;
}

/* End COMPMGR.CPP **************************************************/
