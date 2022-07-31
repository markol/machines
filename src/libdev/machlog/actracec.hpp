/*
 * A C T R A C E C . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachLogChangeRaceAction

    This action class will change all objects of original race to be target race witihin 
	an area defined by a point and a range.
*/

#ifndef _MACHLOG_ACTRACEC_HPP
#define _MACHLOG_ACTRACEC_HPP

#include "base/base.hpp"
#include "mathex/point2d.hpp"
#include "sim/action.hpp"
#include "machphys/machphys.hpp"

class UtlLineTokeniser;
class SimCondition;

class MachLogChangeRaceAction : public SimAction
// Canonical form revoked
{
public:

    virtual ~MachLogChangeRaceAction();
	static MachLogChangeRaceAction* newFromParser( SimCondition*, bool enabled, UtlLineTokeniser* );

    void CLASS_INVARIANT;

	PER_MEMBER_PERSISTENT_VIRTUAL( MachLogChangeRaceAction );
	PER_FRIEND_READ_WRITE( MachLogChangeRaceAction );

protected:
	virtual void doAction();
	virtual void doOutputOperator( ostream& ) const;

private:

    MachLogChangeRaceAction( SimCondition*, bool enabled );
    friend ostream& operator <<( ostream& o, const MachLogChangeRaceAction& t );

    MachLogChangeRaceAction( const MachLogChangeRaceAction& );
    MachLogChangeRaceAction& operator =( const MachLogChangeRaceAction& );

	MachPhys::Race			originalRace_;
	MachPhys::Race			targetRace_;
	MexPoint2d				dest_;
	MATHEX_SCALAR			range_;
};

PER_DECLARE_PERSISTENT( MachLogChangeRaceAction );

#endif

/* End ACTREINF.HPP *************************************************/
