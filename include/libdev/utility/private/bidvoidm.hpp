/*
 * B I D V O I D M . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    UtlBoundedIdVoidPMap

    Supporting implementating class for UtlBoundedIdPMap.
    Implements map from bounded id to a void*.
*/

#ifndef _UTILITY_BIDVOIDM_HPP
#define _UTILITY_BIDVOIDM_HPP

#include "base/base.hpp"

#include "utility/id.hpp"


//Forward declarations
template < class X > class UtlBoundedIdPMap;
class MachActor;

//orthodox canonical (revoked)
class UtlBoundedIdVoidPMap
{
public:
    //dtor
    ~UtlBoundedIdVoidPMap();

    //The fixed id upper bound
    UtlId upperBound() const;

    //True if there is an entry for id.
    bool contains( UtlId id ) const;
    //PRE( id < upperBound() );

    //Remove map entry for id
    void remove( UtlId id );
    //PRE( id < upperBound() )
    //PRE( contains( id ) )

private:
    //friend class UtlBoundedIdPMap;
    friend class UtlBoundedIdPMap< class MachActor >;

    //ctor. upperBound sets limit on key ids.
    UtlBoundedIdVoidPMap( UtlId upperBound );

    //returns the pointer mapped from id
    void* entry( UtlId id ) const;
    //PRE( id < upperBound() )
    //PRE( contains( id ) )

    //Add map from id to p
    void add( UtlId id, void* p );
    //PRE( id < upperBound() )
    //PRE( not contains( id ) )

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const UtlBoundedIdVoidPMap& t );

    // Operations deliberately revoked
    UtlBoundedIdVoidPMap( const UtlBoundedIdVoidPMap& );
    UtlBoundedIdVoidPMap& operator =( const UtlBoundedIdVoidPMap& );
    bool operator ==( const UtlBoundedIdVoidPMap& );

    //Data members
    UtlId upperBound_; //The limit
    void** aPointers_; //Array of mapped to pointers
};

#ifdef _INLINE
    #include "utility/private/bidvoidm.ipp"
#endif


#endif

/* End BIDVOIDM.HPP *************************************************/
