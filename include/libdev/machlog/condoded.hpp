/*
 * C O N D D E A D . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachLogAllOtherUnitsDeadCondition

    This conditon code checks for all units of all the other races are dead - makes setting up WIN actions very
	easy - as you may specify ALL_OTHER_UNITS_DEAD allOthersDeadRed RACE red
	Then WIN ON allOthersDeadRed RACE RED
	etc

*/

#ifndef _MACHLOG_CONDODED_HPP
#define _MACHLOG_CONDODED_HPP

#include "base/base.hpp"
#include "phys/phys.hpp"
#include "sim/conditio.hpp"
#include "machlog/machlog.hpp"

class UtlLineTokeniser;

class MachLogAllOtherRacesUnitsDeadCondition : public SimCondition
// Canonical form revoked
{
public:
	static MachLogAllOtherRacesUnitsDeadCondition* newFromParser( UtlLineTokeniser* );

    MachLogAllOtherRacesUnitsDeadCondition( const string& keyName, MachPhys::Race );

	virtual bool doHasConditionBeenMet() const;

    virtual ~MachLogAllOtherRacesUnitsDeadCondition( void );

    void CLASS_INVARIANT;


	PER_MEMBER_PERSISTENT_VIRTUAL( MachLogAllOtherRacesUnitsDeadCondition );
	PER_FRIEND_READ_WRITE( MachLogAllOtherRacesUnitsDeadCondition );

	typedef	ctl_vector< MachPhys::Race >		OtherRaces;

protected:

	virtual const PhysRelativeTime& recommendedCallBackTimeGap() const;
	virtual void doOutputOperator( ostream& ) const;

private:

    friend ostream& operator <<( ostream& o, const MachLogAllOtherRacesUnitsDeadCondition& t );

    MachLogAllOtherRacesUnitsDeadCondition( const MachLogAllOtherRacesUnitsDeadCondition& );
    MachLogAllOtherRacesUnitsDeadCondition& operator =( const MachLogAllOtherRacesUnitsDeadCondition& );

	MachPhys::Race								race_;
	OtherRaces									otherRace_;
};

PER_DECLARE_PERSISTENT( MachLogAllOtherRacesUnitsDeadCondition );

#endif

/* End CONDODED.HPP *************************************************/
