/*
 * O P C O N S T R . C P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machlog/optsk.hpp"

PER_DEFINE_PERSISTENT_ABSTRACT( MachLogTaskOperation );

MachLogTaskOperation::MachLogTaskOperation( const char* pOpType, MachLogOperation::OperationType opType )
:	MachLogOperation( pOpType , opType )
{
}

MachLogTaskOperation::~MachLogTaskOperation()
{
}

void perWrite( PerOstream& ostr, const MachLogTaskOperation& op )
{
	const MachLogOperation& base1 = op;

	ostr << base1;
}

void perRead( PerIstream& istr, MachLogTaskOperation& op )
{
	MachLogOperation& base1 = op;

	istr >> base1;
}

MachLogTaskOperation::MachLogTaskOperation( PerConstructor con )
:	MachLogOperation( con )
{
}


/* End OPtsk.CPP *************************************************/
