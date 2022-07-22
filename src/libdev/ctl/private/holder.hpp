/*
 * H O L D E R . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved.
 */

/*
	Private classes used by the counted pointer code.
*/

#ifndef _CTL_HOLDER_HPP
#define _CTL_HOLDER_HPP

#include "base/base.hpp"
#include "base/persist.hpp"

//////////////////////////////////////////////////////////////////////

class CtlCountedObject
{
public:

	CtlCountedObject();
	CtlCountedObject( const CtlCountedObject& );
	CtlCountedObject& operator =( const CtlCountedObject& );

	virtual ~CtlCountedObject() = 0;

	void addReference();
	void removeReference();

	bool isShared() const;

private:

	int		refs_;
};

template< class T >
class CtlCountHolder : public CtlCountedObject
{
public:
    CtlCountHolder();
    
	virtual ~CtlCountHolder();

	T * pT_;
    
    typedef CtlCountHolder< T >*    pointer;
    
    //  This doesn't need to be virtual because nothing
    //  derives from this class. It is an exception to the
    //  normal persistence rules.
    PER_MEMBER_PERSISTENT_DEFAULT( CtlCountHolder );
};

PER_DEFINE_PERSISTENT_INLINE_T1( CtlCountHolder );

#endif
