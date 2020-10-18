virtual const MachPhysMachineData& MachPhysMachine::machineData() const = 0;

const MachPhysAggressorData& MachPhysAggressor::data() const
{
	return MachPhysData::instance().aggressorData( hwLevel_, swLevel_ );
}

virtual const MachPhysMachineData& MachPhysAggressor::machineData() const
{
	return data();
}

//////////////////////////////////////////////////////

virtual const MachPhysObjectData& MachActor::objectData() const = 0;


virtual const MachPhysMachineData& MachLogMachine::machineData() const = 0;
const MachPhysObjectData& MachLogMachine::objectData() const
{
	return machineData();
};


const MachPhysAggressorData& MachLogMachine::aggressorData() const
{
	return physAggressor().data();
}

virtual const MachPhysMachineData& MachLogAggressor::machineData() const
{
	return aggressorData();
}









private:
MachPhysAggressor& MachLogAggressor::physAggressor()
{
	return _STATIC_CAST( MachPhysAggressor*, physObject() );
}