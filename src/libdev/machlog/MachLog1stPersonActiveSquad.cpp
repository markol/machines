#include <utility>
#include "machlog/MachLog1stPersonActiveSquad.hpp"

#include "ctl/pvector.hpp"
#include "machlog/cntrl_pc.hpp"
#include "machlog/races.hpp"
#include "machlog/administ.hpp"
#include "machphys/genedata.hpp"
#include "machphys/mphydata.hpp"

MachLog1stPersonActiveSquadron::MachLog1stPersonActiveSquadron(MachLogSquadron* initialActiveSquad)
    : attackDispatcher_(&pActiveSquadron_),
      followDispatcher_(&pActiveSquadron_),
      moveDispatcher_(&pActiveSquadron_)
{
    pActiveSquadron_ = initialActiveSquad;
}

MachLog1stPersonActiveSquadron::MachLog1stPersonActiveSquadron()
    : MachLog1stPersonActiveSquadron(nullptr)
{

}

MachLog1stPersonActiveSquadron::~MachLog1stPersonActiveSquadron()
{

}

MachLog1stPersonActiveSquadron::MachLog1stPersonActiveSquadron(MachLog1stPersonActiveSquadron&& other)
    : attackDispatcher_(&pActiveSquadron_),
      followDispatcher_(&pActiveSquadron_),
      moveDispatcher_(&pActiveSquadron_)
{
    pActiveSquadron_ = std::move(other.pActiveSquadron_);
}

MachLog1stPersonActiveSquadron& MachLog1stPersonActiveSquadron::operator=(MachLog1stPersonActiveSquadron&& other)
{
    if (this != &other)
    {
        pActiveSquadron_ = std::move(other.pActiveSquadron_);
    }

    return *this;
}

bool MachLog1stPersonActiveSquadron::hasActiveSquadron() const
{
    return ( (pActiveSquadron_ != nullptr) );
}

int64_t MachLog1stPersonActiveSquadron::getActiveSquadronId() const
{
    int64_t id = 0;

    if (hasActiveSquadron())
    {
        id = pActiveSquadron_->squadronId();
    }

    return id;
}

MachLogMachine* const MachLog1stPersonActiveSquadron::getLeadingMachine() const
{
    if (not hasActiveSquadron())
    {
        return nullptr;
    }

    if (pActiveSquadron_->hasCommander())
    {
        return &pActiveSquadron_->commander();
    }

    return pActiveSquadron_->getStrongestMachine();
}

void MachLog1stPersonActiveSquadron::issueAttackCommand(MachActor* target) const
{
    if (not hasActiveSquadron() or target == nullptr)
    {
        return;
    }

    attackDispatcher_.dispatchOperation(target, MachLogAttackOperation::TERMINATE_ON_CHANGE);
}

void MachLog1stPersonActiveSquadron::issueFollowCommand(MachActor* followTarget) const
{
    if (not hasActiveSquadron() or followTarget == nullptr)
    {
        return;
    }

    if (followTarget->objectIsMachine())
    {
        followDispatcher_.setSkipThisMachine(static_cast<MachLogMachine*>(followTarget));
        followDispatcher_.dispatchOperation(static_cast<MachLogMachine*>(followTarget), MexPoint2d( 0, 0 ), getPathfindingPriority());
    }
}

void MachLog1stPersonActiveSquadron::issueMoveCommand(const MexPoint3d& destination) const
{
    if (not hasActiveSquadron() or destination.isZeroPoint())
    {
        return;
    }

    // This is the way they do it elsewhere- a 10m tolerance
    moveDispatcher_.dispatchOperation(destination, true, 10.0);
}

void MachLog1stPersonActiveSquadron::setActiveSquadron(size_t squadIndex)
{
    MachLogRaces& races = MachLogRaces::instance();
    MachPhys::Race race = races.pcController().race();

    // Optionally set the squad so that empty ones may skipped over by player
    auto squad = races.squadrons(race)[squadIndex];
    if (squad->machines().size() > 0)
    {
        pActiveSquadron_ = squad;
    }
}

void MachLog1stPersonActiveSquadron::clearActiveSquadron()
{
    pActiveSquadron_ = nullptr;
}

//static
inline PhysPathFindingPriority MachLog1stPersonActiveSquadron::getPathfindingPriority()
{
    return Phys::defaultPathFindingPriority() + MachPhysData::instance().generalData().pcPathFindingPriority();
}
