/*
 * O B S T A C L E . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "planeted/obstacle.hpp"

#include "render/colour.hpp"

#include <fstream>

PedObstacle::CspType PedObstacle::highlightedType_ = PedObstacle::NORMAL;

PedObstacle::PedObstacle( const PolyVerticies& verticies, MATHEX_SCALAR height, bool selected, CspType cspType )
: 	PedPolygon( verticies, height, selected ),
	cspType_( cspType )
{

    TEST_INVARIANT;
}

PedObstacle::~PedObstacle()
{
    TEST_INVARIANT;

}

void PedObstacle::CLASS_INVARIANT
{
	INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const PedObstacle& t )
{

    o << "PedObstacle " << (void*)&t << " start" << std::endl;
    o << "PedObstacle " << (void*)&t << " end" << std::endl;

    return o;
}
// virtual
void PedObstacle::select( bool selected )
{
	PedPolygon::select( selected );

	highlightedType( cspType() );
}

// virtual
RenColour PedObstacle::getWireColour() const
{
	if( cspType_ == highlightedType_ )
	{
		return RenColour::white();
	}
	return RenColour::red();
}

// virtual
void PedObstacle::save( std::ofstream& out )
{
	out << "OBSTACLE " << verticies_.size() << " " << height() << std::endl;
	out << cspType_;
	if( cspType_ != NORMAL )
		out << std::endl;
	for ( ctl_vector< MexPoint2d >::iterator verIter = verticies_.begin();
		  verIter != verticies_.end();
		  ++verIter )
	{
		out << (*verIter).x() << " " << (*verIter).y() << std::endl;
	}
	out << std::endl;
}

ostream& operator <<( ostream& o, PedObstacle::CspType cspType )
{
	switch( cspType )
	{
		case PedObstacle::NORMAL: 	o << ""; break;
		case PedObstacle::LOW: 		o << "LOW_OBSTACLE"; break;
		case PedObstacle::WATER: 	o << "WATER"; break;
	}
	return o;
}

/* End OBSTACLE.CPP *************************************************/
