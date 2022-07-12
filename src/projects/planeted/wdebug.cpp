/*
 * W D E B U G . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "planeted/wdebug.hpp"
#include "world4d/scenemgr.hpp"

wdebug::wdebug( W4dSceneManager* pSourceSceneManager )
{
	PRE ( pSourceSceneManager != NULL );

	pSceneManager_ = pSourceSceneManager;

    TEST_INVARIANT;
}

wdebug::~wdebug()
{
    TEST_INVARIANT;

}

void wdebug::add( const string& error )
{
	if ( errors.size() == 4 )
	{
		errorlist::iterator i = errors.begin();
		errors.erase( i );
	}
	errors.push_back( error );
}

void wdebug::display()
{
	const char* tab = "\t";

	for ( errorlist::iterator i = errors.begin(); i != errors.end(); i++ )
		pSceneManager_->out() << tab << *i << std::endl;
}

void wdebug::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
	INVARIANT( pSceneManager_ != NULL );
}

ostream& operator <<( ostream& o, const wdebug& t )
{

    o << "wdebug " << (void*)&t << " start" << std::endl;
    o << "wdebug " << (void*)&t << " end" << std::endl;

    return o;
}

/* End WDEBUG.CPP ***************************************************/
