/*
 * P R E S S P A D . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    MachLogPressurePads

    Keep track of all of the data associated with the pressure
    pads used to open and close the doors to constructions.
*/

#ifndef _MACHLOG_PRESSPAD_HPP
#define _MACHLOG_PRESSPAD_HPP

#include "base/base.hpp"

#include "phys/cspace2.hpp"

#include "ctl/map.hpp"

class MexPoint2d;
class MexCircle2d;
class MachLogConstruction;
class MexConvexPolygon2d;

class MachLogPressurePads
{
public:
    MachLogPressurePads( const MexPoint2d& minBound, const MexPoint2d& maxBound );
    ~MachLogPressurePads();

    void    add(
        const MexPoint2d& p1,
        const MexPoint2d& p2,
        const MexPoint2d& p3,
        const MexPoint2d& p4,
        MachLogConstruction* pConstruction,
        size_t entranceNum );

    bool onPad(
        const MexConvexPolygon2d& polygon,
        MachLogConstruction** ppConstruction,
        size_t* pEntrance );

    //True if circle intersects a pressure pad. If so returns the construction and entrance number
    //in ppConstruction and pEntrance.
    bool onPad( const MexCircle2d& circle, MachLogConstruction** ppConstruction, size_t* pEntrance );

    //removes all pressure pads associated with pConstruction
    void remove( MachLogConstruction* pConstruction );

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachLogPressurePads& t );

private:
    // Operation deliberately revoked
    MachLogPressurePads( const MachLogPressurePads& );

    // Operation deliberately revoked
    MachLogPressurePads& operator =( const MachLogPressurePads& );

    // Operation deliberately revoked
    bool operator ==( const MachLogPressurePads& );

    PhysConfigSpace2d* pEntranceConfigSpace_; //The config space for building entrances

    typedef std::pair< MachLogConstruction*, size_t >    EntranceData;

    typedef ctl_map< PhysConfigSpace2d::PolygonId, EntranceData, std::less< PhysConfigSpace2d::PolygonId > > EntranceMap;

    EntranceMap polygonToEntrance_;
};


#endif

/* End PRESSPAD.HPP *************************************************/
