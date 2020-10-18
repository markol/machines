/*
 * T R A C E . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

#ifndef _PHYS_TRACE_HPP
#define _PHYS_TRACE_HPP

//Macros used to bracket tracing statements,. To turn on tracing compile
//the relevant files with PHYSTRACE_ defined.

#ifdef PHYSTRACE_
#define PHYSTRACE( exp ) { ofstream physos( "phys.log", ios::append ); \
                           physos << exp ; }
#define PHYSTRACE_WHERE  { ofstream physos( "phys.log", ios::append ); \
                           physos << __FILE__ << "(" << __LINE__ << ") " << endl ; }
#else
#define PHYSTRACE( exp )
#define PHYSTRACE_WHERE 
#endif

#endif  /*  _PHYS_TRACE_HPP    */

/* End TRACE.HPP *****************************************************/
