/*
 * E U L E R T X . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    PhysEulerTransform3d

    Store a transform as a set of euler angles and a position.
    This class was created to allow transforms which include
    several revolutions to be stored i.e. there is more than
    one representation for each possible orientation.
*/

#ifndef _PHYS_EULERTX_HPP
#define _PHYS_EULERTX_HPP

#include "base/base.hpp"
#include "base/persist.hpp"

#include "mathex/eulerang.hpp"
#include "mathex/point3d.hpp"

class MexTransform3d;

class PhysEulerTransform3d
{
public:
    PhysEulerTransform3d( void );
    PhysEulerTransform3d( const MexEulerAngles& );
    PhysEulerTransform3d( const MexEulerAngles&, const MexPoint3d& );
    PhysEulerTransform3d( const PhysEulerTransform3d& );
    PhysEulerTransform3d( const MexTransform3d& );

    ~PhysEulerTransform3d( void );

    PhysEulerTransform3d& operator =( const PhysEulerTransform3d& );

    const MexEulerAngles& rotation( void ) const;
    const MexPoint3d& position( void ) const;

    void transform( MexTransform3d* pTransform ) const;
        
    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const PhysEulerTransform3d& t );

    PER_MEMBER_PERSISTENT_DEFAULT( PhysEulerTransform3d );
    PER_FRIEND_READ_WRITE( PhysEulerTransform3d );
    
private:


    // Operation deliberately revoked
    bool operator ==( const PhysEulerTransform3d& );

    MexEulerAngles  angles_;
    MexPoint3d      position_;
};

PER_DECLARE_PERSISTENT( PhysEulerTransform3d );

#endif

/* End EULERTX.HPP **************************************************/
