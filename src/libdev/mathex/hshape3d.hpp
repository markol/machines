/*
 * H S H A P E . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MexHierShape3d

	MexHierShape3d is the virtual parent class for different shapes
*/

#ifndef _HSHAPE3D_HPP
#define _HSHAPE3D_HPP

#include "base/base.hpp"

#include "ctl/vector.hpp"

#include "mathex/mathex.hpp"
#include "mathex/point3d.hpp"

class MexTransform3d;
class MexHierSausage3d;
class MexHierSphere3d;

class MexHierShape3d
{
// orthodox canonical form
public:
    MexHierShape3d();
    virtual ~MexHierShape3d();

    void CLASS_INVARIANT;

   typedef ctl_vector<MexPoint3d> PointList;
   virtual bool hasZeroVolume() const { return true; }
   virtual void transform( const MexTransform3d& transform ) = 0;
   virtual void transform(const MexTransform3d& transform, MexHierShape3d* &pShape ) const = 0;
   // PRE(result==NULL);

	virtual bool contains(const PointList&, Mathex::TouchingRule rule = Mathex::TOUCH_IS_INTERSECT) const=0;
	virtual bool contains(const MexPoint3d&, Mathex::TouchingRule rule = Mathex::TOUCH_IS_INTERSECT) const=0;

   // double dispatched method 
   // true if the first argument intersects this 
	virtual bool intersects( const MexHierShape3d & otherShape ) const = 0;
	virtual bool intersects( const MexHierSausage3d & otherShape ) const = 0;
	virtual bool intersects( const MexHierSphere3d & otherShape ) const = 0;
	virtual void stuff( const PointList& points )=0;

protected:
	virtual void streamout(ostream& o) const;

private:
    friend ostream& operator <<( ostream& o, const MexHierShape3d& t );

    MexHierShape3d( const MexHierShape3d& );
    MexHierShape3d& operator =( const MexHierShape3d& );


};


#endif

/* End HSHAPE3D.HPP ***************************************************/
