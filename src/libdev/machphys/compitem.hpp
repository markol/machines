/*
 * C O M P I T E M . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachPhysComplexityItem

    A brief description of the class should go in here
*/

#ifndef _COMPITEM_HPP
#define _COMPITEM_HPP

#include "base/base.hpp"

typedef uint ItemId;

class MachPhysComplexityItem
// Canonical form revoked
{
public:
	MachPhysComplexityItem( const ItemId& id );
	virtual ~MachPhysComplexityItem();

	const ItemId& id() const { return id_; }
	bool changed() const { return changed_; }

	virtual void update() = 0;

private:
    friend ostream& operator <<( ostream& o, const MachPhysComplexityItem& t );

	MachPhysComplexityItem();
	MachPhysComplexityItem( const MachPhysComplexityItem& );
    MachPhysComplexityItem& operator =( const MachPhysComplexityItem& );

protected:
	ItemId	id_;
	bool	changed_;
};

class MachPhysComplexityBooleanItem	: public MachPhysComplexityItem
{
public:
	MachPhysComplexityBooleanItem( const ItemId& id, bool enabled );
	~MachPhysComplexityBooleanItem();

	bool enabled() const { return enabled_; }
	void changeState( bool enabled );

protected:
	bool	enabled_;
};

class MachPhysComplexityChoiceItem	: public MachPhysComplexityItem
{
public:
	MachPhysComplexityChoiceItem( const ItemId& id, uint nChoices, uint choice = 0 );
	~MachPhysComplexityChoiceItem();

	uint nChoices() const;
	uint choice() const;
	void changeState( uint choice );

protected:
	uint nChoices_;
	uint choice_;
};

#endif

/* End COMPITEM.HPP **************************************************/
