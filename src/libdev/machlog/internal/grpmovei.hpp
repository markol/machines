/*
 * G R P M O V E I . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachLogGroupSimpleMoveImplementation

    The implementation class for MachLogGroupSimpleMove
*/

#ifndef _MACHLOG_GRPMOVEI_HPP
#define _MACHLOG_GRPMOVEI_HPP

//#include <string>
#include "stdlib/string.hpp"

#include "base/base.hpp"

#include "mathex/point3d.hpp"

#include "ctl/vector.hpp"
#include "ctl/pvector.hpp"
#include "ctl/utility.hpp"

#include "machlog/actor.hpp"
#include "machlog/spiral.hpp"
#include "machlog/internal/grpmvutl.hpp"

class PhysConfigSpace2d;

class MachLogGroupSimpleMoveImplementation
// Canonical form revoked
{
public:
    typedef ctl_pvector< MachActor > Actors;
    typedef ctl_vector< MachLogMachine* > Machines;
    typedef ctl_vector< MexPoint3d > Points;

    MachLogGroupSimpleMoveImplementation(
      const Actors&,
      const Points&,
      size_t commandId,
      string* pReason,
      PhysPathFindingPriority pathFindingPriority );
    ~MachLogGroupSimpleMoveImplementation();

    //  Return true iff the move command was successful
    bool    moveOK() const;

    void CLASS_INVARIANT;

private:
    friend ostream& operator <<( ostream& o, const MachLogGroupSimpleMoveImplementation::Actors& t );

    MachLogGroupSimpleMoveImplementation( const MachLogGroupSimpleMoveImplementation& );
    MachLogGroupSimpleMoveImplementation& operator =( const MachLogGroupSimpleMoveImplementation& );

    //  Perform the movement in a more sophisticated manner - each
    //  actor will get given a different destination to try and
    //  avoid too many snarl ups.
    void complexMove(
        const Machines& machines,
        const Points& points,
        size_t commandId,
        string* pReason );

    typedef MachLogGroupMoverUtility::Clump Clump;

    void positionControllingClump(
        const Clump& controllingClump,
        Machines* pUnlocatedMachines );

    void positionMachinesOnGrid(
        const Machines& unlocatedMachines );

    bool canPlaceAtDestination( MachLogMachine* pMachine,
      const MexPoint3d& desiredDestination );

    MexPoint3d  nextGridDestination();

    MATHEX_SCALAR       gridSpacing() const;

    void calculateGridSpacing( const Machines& machines );

    //  This is a friend purely to allow it to instantiate
    //  the vector of MachineData
    friend void ti4();

    CTL_QUAD( MachineData,
      MachLogMachine*, pMachine,
      MexPoint3d, destination,
      MATHEX_SCALAR, clearance,
      MATHEX_SCALAR, sqrDistanceToGroupDestination );

    typedef ctl_vector< MachineData >   DestData;

    static bool compareDistanceToGroupDestination( const MachineData&, const MachineData& );

    MachLogSquareSpiral spiral_;
    Points              points_;
    DestData            destData_;
    MATHEX_SCALAR       gridSpacing_;
    const PhysConfigSpace2d&    configSpace_;
    PhysPathFindingPriority pathFindingPriority_;
};


#endif

/* End GRPMOVEI.HPP *************************************************/

