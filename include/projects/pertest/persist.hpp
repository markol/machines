/*
 * P E R S I S T . H P P 
 * (c) Charybdis Limited, 1997, 1998. All Rights Reserved
 */

/*
    Persistence

	Macros needed to add persistence to a class or group of classes.
*/

#ifndef _BASE_PERSIST_HPP
#define _BASE_PERSIST_HPP

#include "pertest/private/persist.hpp"
// #include "base/private/persist.hpp"

//	Use PerOstream as your persistent output stream, PerIstream as the input stream.

#include "base/ostr.hpp"
#include "base/istr.hpp"

//================= DEBUG MACROS ===================================================

//  These write to the same streams as PER_WRITE_STREAM and PER_READ_STREAM but they
//  go through an indenting filter first to make the current state of objects being
//  written out more obvious.

#define PER_WRITE_INDENT_STREAM( exp )  PER_PRIVATE_WRITE_INDENT_STREAM( exp ) 
#define PER_READ_INDENT_STREAM( exp )   PER_PRIVATE_READ_INDENT_STREAM( exp ) 

//  If yesNo is set to true all characters written will be logged to PER_WRITE_STREAM
#define PER_LOG_WRITE( yesNo )          PER_PRIVATE_LOG_WRITE( yesNo )

//  If yesNo is set to true all characters read will be logged to PER_READ_STREAM
#define PER_LOG_READ( yesNo )           PER_PRIVATE_LOG_READ( yesNo )

//  If yesNo is set to true all object addresses will be written to PER_WRITE_STREAM & PER_READ_STREAM
#define PER_LOG_ADDRESSES( yesNo )      PER_PRIVATE_LOG_ADDRESSES( yesNo )

//================= MACROS FOR ALL CLASSES =========================================

//	Put one of these macros in the public: section of the class declaration.
//  Use the _DEFAULT version if the class has a default constructor.
//  Use the _ABSTRACT version if the class is an abstract base class.
//  Use the _INLINE function if you need all functions inlining (e.g. the class is nested)

#define	PER_MEMBER_PERSISTENT_DEFAULT( className )	                        PER_PRIVATE_MEMBER_PERSISTENT_DEFAULT( className, )
#define	PER_MEMBER_PERSISTENT( className )	                                PER_PRIVATE_MEMBER_PERSISTENT( className, )

#define	PER_MEMBER_PERSISTENT_ABSTRACT( className )                         PER_PRIVATE_MEMBER_PERSISTENT_ABSTRACT( className, virtual )
#define	PER_MEMBER_PERSISTENT_DEFAULT_ABSTRACT( className )                 PER_PRIVATE_MEMBER_PERSISTENT_DEFAULT_ABSTRACT( className, virtual )

#define	PER_MEMBER_PERSISTENT_DEFAULT_INLINE( className )	                PER_PRIVATE_MEMBER_PERSISTENT_DEFAULT_INLINE( className, )
#define	PER_MEMBER_PERSISTENT_INLINE( className )	                        PER_PRIVATE_MEMBER_PERSISTENT_INLINE( className, )

#define	PER_MEMBER_PERSISTENT_ABSTRACT_INLINE( className )                  PER_PRIVATE_MEMBER_PERSISTENT_ABSTRACT_INLINE( className, virtual )
#define	PER_MEMBER_PERSISTENT_DEFAULT_ABSTRACT_INLINE( className )          PER_PRIVATE_MEMBER_PERSISTENT_DEFAULT_ABSTRACT_INLINE( className, virtual )

#define	PER_MEMBER_PERSISTENT_DEFAULT_VIRTUAL( className )	                PER_PRIVATE_MEMBER_PERSISTENT_DEFAULT( className, virtual )
#define	PER_MEMBER_PERSISTENT_VIRTUAL( className )	                        PER_PRIVATE_MEMBER_PERSISTENT( className, virtual )

#define	PER_MEMBER_PERSISTENT_DEFAULT_INLINE_VIRTUAL( className )	        PER_PRIVATE_MEMBER_PERSISTENT_DEFAULT_INLINE( className, virtual )
#define	PER_MEMBER_PERSISTENT_INLINE_VIRTUAL( className )	                PER_PRIVATE_MEMBER_PERSISTENT_INLINE( className, virtual )

//  Write out a pointer as a raw pointer - i.e. do not attempt to write out the
//  object it points to. This is useful for writing out pointers to objects that
//  are contained within classes and will be written out by the classes themselves.
//  Note that any object pointed to by a raw pointer must be written out in the
//  normal way.
#define PER_WRITE_RAW_POINTER( ostr, ptr )       PER_PRIVATE_WRITE_RAW_POINTER( ostr, ptr )

//  Read in a pointer that was written as a raw pointer.
#define PER_READ_RAW_POINTER( istr, ptr )        PER_PRIVATE_READ_RAW_POINTER( istr, ptr )

//  A raw object is written out directly without worrying if it has been written out
//  before. It must not have any pointers to it that are also being written out.
#define PER_WRITE_RAW_OBJECT( ostr, ob )   PER_PRIVATE_WRITE_RAW_OBJECT( ostr, ob )
#define PER_READ_RAW_OBJECT( istr, ob )    PER_PRIVATE_READ_RAW_OBJECT( istr, ob )

//  Write and read raw data. This data must have no pointers to it.
#define PER_WRITE_RAW_DATA( ostr, ptr, nBytes ) PER_PRIVATE_WRITE_RAW_DATA( ostr, ptr, nBytes )
#define PER_READ_RAW_DATA( istr, ptr, nBytes )  PER_PRIVATE_READ_RAW_DATA( istr, ptr, nBytes )

//  Read an object into a const data member. We allow this because persistence
//  is effectively initialising the object from scratch - it is not changing
//  the object after it has been initialised.
#define PER_READ_CONST_OBJECT( ISTR, NON_CONST_TYPE, OBJECT )   PER_PRIVATE_READ_CONST_OBJECT( ISTR, NON_CONST_TYPE, OBJECT )

//  Use this in an hpp file to make an enumerated type persistent. Note that
//  it is not possible to write out pointers to this enumerated type.
#define PER_ENUM_PERSISTENT( TYPE )             PER_PRIVATE_ENUM_PERSISTENT( TYPE )

//  If the enumerated type is private to a class use this within the class definition to
//  make the type persistent.
#define PER_FRIEND_ENUM_PERSISTENT( TYPE )      PER_PRIVATE_FRIEND_ENUM_PERSISTENT( TYPE )

//================= MACROS FOR NON-TEMPLATE CLASSES ================================

//  Put this in the hpp file after the class declaration to declare non-friend
//  perRead and perWrite functions,
#define PER_READ_WRITE( className )         PER_PRIVATE_READ_WRITE( className )

//  Put this in the hpp file in the private: section of the class declaration
//  to declare friend perRead and perWrite functions.
#define PER_FRIEND_READ_WRITE( className )  PER_PRIVATE_FRIEND_READ_WRITE( className )

//	Put this in the hpp file of a non template class you want to make persistent. It
//  should go outside the class declaration and after the declarations of perRead and
//  perWrite for the class.

#define	PER_DECLARE_PERSISTENT( className )	PER_PRIVATE_DECLARE_PERSISTENT( className )

//	Put this in the cpp file of a non template class you want to make persistent

#define	PER_DEFINE_PERSISTENT( className )	        PER_PRIVATE_DEFINE_PERSISTENT( className )
#define	PER_DEFINE_PERSISTENT_ABSTRACT( className )	PER_PRIVATE_DEFINE_PERSISTENT_ABSTRACT( className )

//  To make the persistence functions inline use this macro in the class declaration
//  rather than PER_DEFINE_PERSISTENT in the cpp file

#define	PER_DEFINE_PERSISTENT_INLINE( className ) PER_PRIVATE_DEFINE_PERSISTENT_INLINE( className )

//	Register a derived class with the persistence mechanism so that the class can be
//  written and read via a base class pointer. The application must supply and call
//  a function which includes the PER_REGISTER_DERIVED_CLASS macro for every class
//  that can be written / read in this way.

#define	PER_REGISTER_DERIVED_CLASS( className )     PER_PRIVATE_REGISTER_DERIVED_CLASS( className ) 

//================= MACROS FOR TEMPLATE CLASSES ================================

//	Put this in the hpp file outside the class declaration
#define	PER_DECLARE_PERSISTENT_T1( className )	 PER_PRIVATE_DECLARE_PERSISTENT_T1( className )
#define	PER_DECLARE_PERSISTENT_T2( className )	 PER_PRIVATE_DECLARE_PERSISTENT_T2( className )
#define	PER_DECLARE_PERSISTENT_T3( className )	 PER_PRIVATE_DECLARE_PERSISTENT_T3( className )
#define	PER_DECLARE_PERSISTENT_T4( className )	 PER_PRIVATE_DECLARE_PERSISTENT_T4( className )
#define	PER_DECLARE_PERSISTENT_T5( className )	 PER_PRIVATE_DECLARE_PERSISTENT_T5( className )
#define	PER_DECLARE_PERSISTENT_T6( className )	 PER_PRIVATE_DECLARE_PERSISTENT_T6( className )
#define	PER_DECLARE_PERSISTENT_T7( className )	 PER_PRIVATE_DECLARE_PERSISTENT_T7( className )
#define	PER_DECLARE_PERSISTENT_T8( className )	 PER_PRIVATE_DECLARE_PERSISTENT_T8( className )
#define	PER_DECLARE_PERSISTENT_T9( className )	 PER_PRIVATE_DECLARE_PERSISTENT_T9( className )
#define	PER_DECLARE_PERSISTENT_T10( className )  PER_PRIVATE_DECLARE_PERSISTENT_T10( className )

//	Put this in a cpp file. There should be one copy of this for every different
//  set of arguments that the class has been instantiated with. Perhaps the
//  template instantiation file would be a suitable place for it.

#define	PER_DEFINE_PERSISTENT_T1( className, T1 )	PER_PRIVATE_DEFINE_PERSISTENT_T1( className, T1 )
#define	PER_DEFINE_PERSISTENT_T2( className, T1, T2 )	PER_PRIVATE_DEFINE_PERSISTENT_T2( className, T1, T2 )
#define	PER_DEFINE_PERSISTENT_T3( className, T1, T2, T3 )	PER_PRIVATE_DEFINE_PERSISTENT_T3( className, T1, T2, T3 )
#define	PER_DEFINE_PERSISTENT_T4( className, T1, T2, T3, T4 )	PER_PRIVATE_DEFINE_PERSISTENT_T4( className, T1, T2, T3, T4 )
#define	PER_DEFINE_PERSISTENT_T5( className, T1, T2, T3, T4, T5 )	PER_PRIVATE_DEFINE_PERSISTENT_T5( className, T1, T2, T3, T4, T5 )
#define	PER_DEFINE_PERSISTENT_T6( className, T1, T2, T3, T4, T5, T6 )	PER_PRIVATE_DEFINE_PERSISTENT_T6( className, T1, T2, T3, T4, T5, T6 )
#define	PER_DEFINE_PERSISTENT_T7( className, T1, T2, T3, T4, T5, T6, T7 )	PER_PRIVATE_DEFINE_PERSISTENT_T7( className, T1, T2, T3, T4, T5, T6, T7 )
#define	PER_DEFINE_PERSISTENT_T8( className, T1, T2, T3, T4, T5, T6, T7, T8 )	PER_PRIVATE_DEFINE_PERSISTENT_T8( className, T1, T2, T3, T4, T5, T6, T7, T8 )
#define	PER_DEFINE_PERSISTENT_T9( className, T1, T2, T3, T4, T5, T6, T7, T8, T9 )	PER_PRIVATE_DEFINE_PERSISTENT_T9( className, T1, T2, T3, T4, T5, T6, T7, T8, T9 )
#define	PER_DEFINE_PERSISTENT_T10( className, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10 )	PER_PRIVATE_DEFINE_PERSISTENT_T10( className, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10 )


#define	PER_DEFINE_PERSISTENT_INLINE_T1( className )	PER_PRIVATE_DEFINE_PERSISTENT_INLINE_T1( className )
#define	PER_DEFINE_PERSISTENT_INLINE_T2( className )	PER_PRIVATE_DEFINE_PERSISTENT_INLINE_T2( className )



//	Put this in a cpp file. There should be one copy of this for every different
//  set of arguments that the class has been instantiated with. Perhaps the
//  template instantiation file would be a suitable place for it.

#define	PER_DEFINE_PERSISTENT_ABSTRACT_T1( className, T1 )	PER_PRIVATE_DEFINE_PERSISTENT_ABSTRACT_T1( className, T1 )
#define	PER_DEFINE_PERSISTENT_ABSTRACT_T2( className, T1, T2 )	PER_PRIVATE_DEFINE_PERSISTENT_ABSTRACT_T2( className, T1, T2 )
#define	PER_DEFINE_PERSISTENT_ABSTRACT_T3( className, T1, T2, T3 )	PER_PRIVATE_DEFINE_PERSISTENT_ABSTRACT_T3( className, T1, T2, T3 )
#define	PER_DEFINE_PERSISTENT_ABSTRACT_T4( className, T1, T2, T3, T4 )	PER_PRIVATE_DEFINE_PERSISTENT_ABSTRACT_T4( className, T1, T2, T3, T4 )
#define	PER_DEFINE_PERSISTENT_ABSTRACT_T5( className, T1, T2, T3, T4, T5 )	PER_PRIVATE_DEFINE_PERSISTENT_ABSTRACT_T5( className, T1, T2, T3, T4, T5 )
#define	PER_DEFINE_PERSISTENT_ABSTRACT_T6( className, T1, T2, T3, T4, T5, T6 )	PER_PRIVATE_DEFINE_PERSISTENT_ABSTRACT_T6( className, T1, T2, T3, T4, T5, T6 )
#define	PER_DEFINE_PERSISTENT_ABSTRACT_T7( className, T1, T2, T3, T4, T5, T6, T7 )	PER_PRIVATE_DEFINE_PERSISTENT_ABSTRACT_T7( className, T1, T2, T3, T4, T5, T6, T7 )
#define	PER_DEFINE_PERSISTENT_ABSTRACT_T8( className, T1, T2, T3, T4, T5, T6, T7, T8 )	PER_PRIVATE_DEFINE_PERSISTENT_ABSTRACT_T8( className, T1, T2, T3, T4, T5, T6, T7, T8 )
#define	PER_DEFINE_PERSISTENT_ABSTRACT_T9( className, T1, T2, T3, T4, T5, T6, T7, T8, T9 )	PER_PRIVATE_DEFINE_PERSISTENT_ABSTRACT_T9( className, T1, T2, T3, T4, T5, T6, T7, T8, T9 )
#define	PER_DEFINE_PERSISTENT_ABSTRACT_T10( className, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10 )	PER_PRIVATE_DEFINE_PERSISTENT_ABSTRACT_T10( className, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10 )

//	Register a derived class with the persistence mechanism so that the class can be
//  written and read via a base class pointer. The application must supply and call
//  a function which includes the PER_REGISTER_DERIVED_CLASS macro for every class
//  that can be written / read in this way.

#define	PER_REGISTER_DERIVED_CLASS_T1( className, T1 )	PER_PRIVATE_REGISTER_DERIVED_CLASS_T1( className, T1 )
#define	PER_REGISTER_DERIVED_CLASS_T2( className, T1, T2 )	PER_PRIVATE_REGISTER_DERIVED_CLASS_T2( className, T1, T2 )
#define	PER_REGISTER_DERIVED_CLASS_T3( className, T1, T2, T3 )	PER_PRIVATE_REGISTER_DERIVED_CLASS_T3( className, T1, T2, T3 )
#define	PER_REGISTER_DERIVED_CLASS_T4( className, T1, T2, T3, T4 )	PER_PRIVATE_REGISTER_DERIVED_CLASS_T4( className, T1, T2, T3, T4 )
#define	PER_REGISTER_DERIVED_CLASS_T5( className, T1, T2, T3, T4, T5 )	PER_PRIVATE_REGISTER_DERIVED_CLASS_T5( className, T1, T2, T3, T4, T5 )
#define	PER_REGISTER_DERIVED_CLASS_T6( className, T1, T2, T3, T4, T5, T6 )	PER_PRIVATE_REGISTER_DERIVED_CLASS_T6( className, T1, T2, T3, T4, T5, T6 )
#define	PER_REGISTER_DERIVED_CLASS_T7( className, T1, T2, T3, T4, T5, T6, T7 )	PER_PRIVATE_REGISTER_DERIVED_CLASS_T7( className, T1, T2, T3, T4, T5, T6, T7 )
#define	PER_REGISTER_DERIVED_CLASS_T8( className, T1, T2, T3, T4, T5, T6, T7, T8 )	PER_PRIVATE_REGISTER_DERIVED_CLASS_T8( className, T1, T2, T3, T4, T5, T6, T7, T8 )
#define	PER_REGISTER_DERIVED_CLASS_T9( className, T1, T2, T3, T4, T5, T6, T7, T8, T9 )	PER_PRIVATE_REGISTER_DERIVED_CLASS_T9( className, T1, T2, T3, T4, T5, T6, T7, T8, T9 )
#define	PER_REGISTER_DERIVED_CLASS_T10( className, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10 )	PER_PRIVATE_REGISTER_DERIVED_CLASS_T10( className, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10 )

//  Put this in the ctp file to define a create function that uses the default
//  constructor.
#define PER_DEFAULT_CREATE_FUNCTION_T1( className )    PER_PRIVATE_DEFAULT_CREATE_FUNCTION_T1( className )
#define PER_DEFAULT_CREATE_FUNCTION_T2( className )    PER_PRIVATE_DEFAULT_CREATE_FUNCTION_T2( className )
#define PER_DEFAULT_CREATE_FUNCTION_T3( className )    PER_PRIVATE_DEFAULT_CREATE_FUNCTION_T3( className )
#define PER_DEFAULT_CREATE_FUNCTION_T4( className )    PER_PRIVATE_DEFAULT_CREATE_FUNCTION_T4( className )
#define PER_DEFAULT_CREATE_FUNCTION_T5( className )    PER_PRIVATE_DEFAULT_CREATE_FUNCTION_T5( className )
#define PER_DEFAULT_CREATE_FUNCTION_T6( className )    PER_PRIVATE_DEFAULT_CREATE_FUNCTION_T6( className )
#define PER_DEFAULT_CREATE_FUNCTION_T7( className )    PER_PRIVATE_DEFAULT_CREATE_FUNCTION_T7( className )
#define PER_DEFAULT_CREATE_FUNCTION_T8( className )    PER_PRIVATE_DEFAULT_CREATE_FUNCTION_T8( className )
#define PER_DEFAULT_CREATE_FUNCTION_T9( className )    PER_PRIVATE_DEFAULT_CREATE_FUNCTION_T9( className )
#define PER_DEFAULT_CREATE_FUNCTION_T10( className )   PER_PRIVATE_DEFAULT_CREATE_FUNCTION_T10( className )

//  Put this in the ctp file to define a create function that uses the stream
//  constructor.
#define PER_STREAM_CREATE_FUNCTION_T1( className )    PER_PRIVATE_STREAM_CREATE_FUNCTION_T1( className )
#define PER_STREAM_CREATE_FUNCTION_T2( className )    PER_PRIVATE_STREAM_CREATE_FUNCTION_T2( className )
#define PER_STREAM_CREATE_FUNCTION_T3( className )    PER_PRIVATE_STREAM_CREATE_FUNCTION_T3( className )
#define PER_STREAM_CREATE_FUNCTION_T4( className )    PER_PRIVATE_STREAM_CREATE_FUNCTION_T4( className )
#define PER_STREAM_CREATE_FUNCTION_T5( className )    PER_PRIVATE_STREAM_CREATE_FUNCTION_T5( className )
#define PER_STREAM_CREATE_FUNCTION_T6( className )    PER_PRIVATE_STREAM_CREATE_FUNCTION_T6( className )
#define PER_STREAM_CREATE_FUNCTION_T7( className )    PER_PRIVATE_STREAM_CREATE_FUNCTION_T7( className )
#define PER_STREAM_CREATE_FUNCTION_T8( className )    PER_PRIVATE_STREAM_CREATE_FUNCTION_T8( className )
#define PER_STREAM_CREATE_FUNCTION_T9( className )    PER_PRIVATE_STREAM_CREATE_FUNCTION_T9( className )
#define PER_STREAM_CREATE_FUNCTION_T10( className )   PER_PRIVATE_STREAM_CREATE_FUNCTION_T10( className )

//  Put one of these in the hpp file after the class declaration to declare non-friend
//  perRead and perWrite functions,
#define PER_READ_WRITE_T1( className )          PER_PRIVATE_READ_WRITE_T1( className )
#define PER_READ_WRITE_T2( className )          PER_PRIVATE_READ_WRITE_T2( className )
#define PER_READ_WRITE_T3( className )          PER_PRIVATE_READ_WRITE_T3( className )
#define PER_READ_WRITE_T4( className )          PER_PRIVATE_READ_WRITE_T4( className )
#define PER_READ_WRITE_T5( className )          PER_PRIVATE_READ_WRITE_T5( className )
#define PER_READ_WRITE_T6( className )          PER_PRIVATE_READ_WRITE_T6( className )
#define PER_READ_WRITE_T7( className )          PER_PRIVATE_READ_WRITE_T7( className )
#define PER_READ_WRITE_T8( className )          PER_PRIVATE_READ_WRITE_T8( className )
#define PER_READ_WRITE_T9( className )          PER_PRIVATE_READ_WRITE_T9( className )
#define PER_READ_WRITE_T10( className )         PER_PRIVATE_READ_WRITE_T10( className )

//  Put one of these in the hpp file in the private: section of the class declaration
//  to declare friend perRead and perWrite functions.
#define PER_FRIEND_READ_WRITE_T1( className )   PER_PRIVATE_FRIEND_READ_WRITE_T1( className )
#define PER_FRIEND_READ_WRITE_T2( className )   PER_PRIVATE_FRIEND_READ_WRITE_T2( className )
#define PER_FRIEND_READ_WRITE_T3( className )   PER_PRIVATE_FRIEND_READ_WRITE_T3( className )
#define PER_FRIEND_READ_WRITE_T4( className )   PER_PRIVATE_FRIEND_READ_WRITE_T4( className )
#define PER_FRIEND_READ_WRITE_T5( className )   PER_PRIVATE_FRIEND_READ_WRITE_T5( className )
#define PER_FRIEND_READ_WRITE_T6( className )   PER_PRIVATE_FRIEND_READ_WRITE_T6( className )
#define PER_FRIEND_READ_WRITE_T7( className )   PER_PRIVATE_FRIEND_READ_WRITE_T7( className )
#define PER_FRIEND_READ_WRITE_T8( className )   PER_PRIVATE_FRIEND_READ_WRITE_T8( className )
#define PER_FRIEND_READ_WRITE_T9( className )   PER_PRIVATE_FRIEND_READ_WRITE_T9( className )
#define PER_FRIEND_READ_WRITE_T10( className )  PER_PRIVATE_FRIEND_READ_WRITE_T10( className )

//  Use these macros to add persistence to classes that we cannot get
//  into to change. Note that this will only work for classes with a 
//  default constructor that are not part of an inheritance hierarchy.
//  You will need to add perWrite and perRead functions as normal.

#define PER_DEFINE_NON_INTRUSIVE_PERSISTENT_T2( className )  PER_PRIVATE_DEFINE_NON_INTRUSIVE_PERSISTENT_T2( className )

#endif

/* End PERSIST.HPP ******************************************************/

