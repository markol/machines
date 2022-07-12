/*
 * G E N R E P O S . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "world4d/genrepos.hpp"
#include "world4d/root.hpp"
#include "world4d/generic.hpp"
#include "world4d/genecomp.hpp"
#include "world4d/entity.hpp"

#include "mathex/transf3d.hpp"

#ifndef _INLINE
    #include "world4d/genrepos.ipp"
#endif

PER_DEFINE_PERSISTENT( W4dGenericRepository );

W4dGenericRepository::W4dGenericRepository()
:pRoot_ ( _NEW(W4dRoot( W4dRoot::W4dRootId() ) ) )
{
    simpleEntries_.reserve(16);
    compositeEntries_.reserve(16);

    TEST_INVARIANT;
}

W4dGenericRepository::~W4dGenericRepository()
{
	clear();
	_DELETE(pRoot_);

    TEST_INVARIANT;
}

void W4dGenericRepository::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

W4dGenericRepository::KeyType W4dGenericRepository::type( const string& key ) const
{
	KeyType keyType;
    size_t index;

    if( findSimple( key, &index ) )
        keyType = SIMPLE_ENTITY;
    else if( findComposite( key, &index ) )
        keyType = COMPOSITE;
    else
        keyType = UNKNOWN;

	return keyType;
}

const W4dGeneric& W4dGenericRepository::simpleEntity( const string& key )
{
    size_t index;
    bool found = findSimple( key, &index );
    ASSERT( found, "No such entity in repository" );

	return *(simpleEntries_[index].second);
}


const W4dGenericComposite& W4dGenericRepository::compositeEntity( const string& key )
{
    size_t index;
    bool found = findComposite( key, &index );
    ASSERT( found, "No such composite in repository" );

	return *(compositeEntries_[index].second);
}


const W4dEntity& W4dGenericRepository::entity( const string& key )
{
	PRE( type( key ) != UNKNOWN );

    size_t index;
    const W4dEntity* pResult = NULL;

    if( findSimple( key, &index ) )
        pResult = simpleEntries_[index].second;
    else if( findComposite( key, &index ) )
        pResult = compositeEntries_[index].second;

    ASSERT( pResult != NULL, "No such entity" );

    return *pResult;
}


void W4dGenericRepository::readGenericFile( const SysPathName& listFile )
{
    UtlLineTokeniser   parser( listFile );

    while( not parser.finished() )
    {
        uint nTokens = parser.tokens().size();
        ASSERT( nTokens == 2  or  nTokens == 3, "" );

        //Determine solidity - default is solid
        Solidity solid = (nTokens == 3  and  parser.tokens()[2] == "NOT_SOLID" ? NOT_SOLID : SOLID);

		add(parser.tokens()[ 0 ], parser.tokens()[1], solid);
        parser.parseNextLine();
    }
}

void W4dGenericRepository::add( const string& key, const SysPathName& fileName, Solidity solidity )
{
    string extension = fileName.extension();
	PRE( extension == "lod" or extension == "cdf" );
	PRE( type( key ) == UNKNOWN );

    //Convert solidity to W4dEntity version
    W4dEntity::Solidity solid = (solidity == SOLID ? W4dEntity::SOLID : W4dEntity::NOT_SOLID);

	if( extension == "lod" )
	{
		W4dGeneric* pGeneric = _NEW( W4dGeneric(pRoot_, W4dTransform3d(), solid) );
		pGeneric->loadLODFile(fileName);

		add(key, pGeneric);
	}
	else if( extension == "cdf" )
	{
		W4dGenericComposite* pGeneric = _NEW( W4dGenericComposite( pRoot_, W4dTransform3d(),fileName, solid ) );
		add(key, pGeneric);
	}
}

void W4dGenericRepository::takeAllFrom( W4dGenericRepository* pRepository )
{
	uint size = pRepository->simpleEntries_.size();
	uint i;

	for(i=0; i<size; ++i)
	{
		(pRepository->simpleEntries_)[i].second->attachTo(pRoot_);
		simpleEntries_.push_back( (pRepository->simpleEntries_)[i] );
	}

	size = pRepository->compositeEntries_.size();

	for(i=0; i<size; ++i)
	{
		(pRepository->compositeEntries_)[i].second->attachTo(pRoot_);
		compositeEntries_.push_back( (pRepository->compositeEntries_)[i] );
	}

	while (pRepository->simpleEntries_.size() != 0 )
		pRepository->simpleEntries_.pop_back();


	while ( pRepository->compositeEntries_.size() != 0 )
		pRepository->compositeEntries_.pop_back();
}


void W4dGenericRepository::clear()
{
	while( simpleEntries_.size() != 0 )
	{
		_DELETE( simpleEntries_.back().second );
		simpleEntries_.pop_back();
	}

	while( compositeEntries_.size() != 0 )
	{
		_DELETE( compositeEntries_.back().second );
		compositeEntries_.pop_back();
	}
}

void W4dGenericRepository::add( const string& key, W4dGeneric* pGeneric )
{
	pGeneric->attachTo(pRoot_);
	simpleEntries_.push_back(SimpleEntry(key, pGeneric));
}

void W4dGenericRepository::add( const string& key, W4dGenericComposite* pGeneric )
{
	pGeneric->attachTo(pRoot_);
	compositeEntries_.push_back(CompositeEntry(key, pGeneric));
}

uint W4dGenericRepository::nGenerics() const
{
	return simpleEntries_.size() + compositeEntries_.size();
}

const string& W4dGenericRepository::key( uint index ) const
{
	PRE( index < nGenerics() );
	uint nSimples = simpleEntries_.size();
	if ( index < nSimples )
		return simpleEntries_[ index ].first;
	else
		return compositeEntries_[ index - nSimples ].first;
}

ostream& operator <<( ostream& o, const W4dGenericRepository& t )
{

    o << "W4dGenericRepository " << (void*)&t << " start" << std::endl;
    o << "W4dGenericRepository " << (void*)&t << " end" << std::endl;

    return o;
}

W4dEntity* W4dGenericRepository::cloneEntity( const string& key, W4dEntity* pParent, const MexTransform3d& localTransform )
{
    size_t index;
    W4dEntity* pResult = NULL;

    if( findSimple( key, &index ) )
        pResult = _NEW( W4dGeneric( *(simpleEntries_[index].second), pParent, localTransform ) );
    else if( findComposite( key, &index ) )
        pResult = _NEW( W4dGenericComposite( *(compositeEntries_[index].second), pParent, localTransform ) );


    ASSERT( pResult != NULL, "No such entity" );

    return pResult;
}

bool W4dGenericRepository::findSimple( const string& key, size_t* pIndex ) const
{
    size_t size = simpleEntries_.size();
    bool found = false;
	for(size_t i = 0; i < size && not found; ++i)
	{
		if( simpleEntries_[i].first == key )
		{
			found = true;
            *pIndex = i;
		}
	}

    return found;
}

bool W4dGenericRepository::findComposite( const string& key, size_t* pIndex ) const
{
    size_t size = compositeEntries_.size();
    bool found = false;
	for(size_t i = 0; i < size && not found; ++i)
	{
		if( compositeEntries_[i].first == key )
		{
			found = true;
            *pIndex = i;
		}
	}

    return found;
}

W4dGenericRepository::W4dGenericRepository( PerConstructor )
: pRoot_( NULL )
{
}

void perWrite( PerOstream& ostr, const W4dGenericRepository& t )
{
    ostr << t.pRoot_;
    ostr << t.simpleEntries_;
    ostr << t.compositeEntries_;
}

void perRead( PerIstream& istr, W4dGenericRepository& t )
{
    _DELETE( t.pRoot_ );
    istr >> t.pRoot_;
    istr >> t.simpleEntries_;
    istr >> t.compositeEntries_;
}
/* End GENREPOS.CPP *************************************************/
