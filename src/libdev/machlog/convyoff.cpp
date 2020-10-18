/*
 * C O N V Y O F F . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machlog/convyoff.hpp"

MachLogConvoyOffsets::MachLogConvoyOffsets()
{

    TEST_INVARIANT;
}

MachLogConvoyOffsets::~MachLogConvoyOffsets()
{
    TEST_INVARIANT;

}

void MachLogConvoyOffsets::CLASS_INVARIANT
{
}

//static
const MexPoint2d MachLogConvoyOffsets::convoyOffset
(
	const ConvoyType& type,
	const size_t& index,
	const MATHEX_SCALAR& clearence )
{
	MATHEX_SCALAR x = 0;
	MATHEX_SCALAR y = 0;
	MexPoint2d result( 0, 0 );
/*	//Things to remeber - the offset coordinates are used with the following axes
	+ve X
	^
	|
	|_____> +ve Y
*/
	switch( type )
	{
		case PATROL_CONVOY:
			//equilateral inverted triangle
			y = -1 * clearence;
			y += ( (index % 2) * 2 * clearence );
			x = ( index / 2 ) * clearence;
			x += (clearence * 1.5 );
			break;
		case KILLER_CONVOY:
			//thin inverted triangle
			y = -10;
			y += ( (index % 2) * 20 );
			x = (index / 2 ) * clearence;
			x += clearence;
			break;
		case LOCATOR_CONVOY:
			//column
			y = 0;
			x = clearence;
			x += ( clearence * index );
			break;
		default:
			ASSERT_BAD_CASE;
	}
	result.x( x );
	result.y( y );
	return result;

}

ostream& operator <<( ostream& o, const MachLogConvoyOffsets& t )
{

    o << "MachLogConvoyOffsets " << (void*)&t << " start" << std::endl;
    o << "MachLogConvoyOffsets " << (void*)&t << " end" << std::endl;

    return o;
}

/* End CONVYOFF.CPP *************************************************/
