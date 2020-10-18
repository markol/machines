/*
 * I S T Y P E . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved.
 */

#ifndef _MACHLOG_ISTYPE_HPP
#define _MACHLOG_ISTYPE_HPP

#include "machgui/restorer.hpp"

/* //////////////////////////////////////////////////////////////// */

class machLogTypePredicate
: protected MachLogTypeRestorer
{
	machLogTypePredicate( MachActor * pActor )
	: pActor_( pActor )
	{
	}

	operator bool()
	{
		process( pActor_ );
		return result_;
	}

	///////////////////////////////

protected:

	virtual ProcessAsSubtype doProcessConstruction( MachLogConstruction * )
	{
		return PROCESS_AS_SUBTYPE;
	}
	
	virtual ProcessAsSubtype doProcessMachine( MachLogMachine * )
	{
		return PROCESS_AS_SUBTYPE;
	}

	virtual ProcessAsSubtype doDefaultProcess( MachActor * )
	{
		result_ = false;
		return DO_NOT_PROCESS_AS_SUBTYPE;
	}
		
	MachActor * pActor_;
	bool		result_;
};

/* //////////////////////////////////////////////////////////////// */

class machLogIsBoss
: public machLogTypePredicate
{
public:

	machLogIsBoss( MachActor * pActor )
	: machLogTypePredicate( pActor )
	{
	}


protected:
	
	virtual ProcessAsSubtype doProcessBoss( MachLogAdministrator * )
	{
		result_ = true;
		return DO_NOT_PROCESS_AS_SUBTYPE;
	}
};

/* //////////////////////////////////////////////////////////////// */

/* //////////////////////////////////////////////////////////////// */

#endif // #ifndef _MACHLOG_ISTYPE_HPP

/* End ISTYPE.HPP ***************************************************/