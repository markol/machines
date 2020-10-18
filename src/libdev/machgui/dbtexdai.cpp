/*
 * D B T E X D A I . C P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machgui/internal/dbtexdai.hpp"

PER_DEFINE_PERSISTENT( MachGuiDbITextData );

MachGuiDbITextData::MachGuiDbITextData()
{
    objectives_.reserve( 2 );
	objectiveVoicemails_.reserve( 2 );
    tasks_.reserve( 2 );
    tasksStartAvailable_.reserve( 2 );

    TEST_INVARIANT;
}

MachGuiDbITextData::~MachGuiDbITextData()
{
    TEST_INVARIANT;

}

void MachGuiDbITextData::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachGuiDbITextData& t )
{

    o << "MachGuiDbITextData " << (void*)&t << " start" << std::endl;
    o << "MachGuiDbITextData " << (void*)&t << " end" << std::endl;

    return o;
}

void perWrite( PerOstream& ostr, const MachGuiDbITextData& ob )
{
    ostr << ob.campaignText_;
    ostr << ob.objectives_;
	ostr << ob.objectiveVoicemails_;
    ostr << ob.hintText_;
    ostr << ob.winText_;
    ostr << ob.loseText_;
    ostr << ob.tasks_;
	ostr << ob.winVoicemail_;
	ostr << ob.loseVoicemail_;
    ostr << ob.tasksStartAvailable_;
}

void perRead( PerIstream& istr, MachGuiDbITextData& ob )
{
    istr >> ob.campaignText_;
    istr >> ob.objectives_;
	istr >> ob.objectiveVoicemails_;
    istr >> ob.hintText_;
    istr >> ob.winText_;
    istr >> ob.loseText_;
    istr >> ob.tasks_;
	istr >> ob.winVoicemail_;
	istr >> ob.loseVoicemail_;
    istr >> ob.tasksStartAvailable_;
}
/* End DBTEXDAI.CPP *************************************************/
