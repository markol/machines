/*
 * P T R V A L I D . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved.
 */

#include "base/tracker.hpp"

template< class T >
bool ctlCountedPtrPointerValid( T* ptr )
{
	bool	result;

	if( ptr == NULL )
	{
		result = true;
	}
	else
	{
		void*	voidPtr = _STATIC_CAST( void*, ptr );

		// cout << "Check pointer " << voidPtr << endl;

		//	The pointer is only valid if it has not already been assigned to
		//  a counted pointer.

		if( DiagInternal::countedPointerTracker().pointerPresent( voidPtr ) )
		{
			result = false;
		}
		else
		{
			result = true;
			DiagInternal::countedPointerTracker().addPointer( voidPtr );
		}
	}

	return result;
}

template< class T >
bool ctlCountedPtrPointerDestructionValid( const T* pT )
{
	bool	result;

	if( pT == NULL )
	{
		result = true;
	}
	else
	{
		void*	voidPtr = _CONST_CAST(void*, _STATIC_CAST( const void*, pT ) );

		if( DiagInternal::countedPointerTracker().pointerPresent( voidPtr ) )
		{
			result = true;
			DiagInternal::countedPointerTracker().removePointer( voidPtr );
		}
		else
		{
			result = false;
		}
	}

	return result;
}

