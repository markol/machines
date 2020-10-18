/*
 * P R E S S P A D . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machlog/presspad.hpp"
#include "machlog/constron.hpp"
#include "machlog/machine.hpp"

#include "machphys/consdata.hpp"

#include "phys/cspace2.hpp"

#include "mathex/vec2.hpp"
#include "mathex/line2d.hpp"
#include "mathex/transf3d.hpp"
#include "mathex/cvexpgon.hpp"

MachLogPressurePads::MachLogPressurePads( const MexPoint2d& minBound, const MexPoint2d& maxBound )
{
    pEntranceConfigSpace_ = _NEW( PhysConfigSpace2d( minBound, maxBound,
                                             PhysConfigSpace2d::SUBTRACTIVE,
                                             0.0, MachLogMachine::minLowClearance(),
                                             MachLogMachine::maxLowClearance(),
                                             MachLog::OBSTACLE_ALL ) );

    TEST_INVARIANT;
}

MachLogPressurePads::~MachLogPressurePads()
{
    TEST_INVARIANT;

    _DELETE( pEntranceConfigSpace_ );
}

void    MachLogPressurePads::add(
    const MexPoint2d& p1,
    const MexPoint2d& p2,
    const MexPoint2d& p3,
    const MexPoint2d& p4,
    MachLogConstruction* pConstruction,
    size_t entranceNum )
{
    ctl_vector< MexPoint2d >   points;
    points.reserve( 4 );
    points.push_back( p1 );
    points.push_back( p2 );
    points.push_back( p3 );
    points.push_back( p4 );

    MexConvexPolygon2d*  pPad = _NEW( MexConvexPolygon2d( points ) );

    std::auto_ptr< MexPolygon2d >    padAPtr( pPad );

    MATHEX_SCALAR   padHeight = 20.0;

    PhysConfigSpace2d::PolygonId id =
      pEntranceConfigSpace_->add( padAPtr, padHeight, MachLog::OBSTACLE_NORMAL, PhysConfigSpace2d::PERMANENT );

    polygonToEntrance_.insert( id, EntranceData( pConstruction, entranceNum ) );
}

bool MachLogPressurePads::onPad(
    const MexConvexPolygon2d& polygon,
    MachLogConstruction** ppConstruction,
    size_t* pEntrance )
{
    PRE( pEntranceConfigSpace_ != NULL );

    bool    result = false;
    PhysConfigSpace2d::PolygonId id;

    if( not pEntranceConfigSpace_->contains( polygon, MachLog::OBSTACLE_NORMAL, &id ) )
    {
        result = true;

        EntranceMap::iterator i = polygonToEntrance_.find( id );

        ASSERT( i != polygonToEntrance_.end(), "Polygon was not in pressure pad map" );

        *ppConstruction = (*i).second.first;
        *pEntrance = (*i).second.second;

    }

    return result;
}

void MachLogPressurePads::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachLogPressurePads& t )
{

    o << "MachLogPressurePads " << (void*)&t << " start" << std::endl;
    o << "MachLogPressurePads " << (void*)&t << " end" << std::endl;

    return o;
}

void MachLogPressurePads::remove( MachLogConstruction* pConstruction )
{
    //Iterate through all the map entries, looking for refernces to the construction
    EntranceMap::iterator nextIt;
    for( EntranceMap::iterator it = polygonToEntrance_.begin(); it != polygonToEntrance_.end(); it = nextIt )
    {
        //Advance iterator in case we erase the current entry
        nextIt = it;
        ++nextIt;

        //Check for match with target construction
        const EntranceData& entranceData = (*it).second;
        if( entranceData.first == pConstruction )
        {
            //Yes we need to erase this entry. First remove the polygon from the config space.
            PhysConfigSpace2d::PolygonId polygonId = (*it).first;
            pEntranceConfigSpace_->remove( polygonId );

            //erase the map entry
            polygonToEntrance_.erase( it );
        }
    }
}

bool MachLogPressurePads::onPad(
    const MexCircle2d& circle,
    MachLogConstruction** ppConstruction,
    size_t* pEntrance )
{
    PRE( pEntranceConfigSpace_ != NULL );

    bool    result = false;
    PhysConfigSpace2d::PolygonId id;

    if( not pEntranceConfigSpace_->contains( circle, MachLog::OBSTACLE_NORMAL, &id ) )
    {
        result = true;

        EntranceMap::iterator i = polygonToEntrance_.find( id );

        ASSERT( i != polygonToEntrance_.end(), "Polygon was not in pressure pad map" );

        *ppConstruction = (*i).second.first;
        *pEntrance = (*i).second.second;
    }

    return result;
}
/* End PRESSPAD.CPP *************************************************/
