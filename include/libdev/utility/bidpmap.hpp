/*
 * B I D P M A P . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    UtlBoundedIdPMap

    Implements a map from a UtlId to a pointer to object of parameter class,
    given an upper bound on the value of the id key.
*/

#ifndef _UTILITY_BIDPMAP_HPP
#define _UTILITY_BIDPMAP_HPP

#include "base/base.hpp"
#include "utility/private/bidvoidm.hpp"

//orthodox canonical (revoked)
template< class X >
class UtlBoundedIdPMap : public UtlBoundedIdVoidPMap
{
public:
    //ctor. Key ids must be < upperBound.
    UtlBoundedIdPMap( UtlId upperBound );

    //dtor.
    ~UtlBoundedIdPMap( void );

    //////////////////////////////////////
    //Inherited from implementation class
    //The fixed id upper bound
    //UtlId upperBound() const;

    //True if there is an entry for id.
    //bool contains( UtlId id ) const;
    //PRE( id < upperBound() );

    //Remove map entry for id
    //void remove( UtlId id );
    //PRE( id < upperBound() )
    //PRE( contains( id ) )
    //////////////////////////////////////

    //returns the pointer mapped from id
    X* operator[] ( UtlId id ) const;
    //PRE( id < upperBound() )
    //PRE( contains( id ) )

    //Add map from id to p
    void add( UtlId id, X* p );
    //PRE( id < upperBound() )
    //PRE( not contains( id ) )

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const UtlBoundedIdPMap< X >& t );

private:
    // Operation deliberately revoked
    UtlBoundedIdPMap( const UtlBoundedIdPMap< X >& );
    UtlBoundedIdPMap< X >& operator =( const UtlBoundedIdPMap< X >& );
    bool operator ==( const UtlBoundedIdPMap< X >& );

};

//#ifdef _INSTANTIATE_TEMPLATE_CLASSES
    #include "utility/bidpmap.ctp"
//#endif

#ifdef _INLINE
    #include "utility/bidpmap.itp"
#endif


#endif

/* End BIDPMAP.HPP **************************************************/
