/*
 * C O N D V I E W . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "stdlib/string.hpp"
#include "world4d/camera.hpp"
#include "world4d/manager.hpp"
#include "world4d/scenemgr.hpp"
#include "sim/manager.hpp"
#include "utility/linetok.hpp"
#include "machlog/condview.hpp"


PER_DEFINE_PERSISTENT( MachLogCameraViewCondition );

MachLogCameraViewCondition::MachLogCameraViewCondition( const string& keyName, const MexPoint3d& location, const MexRadians& angle )
:	SimCondition( keyName ),
	location_( location ),
	angle_( angle )
{
    TEST_INVARIANT;
}

MachLogCameraViewCondition::~MachLogCameraViewCondition()
{
    TEST_INVARIANT;

}

//virtual
bool MachLogCameraViewCondition::doHasConditionBeenMet() const
{
	W4dSceneManager* pSceneManager = W4dManager::instance().sceneManager();
	W4dCamera* pCamera = pSceneManager->currentCamera();
	return pCamera->canSee( location_ );
}

//static
MachLogCameraViewCondition* MachLogCameraViewCondition::newFromParser( UtlLineTokeniser* pParser )
{
	MexPoint3d location;
	MexRadians angle;
	for( int i = 0; i < pParser->tokens().size(); ++i )
		if( pParser->tokens()[i] == "AT" )
		{
			location.x( atof( pParser->tokens()[i+1].c_str() ) );
			location.y( atof( pParser->tokens()[i+2].c_str() ) );
			location.z( atof( pParser->tokens()[i+3].c_str() ) );
		}
		else if( pParser->tokens()[i] == "ANGLE" )
		{
			angle = MexRadians( MexDegrees( atof( pParser->tokens()[i+1].c_str() ) ) );
		}
	return _NEW( MachLogCameraViewCondition( pParser->tokens()[1], location, angle ) );
}

void MachLogCameraViewCondition::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachLogCameraViewCondition& t )
{

	t.doOutputOperator( o );
    return o;
}

//virtual
const PhysRelativeTime& MachLogCameraViewCondition::recommendedCallBackTimeGap() const
{
	static const PhysRelativeTime value = 5;
	return value;
}

//virtual
void MachLogCameraViewCondition::doOutputOperator( ostream& o ) const
{
	SimCondition::doOutputOperator( o );
    o << "MachLogCameraViewCondition " << (void*)this << " start" << std::endl;
}


void perWrite( PerOstream& ostr, const MachLogCameraViewCondition& condition )
{
	const SimCondition& base1 = condition;

	ostr << base1;
	ostr << condition.location_;
	ostr << condition.angle_;

}

void perRead( PerIstream& istr, MachLogCameraViewCondition& condition )
{
	SimCondition& base1 = condition;

	istr >> base1;
	istr >> condition.location_;
	istr >> condition.angle_;
}

MachLogCameraViewCondition::MachLogCameraViewCondition( PerConstructor con )
:	SimCondition( con )
{
}

/* End CONDTIME.CPP *************************************************/
