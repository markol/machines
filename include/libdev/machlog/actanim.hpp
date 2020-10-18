/*
 * A C T A N I M . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachLogPlayAnimationAction

    This action class will play the named animation on the given machine types within the area
	defined by the location and the range.
*/

#ifndef _MACHLOG_ACTPlayAnimation_HPP
#define _MACHLOG_ACTPlayAnimation_HPP

#include "base/base.hpp"
#include "mathex/point2d.hpp"
#include "sim/action.hpp"
#include "machphys/machphys.hpp"
#include "machlog/machlog.hpp"

class UtlLineTokeniser;
class SimCondition;

class MachLogPlayAnimationAction : public SimAction
// Canonical form revoked
{
public:

    virtual ~MachLogPlayAnimationAction( void );
	static MachLogPlayAnimationAction* newFromParser( SimCondition*, bool enabled, UtlLineTokeniser* );

    void CLASS_INVARIANT;

	PER_MEMBER_PERSISTENT_VIRTUAL( MachLogPlayAnimationAction );
	PER_FRIEND_READ_WRITE( MachLogPlayAnimationAction );

protected:
	virtual void doAction();
	virtual void doOutputOperator( ostream& ) const;

private:

    MachLogPlayAnimationAction( SimCondition*, bool enabled );
    friend ostream& operator <<( ostream& o, const MachLogPlayAnimationAction& t );

    MachLogPlayAnimationAction( const MachLogPlayAnimationAction& );
    MachLogPlayAnimationAction& operator =( const MachLogPlayAnimationAction& );

	MachPhys::Race			race_;
	MexPoint2d				point_;
	MATHEX_SCALAR			sqrRange_;
	//all the selection criteria from objectType down are optional. 
	//For certain actor types they are simply not necessary. And if the anyTypeWillDo set then all actors will be
	//considered
	MachLog::ObjectType		objectType_;
	bool					subTypeSet_;
	int						subType_;
	bool					hwLevelSet_;
	int						hwLevel_;
	bool					weaponComboSet_;
	MachPhys::WeaponCombo	weaponCombo_;
	bool					anyTypeWillDo_;
	string					animation_;
	int						repetitions_;
};

PER_DECLARE_PERSISTENT( MachLogPlayAnimationAction );

#endif

/* End ACTPlayAnimation.HPP *************************************************/
