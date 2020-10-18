/*
 * E X P H O L D R . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machphys/expholdr.hpp"
#include "machphys/cexpdata.hpp"

MachPhysGenericExplosionData::MachPhysGenericExplosionData()
{

    TEST_INVARIANT;
}

MachPhysGenericExplosionData::~MachPhysGenericExplosionData()
{
    TEST_INVARIANT;

}

//static
const CtlConstCountedPtr< MachPhysConstructionExplosionData >&
MachPhysGenericExplosionData::genericExplosion( size_t level )
{
	PRE_INFO( level );
	PRE( level > 0 and level < 6 );
	static bool defined[ 5 ] = { false, false, false, false , false };
	static CtlConstCountedPtr< MachPhysConstructionExplosionData > dataPtr[ 5 ];

	if( not defined[ level - 1 ] )
	{
	    MachPhysConstructionExplosionData*   pData =
	      _NEW( MachPhysConstructionExplosionData( 15.0 ) );

	    MATHEX_SCALAR maxFireballOffset = 2.0;
	    MATHEX_SCALAR depthOffset = -7;

	    pData->addPoint( MachPhysConstructionExplosionData::ExplosionPoint(
	        MexPoint3d(0.0, 0.0, 3.0 ),
	        maxFireballOffset,
	        1, 2,
	        0.0,
	        0.0,
	        12,
	        depthOffset + 0.5 ) );

	    pData->addPoint( MachPhysConstructionExplosionData::ExplosionPoint(
	        MexPoint3d(0.0, 0.0, 16.0 ),
	        maxFireballOffset,
	        1, 2,
	        0.0,
	        0.0,
	        12,
	        depthOffset + 0.5 ) );
	    pData->addPoint( MachPhysConstructionExplosionData::ExplosionPoint(
	        MexPoint3d(0.0, 0.0, 2.0 ),
	        maxFireballOffset,
	        2, 5,
	        0.5,
	        0.7,
	        6,
	        depthOffset ) );
		if( level > 1 )
		{
		    pData->addPoint( MachPhysConstructionExplosionData::ExplosionPoint(
		        MexPoint3d(0.0, 0.0, 8.0 ),
		        maxFireballOffset,
		        2, 5,
		        0.1,
		        0.9,
		        3,
		        depthOffset ) );
		}
		if( level > 3 )
		{
		    pData->addPoint( MachPhysConstructionExplosionData::ExplosionPoint(
		        MexPoint3d(0.0, 0.0, 8.0 ),
		        maxFireballOffset,
		        5, 7,
		        0.1,
		        0.9,
		        2,
		        depthOffset ) );
		}
		dataPtr[ level - 1 ] = CtlConstCountedPtr< MachPhysConstructionExplosionData >( pData );
		defined[ level - 1 ] = true;
	}
	return dataPtr[ level - 1 ];
}

void MachPhysGenericExplosionData::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachPhysGenericExplosionData& t )
{

    o << "MachPhysGenericExplosionData " << (void*)&t << " start" << std::endl;
    o << "MachPhysGenericExplosionData " << (void*)&t << " end" << std::endl;

    return o;
}

/* End EXPHOLDR.CPP *************************************************/
















