/*
 * C O N D P O D . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachLogPodCapturedCondition

    This conditon code checks for a pod having been captured.

*/

#ifndef _MACHLOG_CONDPOD_HPP
#define _MACHLOG_CONDPOD_HPP

#include "base/base.hpp"
#include "phys/phys.hpp"
#include "sim/conditio.hpp"
#include "machlog/machlog.hpp"

class UtlLineTokeniser;

class MachLogPodCapturedCondition : public SimCondition
// Canonical form revoked
{
public:
	static MachLogPodCapturedCondition* newFromParser( UtlLineTokeniser* );

	virtual bool doHasConditionBeenMet() const;

    virtual ~MachLogPodCapturedCondition();

    void CLASS_INVARIANT;


	PER_MEMBER_PERSISTENT_VIRTUAL( MachLogPodCapturedCondition );
	PER_FRIEND_READ_WRITE( MachLogPodCapturedCondition );

protected:

	virtual const PhysRelativeTime& recommendedCallBackTimeGap() const;
	virtual void doOutputOperator( ostream& ) const;

private:

    MachLogPodCapturedCondition( const string& keyName, MachPhys::Race );

    friend ostream& operator <<( ostream& o, const MachLogPodCapturedCondition& t );

    MachLogPodCapturedCondition( const MachLogPodCapturedCondition& );
    MachLogPodCapturedCondition& operator =( const MachLogPodCapturedCondition& );

	MachPhys::Race								race_;
};

PER_DECLARE_PERSISTENT( MachLogPodCapturedCondition );

#endif

/* End CONDTIME.HPP *************************************************/
