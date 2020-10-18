/*
 * C O M P I T E M . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "stdlib/string.hpp"
#include <stdio.h>
#include "system/registry.hpp"
#include "ctl/vector.hpp"
#include "machphys/compitem.hpp"

static void idToString( const ItemId& id, string* stringId )
{
	PRE( stringId );
	char buffer[20];
	sprintf( buffer, "%u", id );
	*stringId = buffer;
}

MachPhysComplexityItem::MachPhysComplexityItem( const ItemId& id )
: id_( id ),
  changed_( true )
{
}

MachPhysComplexityItem::~MachPhysComplexityItem()
{
}

MachPhysComplexityBooleanItem::MachPhysComplexityBooleanItem( const ItemId& setId, bool enabled )
: MachPhysComplexityItem( setId ),
  enabled_( enabled )
{
	bool buf;
	int sizeOfBool = sizeof(bool);
	bool readSucceeded;

	string strId;
	idToString(id(), &strId);

	SysRegistry::KeyHandle handle;
	SysRegistry::instance().openKey( "Options\\Graphics Complexity", &handle, SysRegistry::CURRENT_USER );
	readSucceeded = ( SysRegistry::instance().queryValue( handle, strId, SysRegistry::INTEGER, &buf, &sizeOfBool ) == SysRegistry::SUCCESS );
	SysRegistry::instance().closeKey( handle );

	if( readSucceeded )
	{
		enabled_ = buf;
	}
	else
	{
		SysRegistry::instance().setIntegerValue( "Options\\Graphics Complexity", strId, enabled_, SysRegistry::CURRENT_USER );
	}
}

MachPhysComplexityBooleanItem::~MachPhysComplexityBooleanItem()
{
}

void MachPhysComplexityBooleanItem::changeState( bool enabled )
{
	if( enabled_ != enabled )
	{
		enabled_=enabled;
		changed_=true;

		// persist new setting
		string strId;
		idToString(id(), &strId);
		SysRegistry::instance().setIntegerValue( "Options\\Graphics Complexity", strId, enabled_, SysRegistry::CURRENT_USER );
	}
}

MachPhysComplexityChoiceItem::MachPhysComplexityChoiceItem( const ItemId& setId, uint nChoices, uint choice )
: MachPhysComplexityItem( setId ),
  nChoices_( nChoices ),
  choice_( 0 )
{
	PRE(choice < nChoices);
	choice_ = choice;

	// override choice from the value set in the registries if any
	uint buf = 0;
	int sizeOfInt = sizeof(uint);
	bool readSucceeded;

	string strId;
	idToString(id(), &strId);

	SysRegistry::KeyHandle handle;
	SysRegistry::instance().openKey( "Options\\Graphics Complexity", &handle, SysRegistry::CURRENT_USER );
	readSucceeded = ( SysRegistry::instance().queryValue( handle, strId, SysRegistry::INTEGER, &buf, &sizeOfInt ) == SysRegistry::SUCCESS );
	SysRegistry::instance().closeKey( handle );

	if( readSucceeded and buf < nChoices_ )
	{
		choice_ = buf;
	}
	else
	{
		SysRegistry::instance().setIntegerValue( "Options\\Graphics Complexity", strId, choice_, SysRegistry::CURRENT_USER );
	}
}

MachPhysComplexityChoiceItem::~MachPhysComplexityChoiceItem()
{
}

uint MachPhysComplexityChoiceItem::nChoices() const
{
	return nChoices_;
}

uint MachPhysComplexityChoiceItem::choice() const
{
	return choice_;
}

void MachPhysComplexityChoiceItem::changeState( uint choice )
{
	PRE(choice < nChoices_ );
	if( choice_ != choice )
	{
		choice_=choice;
		changed_=true;

		// persist new setting
		string strId;
		idToString(id(), &strId);
		SysRegistry::instance().setIntegerValue( "Options\\Graphics Complexity", strId, choice_, SysRegistry::CURRENT_USER );
	}
}


/* End COMPITEM.CPP **************************************************/
