/*
 * C O U N T P T R . H P P
 * (c) Charybdis Limited, 1996. All Rights Reserved.
 */

/*
    CtlCountedPtr is in fact a renamed version of Scott Meyer's
    RCIPtr. See "More Effective C++" Item 29 for details.
*/

#ifndef _CTL_COUNTPTR_HPP
#define _CTL_COUNTPTR_HPP

#include "base/base.hpp"
#include "base/persist.hpp"
#include "ctl/private/holder.hpp"

//////////////////////////////////////////////////////////////////////

template< class T >
class CtlConstCountedPtr;

template < class T >
class CtlCountedPtr;
template < class T >
bool operator <( const CtlCountedPtr< T > a, const CtlCountedPtr< T > b );
template <class T >
bool operator ==( const CtlCountedPtr< T > a, const CtlCountedPtr< T > b );


template < class T >
class CtlCountedPtr
{
public:

	CtlCountedPtr( T *tPtr = NULL );

	CtlCountedPtr( const CtlCountedPtr& );

	~CtlCountedPtr();

	CtlCountedPtr& operator =( const CtlCountedPtr& );

	T * operator ->() const;
	T&  operator  *() const;
	// PRE( tPtr_ != NULL );

    bool isDefined() const;
    void clear();

	void destruct();
	// PRE( not isShared() );

	bool isShared() const;

    PER_MEMBER_PERSISTENT_DEFAULT( CtlCountedPtr );

private:
    friend class CtlConstCountedPtr< T >;

	void init();

    typename CtlCountHolder< T >::pointer pCount_;

    friend
    bool operator < < >( const CtlCountedPtr< T > a, const CtlCountedPtr< T > b );

    friend
    bool operator == < >( const CtlCountedPtr< T > a, const CtlCountedPtr< T > b );

    //  These functions are a hack to allow perRead and perWrite
    //  to get at the internals of the class. perRead and perWrite cannot be made
    //  friends because then they will get instantiated for everything
    //  that the counted pointer is defined over. This would then force
    //  us to use only persistent obects with counted pointers - a bad thing.

    friend const typename CtlCountHolder< T >::pointer& ctl_cp_holder( const CtlCountedPtr& t )
        { return t.pCount_; }
    friend typename CtlCountHolder< T >::pointer& ctl_cp_holder( CtlCountedPtr& t )
        { return t.pCount_; }
};

PER_DEFINE_PERSISTENT_INLINE_T1( CtlCountedPtr );

//////////////////////////////////////////////////////////////////////
template < class T >
bool operator < ( const CtlConstCountedPtr< T > a, const CtlConstCountedPtr< T > b );
template <class T >
bool operator == ( const CtlConstCountedPtr< T > a, const CtlConstCountedPtr< T > b );

// As above, but clients only have const access to the pointed-to object.
template < class T >
class CtlConstCountedPtr
{
public:
	// Note: the argument is not a const T because this class requires
	// non-const access in order to delete the object.
	CtlConstCountedPtr( T *tPtr = NULL );

	CtlConstCountedPtr( const CtlConstCountedPtr& );

	// Conversion from non-const to const.
	CtlConstCountedPtr( const CtlCountedPtr<T>& );

	~CtlConstCountedPtr();

	CtlConstCountedPtr& operator =( const CtlConstCountedPtr& );

	// These return types are where it differs from the non-const version.
	const T * operator ->() const;
	const T&  operator  *() const;
	// PRE( tPtr_ != NULL );

    bool isDefined() const;
    void clear();

	void destruct();
	// PRE( not isShared() );

	bool isShared() const;

    PER_MEMBER_PERSISTENT_DEFAULT( CtlConstCountedPtr );

private:
	void init();

	typename CtlCountHolder< T >::pointer pCount_;

	// TODO template < typename T >
    friend
    bool operator < < >( const CtlConstCountedPtr< T > a, const CtlConstCountedPtr< T > b );

    //template < class T1 >
    friend
    bool operator == < >( const CtlConstCountedPtr< T > a, const CtlConstCountedPtr< T > b );

    //  These functions are a hack to allow perRead and perWrite
    //  to get at the internals of the class. perRead and perWrite cannot be made
    //  friends because then they will get instantiated for everything
    //  that the counted pointer is defined over. This would then force
    //  us to use only persistent obects with counted pointers - a bad thing.

    friend const typename CtlCountHolder< T >::pointer& ctl_cp_holder( const CtlConstCountedPtr& t )
        { return t.pCount_; }
    friend typename CtlCountHolder< T >::pointer& ctl_cp_holder( CtlConstCountedPtr& t )
        { return t.pCount_; }
};

PER_DEFINE_PERSISTENT_INLINE_T1( CtlConstCountedPtr );

//////////////////////////////////////////////////////////////////////

//#ifdef _INSTANTIATE_TEMPLATE_CLASSES
    #include "ctl/countptr.ctp"
//#endif

//#ifdef _INSTANTIATE_TEMPLATE_FUNCTIONS
    #include "ctl/countptr.ctf"
//#endif

//////////////////////////////////////////////////////////////////////

#endif	/* #ifndef _SYS_COUNTPTR_HPP	*/
