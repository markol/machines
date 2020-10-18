/*
 * C S 2 P O L Y G . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "mathex/poly2d.hpp"

#include "phys/internal/cs2polyg.hpp"

#include "utility/indent.hpp"

#ifndef _INLINE
    #include "phys/internal/cs2polyg.ipp"
#endif

//////////////////////////////////////////////////////////////////////////////////////////

PhysCS2dPolygon::PhysCS2dPolygon
(
    const PolygonId& id, std::auto_ptr< MexPolygon2d >& polygonAPtr,
    MATHEX_SCALAR height, Flags flags,
    const PhysConfigSpace2d::Longevity& longevity
)
:   id_( id ),
    polygon_(*(polygonAPtr.release()) ),
    height_( height ),
    longevity_( longevity ),
    enabled_( true ),
    flags_( flags )
{
    //Construct the bounding volume
    polygon_.boundary( &boundary_ );

    //Cache the edge lengths
    polygon_.isCachingData( true );

    TEST_INVARIANT;
}
//////////////////////////////////////////////////////////////////////////////////////////

PhysCS2dPolygon::~PhysCS2dPolygon()
{
    TEST_INVARIANT;

    //Delete the polygon
    MexPolygon2d* pPolygon = &polygon_;
    if( pPolygon != NULL ) _DELETE( pPolygon );
}
//////////////////////////////////////////////////////////////////////////////////////////

bool PhysCS2dPolygon::isEnabled( Flags flags ) const
{
    bool    result;

    if( enabled_ )
    {
        if( flags == 0 or flags_ == 0 )
            result = true;
        else
            result = ( flags & flags_ ) == 0;
    }
    else
        result = false;

    return result;
}

//////////////////////////////////////////////////////////////////////////////////////////

void PhysCS2dPolygon::CLASS_INVARIANT
{
	INVARIANT( this != NULL );
}
//////////////////////////////////////////////////////////////////////////////////////////

ostream& operator <<( ostream& o, const PhysCS2dPolygon& t )
{

    UtlIndentOstream    indentOstr( o, "  " );

    o << "PhysCS2dPolygon " << (void*)&t << " start" << std::endl;

    indentOstr << "  id " << t.id_.asScalar() << "  height " << t.height_;
    if( t.longevity_ == PhysConfigSpace2d::PERMANENT )
        indentOstr << " permanent ";
    else if( t.longevity_ == PhysConfigSpace2d::TEMPORARY )
        indentOstr << " temporary ";
    indentOstr << t.boundary_ << std::endl;
    indentOstr << "Enabled " << t.enabled_ << std::endl;
    indentOstr << "Flags " << t.flags_ << std::endl;
    indentOstr << t.polygon_;

    o << "PhysCS2dPolygon " << (void*)&t << " end" << std::endl;

    return o;
}
//////////////////////////////////////////////////////////////////////////////////////////
/* End CS2POLYG.CPP *************************************************/
