/*
 * G E N R E P O S . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    W4dGenericRepository

    A repository for W4dGeneric and W4dGenericComposite exemplar models which can
    be accessed via a string key.
*/

#ifndef _WORLD4D_GENREPOS_HPP
#define _WORLD4D_GENREPOS_HPP

#include "stdlib/utility.hpp"
#include "stdlib/string.hpp"

#include "base/base.hpp"
#include "base/persist.hpp"
#include "ctl/vector.hpp"
#include "utility/linetok.hpp"

class MexTransform3d;
class W4dRoot;
class SysPathName;
class W4dGenericComposite;
class W4dGeneric;
class W4dEntity;

class W4dGenericRepository
// Canonical form revoked
{
public:
    W4dGenericRepository();

    ~W4dGenericRepository();

    //Can be used to determine whether a generic with the specified key is registered,
    //and if so whether it is a W4dGeneric or a W4dGenericComposite.
    enum KEY_TYPE { UNKNOWN, SIMPLE_ENTITY, COMPOSITE };
	typedef KEY_TYPE KeyType;

    KeyType type( const string& key ) const;

    //The generic mapped from key
    const W4dGeneric& simpleEntity( const string& key );
    //PRE( type( key ) == SIMPLE_ENTITY );

    //The generic composite mapped from key
    const W4dGenericComposite& compositeEntity( const string& key );
    //PRE( type( key ) == COMPOSITE );

    //The base class view of the entity mapped from key
    const W4dEntity& entity( const string& key );
    //PRE( type( key ) != UNKNOWN );

    //Construct and return a new entity copy of the appropriate kind (W4dGeneric or W4dComposite)
    //from the mapped key. Created at locaTransform owned by pParent.
    W4dEntity* cloneEntity( const string& key, W4dEntity* pParent, const MexTransform3d& localTransform );
    //PRE( type( key ) != UNKNOWN );

    //Reads a text file containing pairs of string and lod/cdf pathnames, one per line as shown below.
    //Each lod file adds a W4dGeneric to the repository, and each cdf file
    //adds a W4dGenericComposite.
    //File format example:
    //   tree1 models/planet/dales/artefacts/tree1.lod NOT_SOLID
    //   generator scenario/gubby/artefacts/generat.cdf SOLID
    void readGenericFile( const SysPathName& listFile );

    //loads the model defined by lod or cdf file fileName, using the key specified
    enum Solidity{ SOLID, NOT_SOLID };
    void add( const string& key, const SysPathName& fileName, Solidity solidity );
    //PRE( fileName.extensions() == "lod" or fileName.extensions() == "cdf" );
    //PRE( type( key ) == UNKNOWN )

    //Adds pGeneric to the repository, becoming the owner thereof (also detaching it from
    //any tree of which it is a member), under the specified key.
    void add( const string& key, W4dGeneric* pGeneric );

    //Adds pGeneric to the repository, becoming the owner thereof (also detaching it from
    //any tree of which it is a member), under the specified key.
    void add( const string& key, W4dGenericComposite* pGeneric );

    //Takes all the entries from pRepository and adds them to this repository.
    //Leaves pRepository empty.
    void takeAllFrom( W4dGenericRepository* pRepository );

    //Deletes all the generic entries from the repository
    void clear();

	//Returns the total number of W4dGeneric and W4dGenericComposite objects.
	uint nGenerics() const;

	//Returns the key string associated with the Generic object at index
	//( treating both lists as one continous list )
	const string& key( uint index ) const;
	//PRE( index < nGenerics() )

    void CLASS_INVARIANT;

    PER_MEMBER_PERSISTENT( W4dGenericRepository );
    PER_FRIEND_READ_WRITE( W4dGenericRepository );

private:
    friend ostream& operator <<( ostream& o, const W4dGenericRepository& t );

    W4dGenericRepository( const W4dGenericRepository& );
    W4dGenericRepository& operator =( const W4dGenericRepository& );

    typedef std::pair< string, W4dGeneric* > SimpleEntry;
    typedef std::pair< string, W4dGenericComposite* > CompositeEntry;

    typedef ctl_vector< SimpleEntry > SimpleEntries;
    typedef ctl_vector< CompositeEntry > CompositeEntries;

    //True iff the list of appropriate type contains an entry for key, and if so
    //returns its index in pIndex.
    bool findSimple( const string& key, size_t* pIndex ) const;
    bool findComposite( const string& key, size_t* pIndex ) const;

    //data members
    W4dRoot* pRoot_; //root used to own all the generics
    SimpleEntries simpleEntries_; //The table of W4dGenerics
    CompositeEntries compositeEntries_; //The table of W4dGenericComposites
};

#ifdef _INLINE
    #include "world4d/genrepos.ipp"
#endif

PER_DECLARE_PERSISTENT( W4dGenericRepository );


#endif

/* End GENREPOS.HPP *************************************************/
