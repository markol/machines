/*
 * G X V E C T 3 . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    GXVect3

    A brief description of the class should go in here
*/

#ifndef _GXVECT3_HPP
#define _GXVECT3_HPP

#include "base/base.hpp"

class GXVect3
// Canonical form revoked
{
public:
    GXVect3( void );
    ~GXVect3( void );
    GXVect3( const GXVect3& );
	// POST(*this==copy);
    GXVect3& operator =( const GXVect3& );
	// POST(*this==copy);
    friend bool operator ==(const GXVect3&, const GXVect3& );

    float x() const { return x_; }
	float y() const { return y_; }
	float z() const { return z_; }

    void x(float newX) { x_=newX; }
	void y(float newY) { y_=newY; }
	void z(float newZ) { z_=newZ; }

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const GXVect3& t );

private:
	float x_;
	float y_;
	float z_;
};

#endif

/* End GXVECT3.HPP **************************************************/
