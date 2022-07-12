/*
 * H A S P H E R E 3 D . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MexHierSphere3d

    A brief description of the class should go in here
*/

#ifndef _HSPHERE3D_HPP
#define _HSPHERE3D_HPP

#include "base/base.hpp"

#include "mathex/hshape3d.hpp"
#include "mathex/point3d.hpp"

class MexTransform3d;

class MexHierSphere3d : public MexHierShape3d
// Canonical form revoked
{
public:
    MexHierSphere3d();
    MexHierSphere3d( const MexPoint3d& , MATHEX_SCALAR );
    MexHierSphere3d( const MexHierSphere3d& );
    const MexHierSphere3d& operator =( const MexHierSphere3d& );
    virtual ~MexHierSphere3d();

    void CLASS_INVARIANT;

   static bool colinearPoints(const PointList& points);
   static bool coplanarPoints(const PointList& points);

   virtual bool hasZeroVolume() const { return ray_==0; }

   // set/get
   MATHEX_SCALAR ray() const { return ray_; }
   void ray( MATHEX_SCALAR newRay ) { ray_=newRay; }
   const MexPoint3d& center() const { return center_; }
   void center( const MexPoint3d& newCenter ) { center_=newCenter; }

   virtual void transform( const MexTransform3d& transform );
   virtual void transform(const MexTransform3d& transform, MexHierShape3d* &pShape ) const;
   // PRE(result==NULL);

	// 
   void matchBoundaryToPoints(const PointList& points);
   // PRE(MexHierSphere3d::isValidBoundary())
   // POST(contains(points));

   // true if the points are a valid boundary for the sphere
   // (1 point) or (two non equal points) or (three non colinear points) or (four non coplanar points)
   static bool areValidBoundary(const PointList& points);

   virtual bool contains(const PointList&, Mathex::TouchingRule rule = Mathex::TOUCH_IS_INTERSECT) const;
   virtual bool contains(const MexPoint3d&, Mathex::TouchingRule rule = Mathex::TOUCH_IS_INTERSECT) const;

   // double dispatched method 
   // true if the first argument intersects this 
   virtual bool intersects( const MexHierShape3d & otherShape ) const;
   virtual bool intersects( const MexHierSausage3d & otherShape ) const;
   virtual bool intersects( const MexHierSphere3d & otherShape ) const;
   virtual void stuff( const PointList& points );
   // PRE(points.size()>=1);
   // POST(contains(points));

protected:
	virtual void streamout(ostream& o) const;

private:
    friend ostream& operator <<( ostream& o, const MexHierSphere3d& t );

	MexPoint3d center_;
	MATHEX_SCALAR ray_;
};


#endif

/* End HSPHERE3D.HPP *************************************************/
