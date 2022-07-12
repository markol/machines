/*
 * S A U S A G E . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MexSausage3d

   Non hierarchical declaration of a 3d sausage
*/


#ifndef _SAUSAGE3D_HPP
#define _SAUSAGE3D_HPP

#include "base/base.hpp"

#include "mathex/point3d.hpp"

class MexTransform3d;
class MexSphere3d;

class MexSausage3d 
{
public:
    MexSausage3d();
    MexSausage3d( const MexPoint3d&, const MexPoint3d&, MATHEX_SCALAR );
    virtual ~MexSausage3d();

    void CLASS_INVARIANT;

   void transform( const MexTransform3d& transform );

   // set/get
   MATHEX_SCALAR radius() const { return radius_; }
   void radius( MATHEX_SCALAR newradius ) { radius_=newradius; }
   MATHEX_SCALAR length() const { return length_; }
   const MexPoint3d& startPoint() const { return startPoint_; }
   void startPoint( const MexPoint3d& newStartPoint ); 
   const MexPoint3d& endPoint() const { return endPoint_; }
   void endPoint( const MexPoint3d& newEndPoint );

   // true if the first argument intersects this 
   bool intersects( const MexSausage3d & otherShape ) const;
   bool intersects( const MexSphere3d & otherShape ) const;
//	virtual void embed( const ctl_vector<MexPoint3d>& points);


private:
    friend ostream& operator <<( ostream& o, const MexSausage3d& t );

    MexSausage3d( const MexSausage3d& );
    MexSausage3d& operator =( const MexSausage3d& );

	MexPoint3d startPoint_;
	MexPoint3d endPoint_;
	MATHEX_SCALAR length_;
	MATHEX_SCALAR radius_;
};


#endif

/* End SAUSAGE3D.HPP ************************************************/
