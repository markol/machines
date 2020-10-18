/*
 * E X C E P T I O . C P P
 * (c) Charybdis Limited, 1995. All Rights Reserved.
 */

#include <csignal>
#include <cstdlib>
#include <cstring>

#include <iostream>

#include "stdlib/exceptio.hpp"

//////////////////////////////////////////////////////////////////////

xmsg::raise_handler xmsg::_Handler = 0;

//////////////////////////////////////////////////////////////////////

static const char *hs( const char *s )
// copy string to heap
{
    const char *d;
    if( s != 0 )
	{
		d = (const char *)malloc( 1 + strlen( s ) );
		if(  d != 0 )
			return (const char *)strcpy( ( char *)d, s );
	}

	return 0;
}

//////////////////////////////////////////////////////////////////////

xmsg::xmsg( const char * what )
: _What( hs( what ) ),
  _Alloced( 1 )
{
    /* Intentionally Empty	*/
}

xmsg::xmsg( const char *what, bool copyfl )
: _What( copyfl ? hs( what ) : what ),
  _Alloced( copyfl )
{
    /* Intentionally Empty	*/
}

xmsg::xmsg( const xmsg& rhs )
: _What( hs( rhs.what() ) ),
  _Alloced( 1 )
{
    /* Intentionally Empty	*/
}

xmsg::~xmsg()
{
    _Tidy();
}

const xmsg& xmsg::operator =( const xmsg& rhs )
{
    if( this == &rhs )
        return *this;

    _Tidy();
    _What = hs( rhs.what() );
    _Alloced = 1;

    return *this;
}

//////////////////////////////////////////////////////////////////////

void xmsg::_Raise()
{
    if( _Handler != 0 )
        (*_Handler)( *this );

    do_raise();

    _RAISE( *this );
}

//////////////////////////////////////////////////////////////////////

const char *xmsg::what() const
{
    return _What != 0 ? _What : "";
}

void xmsg::_Throw( xmsg *ePtr )
// handle an unthrowable xmsg
{
    std::cerr << "xmsg: " << ( ePtr ? ePtr->what() : "unknown" );
    std::cerr << std::endl;
    ::raise( SIGABRT );
}

xmsg::raise_handler xmsg::set_raise_handler( xmsg::raise_handler newh )
{
    const raise_handler oldh = xmsg::_Handler;
    xmsg::_Handler = newh;
    return oldh;
}

void xmsg::do_raise()
{
	/* no special handling by default */
}

void xmsg::_Tidy()
{
    if( _Alloced )
    {
        free( (void *)_What );
        _Alloced = 0;
    }
}

//////////////////////////////////////////////////////////////////////

logic_error::logic_error( const char *what )
: xmsg( what )
{
    /* Intentionally Empty	*/
}

logic_error::~logic_error()
{
	/*  Intentionally Empty  */
}

void logic_error::do_raise() { _RAISE( *this ); }

//////////////////////////////////////////////////////////////////////

bad_cast::bad_cast( const char *what )
: logic_error( what )
{
    /* Intentionally Empty	*/
}

bad_cast::~bad_cast() {   /*  Intentionally Empty  */ }

void bad_cast::do_raise() { _RAISE( *this ); }

//////////////////////////////////////////////////////////////////////

bad_typeid::bad_typeid( const char *what )
: logic_error( what )
{
    /* Intentionally Empty	*/
}

bad_typeid::~bad_typeid() {   /*  Intentionally Empty  */ }

void bad_typeid::do_raise() { _RAISE( *this ); }

//////////////////////////////////////////////////////////////////////

invalid_argument::invalid_argument( const char *what )
: logic_error( what )
{
    /* Intentionally Empty	*/
}

invalid_argument::~invalid_argument()
{
	/*  Intentionally Empty  */
}

void invalid_argument::do_raise() { _RAISE( *this ); }

//////////////////////////////////////////////////////////////////////

length_error::length_error( const char *what )
: logic_error( what )
{
    /* Intentionally Empty	*/
}

length_error::~length_error() {   /*  Intentionally Empty  */ }

void length_error::do_raise() { _RAISE( *this ); }

//////////////////////////////////////////////////////////////////////

out_of_range::out_of_range( const char *what )
: logic_error( what )
{
    /* Intentionally Empty	*/
}

out_of_range::~out_of_range() {   /*  Intentionally Empty  */ }

void out_of_range::do_raise() { _RAISE( *this ); }

//////////////////////////////////////////////////////////////////////

domain_error::domain_error( const char *what )
: logic_error( what )
{
    /* Intentionally Empty	*/
}

domain_error::~domain_error() {   /*  Intentionally Empty  */ }

void domain_error::do_raise() { _RAISE( *this ); }

//////////////////////////////////////////////////////////////////////

runtime_error::runtime_error( const char *what )
: xmsg( what )
{
    /* Intentionally Empty	*/
}

runtime_error::runtime_error( const char *what,	bool copy )
: xmsg( what, copy )
{
    /* Intentionally Empty	*/
}

runtime_error::~runtime_error()
{
    /*  Intentionally Empty */
}

void runtime_error::do_raise()
{
    _RAISE( *this );
}

//////////////////////////////////////////////////////////////////////

overflow_error::overflow_error( const char *what )
: runtime_error( what )
{
    /* Intentionally Empty	*/
}

overflow_error::~overflow_error()
{
    /*  Intentionally Empty */
}

void overflow_error::do_raise()
{
    _RAISE( *this );
}

//////////////////////////////////////////////////////////////////////

range_error::range_error( const char *what )
: runtime_error( what )
{
    /* Intentionally Empty	*/
}

range_error::~range_error() { /*  Intentionally Empty */ }

void range_error::do_raise() {_RAISE( *this ); }

//////////////////////////////////////////////////////////////////////

bad_alloc::bad_alloc( const char *what )
: runtime_error( what )
{
    /* Intentionally Empty	*/
}

bad_alloc::~bad_alloc() { /*  Intentionally Empty */ }

void bad_alloc::do_raise() { _RAISE( *this ); }

//////////////////////////////////////////////////////////////////////

static fvoid_t *terminate_handler = &abort;

fvoid_t *set_terminate( fvoid_t *newh )
{
    fvoid_t *oldh = terminate_handler;
    terminate_handler = newh;
    return oldh;
}

void terminate()
// call terminate handler or abort
{
    if( terminate_handler != 0 )
        (*terminate_handler)();

    abort();
}

//////////////////////////////////////////////////////////////////////

static fvoid_t *unexpected_handler = &terminate;

fvoid_t *set_unexpected( fvoid_t *newh )
{
    fvoid_t *oldh = unexpected_handler;
    unexpected_handler = newh;
    return oldh;
}

void unexpected()
// call unexpected handler or terminate
{
    if( unexpected_handler != 0 )
        (*unexpected_handler)();

    terminate();
}

/* End EXCEPTIO.CPP *************************************************/

