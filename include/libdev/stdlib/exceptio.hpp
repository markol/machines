/*
 * E X C E P T I O . H P P
 * (c) Charybdis Limited, 1995. All Rights Reserved.
 */

#include "base/base.hpp"

#ifndef _STD_EXCEPTIO_HPP
#define _STD_EXCEPTIO_HPP

//////////////////////////////////////////////////////////////////////

typedef void fvoid_t();

void terminate();
fvoid_t *set_terminate( fvoid_t * );

void unexpected();
fvoid_t *set_unexpected( fvoid_t * );

//////////////////////////////////////////////////////////////////////

// xmsg is the base class
// from which all specialised exceptions are derived

class xmsg
{
public:

	xmsg( const char *what = 0 );
                
	xmsg( const xmsg& rhs );

	virtual ~xmsg();
   
	const xmsg& operator =( const xmsg& rhs );

	///////////////////////////////

	virtual const char *what() const;
    
	// handle an unthrowable xmsg
	static void _Throw( xmsg *ePtr );
    
	typedef void (*raise_handler)( xmsg& );
	static raise_handler set_raise_handler( raise_handler newh );

protected:

	xmsg( const char *, bool );

	virtual void do_raise();

private:

	void _Raise();
	void _Tidy();
   
	static raise_handler _Handler;

	const char *	_What;
	bool			_Alloced;
};

//////////////////////////////////////////////////////////////////////

// xmsgs derived from logic_error represents errors which
// can, in principle, be detected and avoided at\before compile-time

class logic_error : public xmsg
{
public:

	logic_error( const char *what = 0 );
	               
	virtual ~logic_error();
    
protected:

	virtual void do_raise();
};

//////////////////////////////////////////////////////////////////////

// domain_error is for reporting precondition violations

class domain_error : public logic_error
{
public:

	domain_error( const char *what = 0 );
               
	virtual ~domain_error();
    
protected:

	virtual void do_raise();
};

//////////////////////////////////////////////////////////////////////

// invalid_argument is the base class for reporting an invalid argument

class invalid_argument : public logic_error
{
public:

	invalid_argument( const char *what = 0 );
               
	virtual ~invalid_argument();
    
protected:

	virtual void do_raise();
};

//////////////////////////////////////////////////////////////////////

// length_error is the base class for reporting an attempt to produce
// an object whose length equals or exceeds NPOS.

class length_error : public logic_error
{
public:

	length_error( const char *what = 0 );
               
	virtual ~length_error();
    
protected:

	virtual void do_raise();
};

//////////////////////////////////////////////////////////////////////

// out_of_range is the base class for reporting out-of-range arguments

class out_of_range : public logic_error
{
public:

	out_of_range( const char *what = 0 );
               
	virtual ~out_of_range();
    
protected:

	virtual void do_raise();
};

//////////////////////////////////////////////////////////////////////

// bad_cast indicates an the execution of an invalid dynamic-cast

class bad_cast : public logic_error
{
public:

	bad_cast( const char *what = 0 );
               
	virtual ~bad_cast();
    
protected:

	virtual void do_raise();
};

//////////////////////////////////////////////////////////////////////

// bad_typeid indicates the invocation of typeid( *p )
// for a null pointer p.

class bad_typeid : public logic_error
{
public:

	bad_typeid( const char *what = 0 );
               
	virtual ~bad_typeid();
    
protected:

	virtual void do_raise();
};

//////////////////////////////////////////////////////////////////////

// xmsgs derived from runtime_error represents errors which
// cannot be detected at compile-time, such as invalid user input
// or running out of heap space

class runtime_error : public xmsg
{
public:

	runtime_error( const char *what = 0 );

	virtual ~runtime_error();

protected:

	runtime_error( const char *what, bool copy );

	void do_raise();
};

//////////////////////////////////////////////////////////////////////

// range_error is for reporting postcondition violations

class range_error : public runtime_error
{
public:
                      
	range_error( const char *what = 0 );

	virtual ~range_error();

protected:

	virtual void do_raise();
};

//////////////////////////////////////////////////////////////////////

// overflow_error is the base class for xmsgs thrown by the
// standard c++ library to indicate an arithmetic overflow

class overflow_error : public runtime_error
{
public:
                      
	overflow_error( const char *what = 0 );
	
	virtual ~overflow_error();

protected:

	virtual void do_raise();
};

//////////////////////////////////////////////////////////////////////

// bad_alloc is the base class for xmsgs
// thrown to report a failure to allocate storage
// errno : ENOMEM

class bad_alloc : public runtime_error
{
public:
                      
	bad_alloc( const char *what = 0 );
	
	virtual ~bad_alloc();

protected:

	virtual void do_raise();
};

//////////////////////////////////////////////////////////////////////

#endif	/* #ifndef _STD_EXCEPTIO_HPP	*/


