/*
 * R E S T R I N G . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
 * GuiResourceString
 * Supports loading of internationalized strings from a resource file.
 */

#ifndef _GUI_RESTRING_HPP
#define _GUI_RESTRING_HPP

#include "base/base.hpp"
#include "stdlib/string.hpp"

//////////////////////////////////////////////////////////////////////

class AfxResourceLib;
template < class T > class ctl_vector;


// TBD: support for extended character sets, double-byte character sets, Unicode etc.

typedef string GuiString;
typedef ctl_vector< GuiString > GuiStrings;

class GuiResourceString
// memberwise cannonical
{
public:

	typedef unsigned Id;

    // loading string with id from current resource file
    GuiResourceString( Id id );
    // PRE( hasResource() );

    // as above, replacing all occurrences of %1 with insert
    GuiResourceString( Id id, const GuiString& insert );
    // PRE( hasResource() );

    // as above, replacing all occurrences of %1 with insert[ 0 ],
    // %2 with insert[ 1 ] etc
    GuiResourceString( Id id, const GuiStrings& insert );
    // PRE( hasResource() );

	const GuiString& asString() const;

	////////////////////////////

	static bool isInsertionString( const GuiString& );
	// <ins str> ::= { <esc seq> | <non-percent-character> }
	// <esc seq> ::= <number esc seq> | <percent esc seq>
	// <percent esc seq> ::= <%> <%>
	// <number esc seq> ::=  <%> <number> [ <%> ]
	// <number>	 		::= <leadingdigit> { <digit> }
	// <leadingdigit>	::= <1> - <9>
	// <digit>			::= <0> - <9>

	static bool hasResource();

    static AfxResourceLib& resource();
	// PRE( hasResource() );

    static void resource( AfxResourceLib * pLib );
	// PRE( pLib != NULL );
	// POST( hasResource() );

    static void clearResource();
    // POST( not hasResource() )
	
	////////////////////////////

    void CLASS_INVARIANT;

private:

	typedef AfxResourceLib * ResourcePtr;
	static ResourcePtr& pResource();
	static GuiString map_Id_to_string( Id );
	// PRE( hasResource() );
	// POST( isInsertionString( result ) );

	void insert( const GuiStrings& );

    GuiString	insertionString_;

    friend ostream& operator <<( ostream& o, const GuiResourceString& t );
};

//////////////////////////////////////////////////////////////////////

#endif

/* End RESTRING.HPP *************************************************/
