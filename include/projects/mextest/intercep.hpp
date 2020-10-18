/*
 * I N T E R C E P . H P P 
 * (c) Charybdis Limited, 1996. All Rights Reserved
 */


#ifndef _INTERCEP_HPP
#define _INTERCEP_HPP

#include "base/base.hpp"

#include "mathex/vec3.hpp"
#include "mathex/point3d.hpp"

typedef MexPoint3d        TestPoint3d;
typedef MexVec3           TestVec3;


class CalculateInterception
{
public:
    CalculateInterception( 
        const  TestPoint3d& chaserPosition,
        double chaserMaximumSpeed,
        const  TestPoint3d& chasedPosition,
        const  TestVec3& chasedDirection,
        double chasedSpeed );
        
    ~CalculateInterception( void );

    bool    interceptionFound( void ) const;

    TestVec3    chaserDirection( void ) const;

    friend ostream& operator <<( ostream& o, const CalculateInterception& t );

private:
    // Operation deliberately revoked
    CalculateInterception( const CalculateInterception& );

    // Operation deliberately revoked
    const CalculateInterception& operator =( const CalculateInterception& );

    // Operation deliberately revoked
    bool operator ==( const CalculateInterception& ) const;

    bool        interceptionFound_;
    double      timeToInterception_;
    TestVec3    chaserDirection_;
    TestPoint3d interceptionPosition_;

    bool    checkInterceptionCalculation(
        const  TestPoint3d& chaserPosition,
        double chaserMaximumSpeed,
        const  TestPoint3d& chasedPosition,
        const  TestVec3& chasedDirection,
        double chasedSpeed ) const;

};

#endif

/* End INTERCEP.HPP *************************************************/
