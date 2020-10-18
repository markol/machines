/*
 * G X C O L O R . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    GXColor

    A brief description of the class should go in here
*/

#ifndef _GXCOLOR_HPP
#define _GXCOLOR_HPP

#include "base/base.hpp"
#include "gx.hpp"

class GXColor
// Canonical form revoked
{
public:
    GXColor( void );
    ~GXColor( void );
    GXColor( const GXColor& );
    GXColor& operator =( const GXColor& );
    friend bool operator ==( const GXColor&, const GXColor& );
	friend bool operator  <(const GXColor&, const GXColor&);

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const GXColor& t );

	UCHAR red(void) const {
	  return red_;  
	}
	void red(UCHAR newRed) {
	  red_=newRed;
	}
	UCHAR green(void) const {
	  return green_;  
	}
	void green(UCHAR newGreen) {
	  green_=newGreen;
	}
	UCHAR blue(void) const {
	  return blue_;  
	}
	void blue(UCHAR newBlue) {
	  blue_=newBlue;
	}

private:

	UCHAR red_;
	UCHAR green_;
	UCHAR blue_;
};


#endif

/* End GXCOLOR.HPP **************************************************/
