/*
 * A T T A F I L T . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachLogAttackFilter

    An entity filter class used during cover testing by the attack operations in
    conjunction with W4dDomain::findNearerEntity.
*/

#ifndef _MACHLOG_ATTAFILT_HPP
#define _MACHLOG_ATTAFILT_HPP

#include "base/base.hpp"
#include "world4d/entyfilt.hpp"
#include "machphys/machphys.hpp"
#include "machlog/canattac.hpp"

class MachLogAttackFilter : public W4dEntityFilter
// Canonical form revoked
{
public:
    //Filters out machines that are enemies of attackerRace if precision is SAME_RACE_OK
    MachLogAttackFilter( MachPhys::Race attackerRace, MachLogCanAttack::SoftCoverAccuracy precision );

    virtual ~MachLogAttackFilter();

    //////////////////////////////////////////////
    // Inherited from W4dEntityFilter

    //True if entity should be processed. 
    //Returns option for continued tree processing in pOption.
    virtual bool check( const W4dEntity& entity, TreeOption* pOption );

    //////////////////////////////////////////////

    void CLASS_INVARIANT;

private:
    friend ostream& operator <<( ostream& o, const MachLogAttackFilter& t );

    MachLogAttackFilter( const MachLogAttackFilter& );
    MachLogAttackFilter& operator =( const MachLogAttackFilter& );

    //data members
    MachPhys::Race attackerRace_;
    MachLogCanAttack::SoftCoverAccuracy precision_;
};


#endif

/* End ATTAFILT.HPP *************************************************/
