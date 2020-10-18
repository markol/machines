/*
 * A N I M R E S . C P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machlog/animres.hpp"
#include "machlog/technici.hpp"

PER_DEFINE_PERSISTENT( MachLogResearchAnimation );
/* //////////////////////////////////////////////////////////////// */

MachLogResearchAnimation::MachLogResearchAnimation( MachLogTechnician * pActor )
: pActor_( pActor )
{
	/* Intentionally Empty	*/
}
	
PhysRelativeTime MachLogResearchAnimation::doUpdate()
{
	return 1.0;
}

void MachLogResearchAnimation::doOutputOperator( ostream& o ) const
{
	o << "MachLogResearchAnimation ";
}

MachLogResearchAnimation::~MachLogResearchAnimation()
{
	/* Intentionally Empty	*/
}

PhysRelativeTime MachLogResearchAnimation::doStartAnimation()
{
	return pActor_->research();
}

void perWrite( PerOstream& ostr, const MachLogResearchAnimation& anim )
{
	const MachLogAnimation& base1 = anim;

	ostr << base1;
	ostr << anim.pActor_;
}

void perRead( PerIstream& istr, MachLogResearchAnimation& anim )
{
	MachLogAnimation& base1 = anim;

	istr >> base1;
	istr >> anim.pActor_;
}

MachLogResearchAnimation::MachLogResearchAnimation( PerConstructor con )
:	MachLogAnimation( con )
{
}


/* //////////////////////////////////////////////////////////////// */
