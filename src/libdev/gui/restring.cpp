/*
 * R E S T R I N G . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

#include "gui/restring.hpp"
#include "ctl/vector.hpp"

#include <ctype.h>

#include "afx/resource.hpp"

//////////////////////////////////////////////////////////////////////

GuiResourceString::GuiResourceString( Id id )
{
	PRE( hasResource() );

	insertionString_ = GuiResourceString::map_Id_to_string( id );

    TEST_INVARIANT;
}

GuiResourceString::GuiResourceString( Id id, const GuiString& str )
{
    PRE( hasResource() );

	insertionString_ = GuiResourceString::map_Id_to_string( id );
	GuiStrings inserts( 1, str );
	insert( inserts );

    TEST_INVARIANT;
}

GuiResourceString::GuiResourceString( Id id, const GuiStrings& inserts )
{
    PRE( hasResource() );

	insertionString_ = GuiResourceString::map_Id_to_string( id );
	insert( inserts );


	TEST_INVARIANT;
}

//////////////////////////////////////////////////////////////////////

void GuiResourceString::insert( const GuiStrings& inserts )
// <ins str> ::= { <esc seq> | <non-percent-character> }
// <esc seq> ::= <number esc seq> | <percent esc seq>
// <percent esc seq> ::= <%> <%>
// <number esc seq> ::=  <%> <number> [ <%> ]
// <number>	 		::= <leadingdigit> { <digit> }
// <leadingdigit>	::= <1> - <9>
// <digit>			::= <0> - <9>
{
	TEST_INVARIANT;

	for( size_t i=0; i<insertionString_.length(); ++i )
	{
		if( insertionString_[ i ] == '%' )
		{
			size_t j = 1;
			if( i + j == insertionString_.length() )
				break;
			else if( insertionString_[ i + j ] == '%' )
				//insertionString_.remove( i, 1 );
				insertionString_.erase( i, 1 );
			else
			{
				unsigned n = 0;
				while( i + j < insertionString_.length() and isdigit( insertionString_[ i + j ] ) )
				{
					n *= 10;
					n += insertionString_[ i + j ] - '0';
					++j;
				}

				if( i + j < insertionString_.length() and insertionString_[ i + j ] == '%' )
					++j;

				//insertionString_.remove( i, j );
				insertionString_.erase( i, j );

				if( n <= inserts.size() )
					insertionString_.insert( i, inserts[ n - 1 ] );
			}
		}
	}

    TEST_INVARIANT;
}

const GuiString&
GuiResourceString::asString() const
{
    return insertionString_;
}

//////////////////////////////////////////////////////////////////////

void GuiResourceString::CLASS_INVARIANT
{
}

ostream& operator <<( ostream& o, const GuiResourceString& t )
{
    o << "GuiResourceString " << (void*)&t << " start" << std::endl;
    o << "GuiResourceString " << (void*)&t << " end" << std::endl;
    return o;
}

//////////////////////////////////////////////////////////////////////

// static
GuiString
GuiResourceString::map_Id_to_string( Id id )
{
	PRE( hasResource() );
	GuiString result = resource().getString( id );
	POST( isInsertionString( result ) );
	return result;
}

// static
bool GuiResourceString::hasResource()
{
	return pResource() != NULL;
}

// static
AfxResourceLib& GuiResourceString::resource()
{
    PRE( hasResource() );
	return *pResource();
}

// static
void GuiResourceString::resource( AfxResourceLib* pLib )
{
	PRE( pLib != NULL );
	pResource() = pLib;
	POST( hasResource() );
}

// static
void GuiResourceString::clearResource()
{
    pResource() = NULL;
	POST( not hasResource() );
}

// static
GuiResourceString::ResourcePtr&
GuiResourceString::pResource()
{
	static AfxResourceLib * pResult_ = NULL;
	return pResult_;
}

// debug-only functions //////////////////////////////////////////////

#ifndef NDEBUG

// static
bool GuiResourceString::isInsertionString( const GuiString& insertionString )
{
// <ins str> ::= { <esc seq> | <non-percent-character> }
// <esc seq> ::= <number esc seq> | <percent esc seq>
// <percent esc seq> ::= <%> <%>
// <number esc seq> ::=  <%> <number> [ <%> ]
// <number>	 		::= <leadingdigit> { <digit> }
// <leadingdigit>	::= <1> - <9>
// <digit>			::= <0> - <9>

	bool valid = true;
	for( size_t i=0; i<insertionString.length() and valid; ++i )
	{
		if( insertionString[ i ] == '%' )
		{
			++i;
			if( i == insertionString.length() )
				valid = false;
			else if( insertionString[ i ] == '%' )
				++i;
			else
			{
				valid = isdigit( insertionString[ i ] ) and insertionString[ i ] != '0';

				if( valid )
				{
					++i;
					while( i<insertionString.length() and isdigit( insertionString[ i ] ) )
						++i;

					if( i < insertionString.length() and insertionString[ i ] == '%' )
						++i;
				}
			}
		}
	}

	return valid;
}

#endif // #ifndef NDEBUG

// end debug-only functions //////////////////////////////////////////

/* End RESTRING.CPP *************************************************/
