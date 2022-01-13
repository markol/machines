#pragma once

#include "ctl/pvector.hpp"
#include "machlog/machine.hpp"
#include "machlog/squad.hpp"
#include "machlog/strategy.hpp"

template<class Operation>
class MachLog1stPersonOpDispatch
{
public:
    explicit MachLog1stPersonOpDispatch(MachLogSquadron** ppSquad)
    {
        ppSquad_ = ppSquad;
        pSkipThisMachine_ = nullptr;
    }
    ~MachLog1stPersonOpDispatch() {}

    // YO!: The first parameter of an operation, pActor, is handled internally.
    template<typename ...OperationArgs>
    void dispatchOperation(OperationArgs... args) const
    {
        MachLogSquadron* dispatchTo = *ppSquad_;

        if (dispatchTo == nullptr)
        {
            return;
        }
        if (dispatchTo->machines().size() == 0)
        {
            return;
        }

        // The Player-controlled machine will not be in this list. It is removed while in 1st person.
        for (auto machine : dispatchTo->machines())
        {
            if (pSkipThisMachine_ != nullptr)
            {
                if (machine == pSkipThisMachine_)
                {
                    continue;
                }
            }

            // DON'T PANIC! MachLogStrategy is responsible for Operation disposal.
            machine->strategy().newOperation(new Operation(machine, args...));
        }
    }

    // Set a machine (if any) to skip when dispatching the op to the squad
    // ...For example, we don't want to tell a machine to follow itself.
    void setSkipThisMachine(MachLogMachine* skipMe) const
    {
        pSkipThisMachine_ = skipMe;
    }

private:
    MachLogSquadron** ppSquad_;
    mutable MachLogMachine* pSkipThisMachine_;
};
