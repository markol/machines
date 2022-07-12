/*
 * C O N D R E A C . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachLogReachedLocationCondition

    This conditon code checks for the existence (on a race basis) for a certain number of the specified
	ObjectType at a position. The object type may be fully specified (i.e. to weaponCombo level).
	ObjectTypes allowed: machines, constructions, landmines, oreholographs.

*/

#ifndef _MACHLOG_CONDREAC_HPP
#define _MACHLOG_CONDREAC_HPP

#include "base/base.hpp"
#include "mathex/point2d.hpp"
#include "phys/phys.hpp"
#include "sim/conditio.hpp"
#include "machlog/machlog.hpp"

class UtlLineTokeniser;

class MachLogReachedLocationCondition : public SimCondition
// Canonical form revoked
{
public:
	enum BooleanType { GREATER_THAN_EQUAL, LESS_THAN };

	static MachLogReachedLocationCondition* newFromParser( UtlLineTokeniser* );
	static MachLogReachedLocationCondition* newFromParser( UtlLineTokeniser*, BooleanType );

	virtual bool doHasConditionBeenMet() const;

    virtual ~MachLogReachedLocationCondition();

    void CLASS_INVARIANT;


	PER_MEMBER_PERSISTENT_VIRTUAL( MachLogReachedLocationCondition );
	PER_FRIEND_READ_WRITE( MachLogReachedLocationCondition );

protected:

	virtual const PhysRelativeTime& recommendedCallBackTimeGap() const;
	virtual void doOutputOperator( ostream& ) const;

private:

    MachLogReachedLocationCondition( const string& keyName, MachPhys::Race, size_t number, const MexPoint2d& point, const MATHEX_SCALAR& range );

    friend ostream& operator <<( ostream& o, const MachLogReachedLocationCondition& t );

    MachLogReachedLocationCondition( const MachLogReachedLocationCondition& );
    MachLogReachedLocationCondition& operator =( const MachLogReachedLocationCondition& );

	MachPhys::Race			race_;
	MexPoint2d				point_;
	bool					doRangeCheck_;
	MATHEX_SCALAR			sqrRange_;
	size_t					number_;		//any number can be specified
	MachLog::ObjectType		objectType_;	//this field must be present
	//all the selection criteria from subtype down are optional. For certain actor types they are simply not necessary.
	bool					subTypeSet_;
	int						subType_;
	bool					hwLevelSet_;
	int						hwLevel_;
	bool					weaponComboSet_;
	MachPhys::WeaponCombo	weaponCombo_;
	bool					anyTypeWillDo_;
	bool					machineWillDo_;
	bool					constructionWillDo_;
	BooleanType				booleanType_;

};

PER_ENUM_PERSISTENT( MachLogReachedLocationCondition::BooleanType );

PER_DECLARE_PERSISTENT( MachLogReachedLocationCondition );

#endif

/* End CONDTIME.HPP *************************************************/
