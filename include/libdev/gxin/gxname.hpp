/*
 * G X N A M E . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
	GXName
	same as std::string  just add std::string () interface method to prevent
	future changes of the class
	No need for a class definition at this point
*/
#ifndef _GX_NAME_HPP
#define _GX_NAME_HPP

#include <string>

class GXName : public std::string
{
  public:
  GXName() {}
  ~GXName() {}
  GXName(const GXName& copy): std::string (copy.str()) {}
  GXName(const char *rhs): std::string (rhs) {}

  const GXName& operator = (const GXName& copy)
  {
    std::string ::operator = ( copy.str() );
	return *this;
  }
  const GXName& operator = ( const char *copy ) {
    std::string ::operator = ( copy );
    return *this;
  }

  const std::string & str() const {
    return (std::string &) *this;
  }
};

#endif
