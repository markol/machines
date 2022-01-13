#pragma once

#include "mathex/point3d.hpp"
#include "machphys/machphys.hpp"

class MachActor;

// UBER-VERBOSE! Might wanna `using` this guy. ;)
struct MachLog1stPersonTargetInfo
{
    MachLog1stPersonTargetInfo()
    {
        strikeType = MachPhys::IN_AIR;
        shootingTarget = nullptr;
        commandTarget = nullptr;
        // The points will default to (0,0,0)
    }

    // What is thy target for thy command?
    // It will either be the enemy in weapon range, command range, or nobody at all.
    MachActor* getCommandTarget() const
    {
        if (shootingTarget != nullptr)
        {
            return shootingTarget;
        }
        if (commandTarget != nullptr)
        {
            return commandTarget;
        }

        return nullptr;
    }

    // There is still a possibility (is there?) that this point could be (0,0,0) so you should check for that
    const MexPoint3d& getCommandPoint() const
    {
        // Prefer command point
        if (not commandPoint.isZeroPoint())
        {
            return commandPoint;
        }

        return shootingPoint;
    }

    //  ON_OBJECT  - Hits machine, building, ILF, etc
    //  ON_TERRAIN - Hits ground
    //  IN_AIR     - hasn't hit anything, but run out of range
    MachPhys::StrikeType strikeType;

    // SHOOTING/COMMAND - Nearer
    MexPoint3d shootingPoint;
    MachActor* shootingTarget;

    // COMMAND ONLY - Farther
    MexPoint3d commandPoint;
    MachActor* commandTarget;
};
