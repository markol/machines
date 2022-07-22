/*
 * H A S P H E R E 3 D . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MexSphere3d

   Non hierarchical version  of a sphere
*/

#ifndef _SPHERE3D_HPP
#define _SPHERE3D_HPP

#include "base/base.hpp"

#include "mathex/point3d.hpp"

class MexTransform3d;
class MexSausage3d;

class MexSphere3d 
// Canonical form revoked
{
public:
    MexSphere3d();
    MexSphere3d( const MexPoint3d& , MATHEX_SCALAR );
    ~MexSphere3d();

    void CLASS_INVARIANT;

   void transform( const MexTransform3d& transform );

   // set/get
   MATHEX_SCALAR radius() const { return radius_; }
   void radius( MATHEX_SCALAR newradius ) { radius_=newradius; }
   const MexPoint3d& center() const { return center_; }
   void center( const MexPoint3d& newCenter ) { center_=newCenter; }

   // true if the first argument intersects this 
   bool intersects( const MexSausage3d & otherShape ) const;
   bool intersects( const MexSphere3d & otherShape ) const;
//	virtual void embed( const ctl_vector<MexPoint3d>& );


private:
    friend ostream& operator <<( ostream& o, const MexSphere3d& t );

    MexSphere3d( const MexSphere3d& );
    MexSphere3d& operator =( const MexSphere3d& );

	MexPoint3d center_;
	MATHEX_SCALAR radius_;
};


#endif

/* End SPHERE3D.HPP *************************************************/
