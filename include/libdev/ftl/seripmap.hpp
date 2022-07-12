/*
 * S E R I P M A P . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    FtlSerialPMap

    Implements a map from FtlSerialId to pointers to objects of class REF, over
    which the class is parameterized.
*/

#ifndef _FTL_SERIPMAP_HPP
#define _FTL_SERIPMAP_HPP

#include "base/base.hpp"

#include "ftl/map.hpp"

//Orthodox canonical
template< class REF >
class FtlSerialPMap : public FtlMap< FtlSerialId, REF* >
{
public:
    typedef REF Ref;
    typedef Ref* PRef;

    //ctor. Appropriate reserve size for collection is reserveSize.
    FtlSerialPMap( size_t reserveSize );

    //dtor.
    virtual ~FtlSerialPMap() {};

	// inherited from FtlContainer...
	virtual bool contains( const FtlSerialId& id ) const;

	// inherited from FtlAssociativeCollection...
	virtual void add( const FtlPair< FtlSerialId, Ref* >& addMe );
    //PRE( not contains( addMe.first )

    //Adds map from id to pValue
    void add( const FtlSerialId& id, Ref* pValue );
    //PRE( not contains( id )

	virtual void remove( const FtlSerialId& id );
    //PRE( contains( id ) )

    void CLASS_INVARIANT;

protected:

	virtual const PRef& doSubscript( const FtlSerialId& id ) const;
	// PRE( contains( id ) );

private:
    // Operations deliberately revoked
    FtlSerialPMap( const FtlSerialPMap< REF >& );
    FtlSerialPMap< REF >& operator =( const FtlSerialPMap< REF >& );
    bool operator ==( const FtlSerialPMap< REF >& );

    //Data members
    ctl_vector< void* > refs_;
};

//#ifdef _INSTANTIATE_TEMPLATE_CLASSES
    #include "ftl/seripmap.ctp"
//#endif

#ifdef _INLINE
    #include "ftl/seripmap.itp"
#endif


#endif

/* End SERIPMAP.HPP *************************************************/
