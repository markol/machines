/*
 * G X M A T R I X 3 . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    GXMatrix3

    A brief description of the class should go in here
*/

#ifndef _GXMATRIX3_HPP
#define _GXMATRIX3_HPP

#include "base/base.hpp"

#include "gxin/gxvect3.hpp"

class GXMatrix3
// Orthodox canonical form
{
public:
    GXMatrix3();
    ~GXMatrix3();
    GXMatrix3( const GXMatrix3& );
    GXMatrix3& operator =( const GXMatrix3& );
    friend bool operator ==( const GXMatrix3&, const GXMatrix3& );

    void CLASS_INVARIANT;

	// not very pretty... get
	float xx() const { return a_.x(); }
	float xy() const { return a_.y(); }
	float xz() const { return a_.z(); }
	float yx() const { return b_.x(); }
	float yy() const { return b_.y(); }
	float yz() const { return b_.z(); }
	float zx() const { return c_.x(); }
	float zy() const { return c_.y(); }
	float zz() const { return c_.z(); }

	// set
	void xx(float val) { a_.x(val); }
	void xy(float val) { a_.y(val); }
	void xz(float val) { a_.z(val); }
	void yx(float val) { b_.x(val);	}
	void yy(float val) { b_.y(val);	}
	void yz(float val) { b_.z(val);	}
	void zx(float val) { c_.x(val);	}
	void zy(float val) { c_.y(val);	}
	void zz(float val) { c_.z(val);	}

	// set the matrix lines
	void x(const GXVect3 &x) { a_=x; }
	void y(const GXVect3 &y) { b_=y; }
	void z(const GXVect3 &z) { c_=z; }

    friend ostream& operator <<( ostream& o, const GXMatrix3& t );

private:

	// a_ : first line
	// b_ : second line
	// c_ : third line
	GXVect3 a_, b_, c_;
};


#endif

/* End GXMATRIX3.HPP ************************************************/
