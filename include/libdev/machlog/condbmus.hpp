/*
 * C O N D T I M E . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachLogBMUCountCondition

    This conditon code checks for the BMUs being at a certain level it can cope with greater than or less than
	checks. 

*/

#ifndef _MACHLOG_CONDBMUS_HPP
#define _MACHLOG_CONDBMUS_HPP

#include "base/base.hpp"
#include "phys/phys.hpp"
#include "sim/conditio.hpp"
#include "machlog/machlog.hpp"

class UtlLineTokeniser;

class MachLogBMUCountCondition : public SimCondition
// Canonical form revoked
{
public:
	enum CheckType{ LESS_THAN_EQUAL, GREATER_THAN_EQUAL };

	static MachLogBMUCountCondition* newFromParser( UtlLineTokeniser* );

	virtual bool doHasConditionBeenMet() const;

    virtual ~MachLogBMUCountCondition();

    void CLASS_INVARIANT;


	PER_MEMBER_PERSISTENT_VIRTUAL( MachLogBMUCountCondition );
	PER_FRIEND_READ_WRITE( MachLogBMUCountCondition );

protected:

	virtual const PhysRelativeTime& recommendedCallBackTimeGap() const;
	virtual void doOutputOperator( ostream& ) const;

private:

    MachLogBMUCountCondition( const string& keyName, MachPhys::Race, MachPhys::BuildingMaterialUnits, CheckType );

    friend ostream& operator <<( ostream& o, const MachLogBMUCountCondition& t );

    MachLogBMUCountCondition( const MachLogBMUCountCondition& );
    MachLogBMUCountCondition& operator =( const MachLogBMUCountCondition& );

	MachPhys::Race								race_;
	//the number to check against
	MachPhys::BuildingMaterialUnits				number_;
	CheckType									checkType_;
};

PER_DECLARE_PERSISTENT( MachLogBMUCountCondition );
PER_ENUM_PERSISTENT( MachLogBMUCountCondition::CheckType );

#endif

/* End CONDTIME.HPP *************************************************/
