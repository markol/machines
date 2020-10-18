/*
 * F A C T O R Y . H P P
 * (c) Charybdis Limited, 1995. All Rights Reserved
 */

#ifndef _UTL_CPFACTORY_HPP
#define	_UTL_CPFACTORY_HPP

#include "ctl/map.hpp"

//////////////////////////////////////////////////////////////////////
// PART is assumed to be a counted pointer type, thus there is no need
// to explicitly destroy it.  Destroying the pointer automatically
// destroys what is pointed to.
template < class ID, class PART >
class UtlCPFactory
{
public:

	UtlCPFactory();
	virtual ~UtlCPFactory();

	virtual PART part( const ID& );

	bool containsPart( const ID& ) const;

	void deletePart( const ID& id );
	// PRE( containsPart( id ) );

protected:

	typedef std::pair< const ID, PART >				PartPair;
	typedef ctl_map< ID, PART, std::less< ID > >		PartMap;

	virtual PART doCreatePart( const ID& ) = 0;

	PartMap& partMap();
	const PartMap& partMap() const;

private:

	UtlCPFactory( const UtlCPFactory& );
	const UtlCPFactory& operator =( const UtlCPFactory& );
	bool operator ==( const UtlCPFactory& ) const;

	PartMap		partMap_;
};

//////////////////////////////////////////////////////////////////////

//#ifdef _INSTANTIATE_TEMPLATE_CLASSES
    #include "render/internal/cpfactry.ctp"
//#endif

//////////////////////////////////////////////////////////////////////

#endif	/* #ifndef _SYS_FACTORY_HPP */
