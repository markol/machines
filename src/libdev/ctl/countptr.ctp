/*
 * C O U N T P T R . C T P
 * (c) Charybdis Limited, 1996. All Rights Reserved.
 */

//////////////////////////////////////////////////////////////////////

#include "base/lessptr.hpp"

#include "ctl/private/ptrvalid.hpp"

template < class T >
CtlCountedPtr< T >::CtlCountedPtr( T * pT )
: pCount_( pT ? _NEW( CtlCountHolder< T > ) : NULL )
{
    PRE_INFO( _STATIC_CAST( void*, pT ) );
    ASSERT( implies( DiagInternal::checkCountedPointers_, ctlCountedPtrPointerValid( pT ) ),
        "Raw pointer has already been assigned to a counted pointer" );

    if( pCount_ )
    	pCount_->pT_ = pT;

	init();
}

template < class T >
CtlCountedPtr< T >::CtlCountedPtr( const CtlCountedPtr& rhs )
: pCount_( rhs.pCount_ )
{
	init();
}

template < class T >
CtlCountedPtr< T >::~CtlCountedPtr()
{
	if( pCount_ != NULL )
		pCount_->removeReference();
}

template < class T >
CtlCountedPtr< T >& 
CtlCountedPtr< T >::operator =( const CtlCountedPtr& rhs )
{
	if( pCount_ != rhs.pCount_ )
	{
		if( pCount_ != NULL )
			pCount_->removeReference();

		pCount_ = rhs.pCount_;
		init();
	}

	return *this;
}

template < class T >
T * CtlCountedPtr< T >::operator ->() const
{
	PRE( pCount_ != NULL );
	return pCount_->pT_;
}

template < class T >
T&  CtlCountedPtr< T >::operator  *() const
{
	PRE( pCount_ != NULL );
	PRE( pCount_->pT_ != NULL );

	return *(pCount_->pT_);
}

template < class T >
void CtlCountedPtr< T >::init()
{
	if( pCount_ != NULL )
	{
		pCount_->addReference();
	}
}

template < class T >
CtlCountHolder< T >::CtlCountHolder()
{
}

template < class T >
CtlCountHolder< T >::~CtlCountHolder()
{
    PRE_INFO( _STATIC_CAST( void*, pT_ ) );

    ASSERT( implies( DiagInternal::checkCountedPointers_, ctlCountedPtrPointerDestructionValid( pT_ ) ),
        "Raw pointer has already been deleted by another counted pointer" );

	_DELETE( pT_ ); 
}

template < class T >
bool CtlCountedPtr< T >::isShared() const
{
	return (pCount_ != NULL ? pCount_->isShared() : false);
}

template < class T >
bool CtlCountedPtr< T >::isDefined() const
{
	return (pCount_ != NULL  and  pCount_->pT_ != NULL);
}

template < class T >
void CtlCountedPtr< T >::clear()
{
	PRE( pCount_ != NULL );

	pCount_->removeReference();
	pCount_ = NULL;
}

//////////////////////////////////////////////////////////////////////

template < class T >
CtlConstCountedPtr< T >::CtlConstCountedPtr( T * pT )
: pCount_( pT ? _NEW( CtlCountHolder< T > ) : NULL )
{
    PRE_INFO( _STATIC_CAST( void*, pT ) );
    ASSERT( implies( DiagInternal::checkCountedPointers_, ctlCountedPtrPointerValid( pT ) ),
        "Raw pointer has already been assigned to a counted pointer" );

    if( pCount_ )
    	pCount_->pT_ = pT;
	init();
}

template < class T >
CtlConstCountedPtr< T >::CtlConstCountedPtr( const CtlConstCountedPtr& rhs )
: pCount_( rhs.pCount_ )
{
	init();
}

template < class T >
CtlConstCountedPtr< T >::CtlConstCountedPtr( const CtlCountedPtr<T>& rhs )
: pCount_( rhs.pCount_ )
{
	init();
}

template < class T >
CtlConstCountedPtr< T >::~CtlConstCountedPtr()
{

	if( pCount_ != NULL )
		pCount_->removeReference();
}

template < class T >
CtlConstCountedPtr< T >& 
CtlConstCountedPtr< T >::operator =( const CtlConstCountedPtr& rhs )
{
	if( pCount_ != rhs.pCount_ )
	{
		if( pCount_ != NULL )
			pCount_->removeReference();

		pCount_ = rhs.pCount_;
		init();
	}

	return *this;
}

template < class T >
const T* CtlConstCountedPtr< T >::operator ->() const
{
	PRE( pCount_ != NULL );
	return pCount_->pT_;
}

template < class T >
const T&  CtlConstCountedPtr< T >::operator  *() const
{
	PRE( pCount_ != NULL );
	PRE( pCount_->pT_ != NULL );
	return *(pCount_->pT_);
}

template < class T >
void CtlConstCountedPtr< T >::init()
{
	if( pCount_ != NULL )
	{
		// if( not pCount_->isShareable() )
		// {
		//	pCount_ = new CountHolder;
		//	pCount_->pT_ = new T( *pCount_->pT_ );
		// }

		pCount_->addReference();
	}
}

template < class T >
bool CtlConstCountedPtr< T >::isShared() const
{
	return (pCount_ != NULL ? pCount_->isShared() : false);
}

template < class T >
bool CtlConstCountedPtr< T >::isDefined() const
{
	return (pCount_ != NULL  and  pCount_->pT_ != NULL);
}

template < class T >
void CtlConstCountedPtr< T >::clear()
{
	PRE( pCount_ != NULL )

	pCount_->removeReference();
	pCount_ = NULL;
}

/*********************************************************************

template < class T >
T&  CtlConstCountedPtr< T >::dereference( const char *file, const char *line ) const
{
	PRE( pCount_ != NULL );
	if( pCount_->pT_ == NULL )
	{
		cout << "trying to dereference null smart pointer" << endl;
		cout << file << " " << line << endl;
		assert( pCount_->pT_ != NULL, logic_error() );
	}

	return *(pCount_->pT_);
}

template < class T >
T * 
CtlConstCountedPtr< T >::get() const 
{ 
	PRE( pCount_ != NULL );
	return pCount_->pT_; 
}

template < class T >
void CtlConstCountedPtr< T >::destruct()
{
	PRE( pCount_ != NULL );
	PRE( not pCount_->isShared() );
	pCount_->removeReference();
	pCount_ = NULL;
}

template < class T >
bool CtlConstCountedPtr< T >::isShared() const
{
	PRE( pCount_ != NULL );
	return pCount_->isShared();
}


*********************************************************************/

//////////////////////////////////////////////////////////////////////

/* End COUNTPTR.CTP *************************************************/
