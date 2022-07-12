/*
 * C O N D R E S E . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachLogResearchCompleteCondition

    A brief description of the class should go in here
*/

#ifndef _MACHLOG_CONDRESEARCH_HPP
#define _MACHLOG_CONDRESEARCH_HPP

#include "base/base.hpp"
#include "phys/phys.hpp"
#include "sim/conditio.hpp"
#include "machlog/machlog.hpp"

class UtlLineTokeniser;
class MachLogResearchCompleteConditionNotifiable;
class MachLogResearchItem;

class MachLogResearchCompleteCondition : public SimCondition
// Canonical form revoked
{
public:

	static MachLogResearchCompleteCondition* newFromParser( UtlLineTokeniser* );

	virtual bool doHasConditionBeenMet() const;

    virtual ~MachLogResearchCompleteCondition();

    void CLASS_INVARIANT;


	PER_MEMBER_PERSISTENT_VIRTUAL( MachLogResearchCompleteCondition );
	PER_FRIEND_READ_WRITE( MachLogResearchCompleteCondition );

protected:

	virtual const PhysRelativeTime& recommendedCallBackTimeGap() const;
	virtual void doOutputOperator( ostream& ) const;

private:

    MachLogResearchCompleteCondition( const string& keyName, MachPhys::Race, 
    								  MachLog::ObjectType, int hwLevel, 
    								  int subType, MachPhys::WeaponCombo );

    friend ostream& operator <<( ostream& o, const MachLogResearchCompleteCondition& t );

    MachLogResearchCompleteCondition( const MachLogResearchCompleteCondition& );
    MachLogResearchCompleteCondition& operator =( const MachLogResearchCompleteCondition& );

	//the item is not persisted directly it is reaquired from the research tree
	MachLogResearchItem*	pItem_;
	MachLog::ObjectType		objectType_;
	int						subType_;
	int						hwLevel_;
	int						swLevel_;
	MachPhys::WeaponCombo	weaponCombo_;
	MachPhys::Race			race_;
	PhysRelativeTime		callBackTimeGap_;
	//the notifiable is also not persisted but recreated
	//defined friendly so that it can change the call back time directly.
	friend class MachLogResearchCompleteConditionNotifiable;
	MachLogResearchCompleteConditionNotifiable*	pNotifiable_;

};

PER_DECLARE_PERSISTENT( MachLogResearchCompleteCondition );

#endif

/* End CONDTIME.HPP *************************************************/
