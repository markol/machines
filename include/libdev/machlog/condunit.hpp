/*
 * C O N D T I M E . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachLogLowUnitCountCondition

    This conditon code checks for the existence (on a race basis) for a certain number of the specified
	ObjectType - this pulls directly into the MLRaces collections.
	ObjectTypes allowed Machines heirarchy, construction hierarchy

*/

#ifndef _MACHLOG_CONDUNIT_HPP
#define _MACHLOG_CONDUNIT_HPP

#include "base/base.hpp"
#include "phys/phys.hpp"
#include "sim/conditio.hpp"
#include "machlog/machlog.hpp"

class UtlLineTokeniser;

class MachLogLowUnitCountCondition : public SimCondition
// Canonical form revoked
{
public:

	static MachLogLowUnitCountCondition* newFromParser( UtlLineTokeniser* );

	//this constructor will not normally be called directly but may be called for the dynamic creation of these objects.
    MachLogLowUnitCountCondition( const string& keyName, MachPhys::Race, MachLog::ObjectType, size_t number, int subType );

	virtual bool doHasConditionBeenMet() const;

    virtual ~MachLogLowUnitCountCondition();

    void CLASS_INVARIANT;


	PER_MEMBER_PERSISTENT_VIRTUAL( MachLogLowUnitCountCondition );
	PER_FRIEND_READ_WRITE( MachLogLowUnitCountCondition );

protected:

	virtual const PhysRelativeTime& recommendedCallBackTimeGap() const;
	virtual void doOutputOperator( ostream& ) const;

private:

    //MachLogLowUnitCountCondition( const string& keyName, MachPhys::Race, MachLog::ObjectType, size_t number, int subType );

    friend ostream& operator <<( ostream& o, const MachLogLowUnitCountCondition& t );

    MachLogLowUnitCountCondition( const MachLogLowUnitCountCondition& );
    MachLogLowUnitCountCondition& operator =( const MachLogLowUnitCountCondition& );

	MachLog::ObjectType		objectType_;
	MachPhys::Race			race_;
	size_t					number_;
	//the sub type is only valid for artefacts
	int						subType_;
};

PER_DECLARE_PERSISTENT( MachLogLowUnitCountCondition );

#endif

/* End CONDTIME.HPP *************************************************/
