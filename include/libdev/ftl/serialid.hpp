/*
 * S E R I A L I D . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    FtlSerialId

    Implements an id class for ids generated serially.
    Next and previous ids in the sequence defined by the ++ and -- operators.
    Number of unique ids available is limited to distinct values of a unsigned long.
    The value of an id can be obtained as a size_t for vector indexing, etc.
*/

#ifndef _FTL_SERIALID_HPP
#define _FTL_SERIALID_HPP

#include "base/base.hpp"

//Memberwise canonical
class FtlSerialId
{
public:
    //The type used for implementation
    typedef unsigned long RepType;

    //ctors
    FtlSerialId( void )
        : id_( 0 ) {};

    FtlSerialId( const RepType& value )
        : id_( value ) {};

    FtlSerialId( const FtlSerialId& id )
        : id_( id.asScalar() ) {};

    FtlSerialId& operator =( const FtlSerialId& rhs )
        { id_ = rhs.id_; return *this; };

    //Next in sequence
    FtlSerialId& operator++ () { ++id_; return *this; };

    //Previous in sequence
    FtlSerialId& operator-- () { --id_; return *this;  };

    //Id in format suitable for indexing vector etc
    size_t asScalar( void ) const { return id_; };

    //An invalid id.
    static FtlSerialId invalidId( void ) { return FtlSerialId( 0xFFFFFFFF ); };

    friend ostream& operator <<( ostream& o, const FtlSerialId& t )
    {
        return o << t.asScalar();
    };

    //Comparators
    bool operator< ( const FtlSerialId& rhs )  const { return id_ <  rhs.id_; };
    bool operator== ( const FtlSerialId& rhs ) const { return id_ == rhs.id_; };
    bool operator!= ( const FtlSerialId& rhs ) const { return id_ != rhs.id_; };

private:
    RepType id_; // The id
};

//////////////////////////////////////////////////////////////////////////////////////////

//Related class used to generate sequence of ids
class FtlSerialIdGenerator
{
public:
    //ctor start ids at zero
    FtlSerialIdGenerator( void ) : nextId_( 0 ) {};

    //Generate and return next id in sequence    
    FtlSerialId next( void )
    {
        FtlSerialId id = nextId_;
        ++nextId_;
        return id;
    };

    //First/last+1 generated ids
    FtlSerialId begin( void ) const { return FtlSerialId( 0 ); };
    const FtlSerialId& end( void ) const { return nextId_; };

private:
    FtlSerialId nextId_; //The next id to use
};
//////////////////////////////////////////////////////////////////////////////////////////

#endif

/* End SERIALID.HPP *************************************************/
