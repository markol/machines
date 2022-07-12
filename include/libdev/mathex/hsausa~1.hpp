/*
 * H S A U S A G E . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MexHierSausage3d

    A brief description of the class should go in here
*/

#ifndef _HSAUSAGE3D_HPP
#define _HSAUSAGE3D_HPP

#include "base/base.hpp"

#include "mathex/point3d.hpp"
#include "mathex/hshape3d.hpp"

class MexTransform3d;

class MexHierSausage3d : public MexHierShape3d
{
public:
    MexHierSausage3d();
    MexHierSausage3d( const MexPoint3d&, const MexPoint3d&, MATHEX_SCALAR );
    MexHierSausage3d( const MexHierSausage3d& );
    const MexHierSausage3d& operator =( const MexHierSausage3d& );
    virtual ~MexHierSausage3d();

    void CLASS_INVARIANT;

   virtual bool hasZeroVolume() const { return ray_==0; }

   // set/get
   MATHEX_SCALAR ray() const { return ray_; }
   void ray( MATHEX_SCALAR newRay ) { ray_=newRay; }
   MATHEX_SCALAR length() const { return length_; }
   const MexPoint3d& startPoint() const { return startPoint_; }
   void startPoint( const MexPoint3d& newStartPoint ); 
   const MexPoint3d& endPoint() const { return endPoint_; }
   void endPoint( const MexPoint3d& newEndPoint );

   virtual void transform(const MexTransform3d& transform );
   virtual void transform(const MexTransform3d& transform, MexHierShape3d* &pShape ) const ;
   //PRE(result==NULL);

   virtual bool contains(const PointList&, Mathex::TouchingRule rule = Mathex::TOUCH_IS_INTERSECT) const;
   virtual bool contains(const MexPoint3d&, Mathex::TouchingRule rule = Mathex::TOUCH_IS_INTERSECT) const;

   // double dispatched method 
   // true if the first argument intersects this 
	virtual bool intersects( const MexHierShape3d & otherShape ) const;
	virtual bool intersects( const MexHierSausage3d & otherShape ) const;
	virtual bool intersects( const MexHierSphere3d & otherShape ) const;
    virtual void stuff( const PointList& points );

protected:
	virtual void streamout(ostream& o) const;

private:
    friend ostream& operator <<( ostream& o, const MexHierSausage3d& t );

	MexPoint3d startPoint_;
	MexPoint3d endPoint_;
	MATHEX_SCALAR length_;
	MATHEX_SCALAR ray_;
};


#endif

/* End HSAUSAGE3D.HPP ************************************************/
