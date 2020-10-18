/*
 * F A C T O R Y . H P P
 * (c) Charybdis Limited, 1995. All Rights Reserved
 */

/*
    Implements a default factory where the part is created from one
    argument - the id - and all parts will be owned by the factory
    and deleted on factory destruction. If anything more sophisticated
    is required use UtlBasicFactory.
*/

#ifndef _UTL_FACTORY_HPP
#define	_UTL_FACTORY_HPP

#include "ctl/map.hpp"

//////////////////////////////////////////////////////////////////////

template < class ID, class PART >
class UtlFactory
{
public:

	UtlFactory();
	virtual ~UtlFactory();

	virtual PART& part( const ID& );

	bool containsPart( const ID& ) const;

	void deletePart( const ID& id );
	// PRE( containsPart( id ) );

protected:

	typedef std::pair< const ID, PART* >				PartPair;
	typedef ctl_map< ID, PART*, std::less< ID > >	PartMap;

	PartMap& partMap();
	const PartMap& partMap() const;

private:

	UtlFactory( const UtlFactory& );
	const UtlFactory& operator =( const UtlFactory& );
	bool operator ==( const UtlFactory& ) const;

	PartMap		partMap_;
};

//////////////////////////////////////////////////////////////////////

//#ifdef _INSTANTIATE_TEMPLATE_CLASSES
    #include "utility/factory.ctp"
//#endif

//////////////////////////////////////////////////////////////////////

#endif	/* #ifndef _SYS_FACTORY_HPP */
