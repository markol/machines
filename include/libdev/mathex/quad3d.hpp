/*
 * Q U A D 3 D . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

#ifndef _MATHEX_QUAD3D_HPP
#define _MATHEX_QUAD3D_HPP

#include "base/persist.hpp"
#include "base/base.hpp"
#include "mathex/point3d.hpp"

// Represents a four vertex polygon in 3D space.
class MexQuad3d
{
public:
    MexQuad3d(const MexPoint3d& v1, const MexPoint3d& v2, const MexPoint3d& v3, const MexPoint3d& v4);
    ~MexQuad3d();

    MexQuad3d( const MexQuad3d& );
    MexQuad3d& operator =( const MexQuad3d& );
    bool operator ==( const MexQuad3d& );
    bool operator !=( const MexQuad3d& );

	// PRE(i < 4)
	const MexPoint3d& vertex(uint i) const;

	// Checks for co-planarity.
    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MexQuad3d& t );

    //Persistence
    PER_MEMBER_PERSISTENT( MexQuad3d );
    PER_FRIEND_READ_WRITE( MexQuad3d );

private:
	MexPoint3d	vertices_[4];
};

PER_DECLARE_PERSISTENT( MexQuad3d );

#ifdef _INLINE
    #include "mathex/quad3d.ipp"
#endif

#endif

/* End QUAD3D.HPP ***************************************************/
