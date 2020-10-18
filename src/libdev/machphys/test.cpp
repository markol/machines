MachPhysTransporter* pTransporter = _NEW(
	MachPhysTransporter( pParent, transform, bodyLevel, brainLevel ) );




MachPhysTransporter::MachPhysTransporter(
	pParent, transform, bodyLevel, brainLevel )

: MachPhysMachine( pParent, transform,
    transporterFactory_( bodyLevel, brainLevel ) )
{
	//	Set transporter specific things from the copy stored in the factory
}


//	Constructor used by the factory
MachPhysTransporter::MachPhysTransporter(
	pParent, bodyLevel, brainLevel )

: MachPhysMachine( pParent, MexTransform3d(),
     bodyCDFName( bodyLevel ), faceplateLODName( brainLevel ),
     locomotionType( bodyLevel ) )
{
}



MachPhysMachine::MachPhysMachine(
   pParent, transform, bodyCDFName, faceplateLODName, locomotionType )
{
}


MachPhysMachine::MachPhysMachine(
   pParent, transform, MachPhysMachine& copyMe )
{
	//	Copy all W4dLinks (i.e. copy the composite)

	//	Copy all speed / acceleration information
}