/*
 * C O N T A I N . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved.
 */

#ifndef _FTL_CONTAIN_HPP
#define _FTL_CONTAIN_HPP

/* //////////////////////////////////////////////////////////////// */

// FtlContainer is the base class of the Ftl Container
// heirarchy.

template <	class KEY >
class FtlContainer
// cannonical form revoked
{
public:

	typedef KEY		Key;
	
	FtlContainer() {};

	virtual ~FtlContainer() {};

	virtual bool contains( const Key& key ) const = 0;

protected:

	FtlContainer( const FtlContainer& );
	FtlContainer& operator =( const FtlContainer& );	
	bool operator ==( const FtlContainer& );
};

/* //////////////////////////////////////////////////////////////// */

#endif	/* #ifndef _FTL_CONTAIN_HPP	*/

/* End CONTAIN.HPP **************************************************/