#pragma once

#include "machlog/squad.hpp"
#include "machlog/attack.hpp"
#include "machlog/follow.hpp"
#include "machlog/move.hpp"
#include "machlog/MachLog1stPersonOpDispatch.hpp"

class MachLog1stPersonActiveSquadron final
{
public:
    // This constructor is for the case when the embodied machine is a squad member.
    explicit MachLog1stPersonActiveSquadron(MachLogSquadron* initialActiveSquad);
    // When not a squad member, the player will need to select one in the GUI layer.
    MachLog1stPersonActiveSquadron();
    ~MachLog1stPersonActiveSquadron();
    // MOVEABLE
    MachLog1stPersonActiveSquadron(MachLog1stPersonActiveSquadron&& other);
    MachLog1stPersonActiveSquadron& operator=( MachLog1stPersonActiveSquadron&& other);
    // NOT COPYABLE
    MachLog1stPersonActiveSquadron(const MachLog1stPersonActiveSquadron&) = delete;
    MachLog1stPersonActiveSquadron& operator=(const MachLog1stPersonActiveSquadron&) = delete;

    ///////////////////////////////////////////////////

    // Is there an active squad being tracked?
    bool hasActiveSquadron() const;
    // Squadron Id. YOU MUST SUBTRACT ONE to use as an index.
    int64_t getActiveSquadronId() const;

    // Who is the commander or strongest machine in the squad?
    MachLogMachine* const getLeadingMachine() const;

    void issueAttackCommand(MachActor* target) const;
    void issueFollowCommand(MachActor* followTarget) const;
    void issueMoveCommand(const MexPoint3d& destination) const;

    // Set the active squadron "safely" using squad index, 0...9
    // ... Do nothing if the squad is empty.
    void setActiveSquadron(size_t squadIndex);
    void clearActiveSquadron();

private:
    MachLogSquadron* pActiveSquadron_;

    MachLog1stPersonOpDispatch<MachLogAttackOperation> attackDispatcher_;
    MachLog1stPersonOpDispatch<MachLogFollowOperation> followDispatcher_;
    MachLog1stPersonOpDispatch<MachLogMoveToOperation> moveDispatcher_;

    // helper for pathfinding priority to use when dispatching follow commands
    static PhysPathFindingPriority getPathfindingPriority();
};
