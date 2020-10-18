/*
 * P E R S I S T . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
	Internal typedefs and classes used by the persistence mechanism
*/

#ifndef _PER_INTERNAL_PERSIST_HPP
#define _PER_INTERNAL_PERSIST_HPP

enum PerDataType {
	PER_OBJECT,
	PER_POINTER,
	PER_RAW_POINTER,
	PER_OBJECT_POINTER,
	PER_NAMED_OBJECT_POINTER
};

#endif