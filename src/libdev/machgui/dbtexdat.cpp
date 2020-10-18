/*
 * D B T E X D A T . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "base/persist.hpp"
#include "system/pathname.hpp"
#include "stdlib/string.hpp"
#include "utility/linetok.hpp"

#include "machgui/dbtexdat.hpp"
#include "machgui/internal/dbtexdai.hpp"

MachGuiDbTextData::MachGuiDbTextData()
{
    pData_ = _NEW( MachGuiDbITextData );

    TEST_INVARIANT;
}

MachGuiDbTextData::~MachGuiDbTextData()
{
    TEST_INVARIANT;

    _DELETE( pData_ );
}

void MachGuiDbTextData::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
	INVARIANT( pData_->objectives_.size() == pData_->objectiveVoicemails_.size() );
	INVARIANT( pData_->tasks_.size() == pData_->tasksStartAvailable_.size() );

}

ostream& operator <<( ostream& o, const MachGuiDbTextData& t )
{

    o << "MachGuiDbTextData " << (void*)&t << " start" << std::endl;
    o << "MachGuiDbTextData " << (void*)&t << " end" << std::endl;

    return o;
}

//static
MachGuiDbTextData* MachGuiDbTextData::pNewTextData( const string& filename )
{

    //Construct an empty object
    MachGuiDbTextData* pTextData = _NEW( MachGuiDbTextData );

    //Construct the persistent file name
    string work = "pdata/" + filename;
    SysPathName persistentPath( work );
    persistentPath.extension( "bin" );

    //If the persistent file exists..
    if( persistentPath.existsAsFile() )
    {
        //Open the input persistence stream
    	std::ifstream ifstr( persistentPath.c_str(), std::ios::binary );
    	PerIstream istr( ifstr );

    	istr >> *(pTextData->pData_);
    }
    else
    {
        //Construct the source path
        string work = "data/" + filename;
        SysPathName sourcePath( work );
        ASSERT_FILE_EXISTS( sourcePath.c_str() );

        //Read it in
        pTextData->parseFile( sourcePath );

        //save as persistent binary
    	std::ofstream ofstr( persistentPath.c_str(), std::ios::binary );
    	PerOstream ostr( ofstr );

    	ostr << *(pTextData->pData_);
    }
    return pTextData;
}

const string& MachGuiDbTextData::campaignText() const
{
	TEST_INVARIANT;
    return pData_->campaignText_;
}

const string& MachGuiDbTextData::hintText() const
{
	TEST_INVARIANT;
    return pData_->hintText_;
}

const string& MachGuiDbTextData::winText() const
{
	TEST_INVARIANT;
    return pData_->winText_;
}

const string& MachGuiDbTextData::loseText() const
{
	TEST_INVARIANT;
    return pData_->loseText_;
}

const string& MachGuiDbTextData::winVoicemail() const
{
	TEST_INVARIANT;
	return pData_->winVoicemail_;
}

const string& MachGuiDbTextData::loseVoicemail() const
{
	TEST_INVARIANT;
	return pData_->loseVoicemail_;
}

uint MachGuiDbTextData::nObjectives() const
{
	TEST_INVARIANT;
    return pData_->objectives_.size();
}

const string& MachGuiDbTextData::objectiveText( uint index ) const
{
	TEST_INVARIANT;
    PRE( index < nObjectives() );
    return pData_->objectives_[ index ];
}

const string& MachGuiDbTextData::objectiveVoicemail( uint index ) const
{
	TEST_INVARIANT;
    PRE( index < nObjectives() );
	return pData_->objectiveVoicemails_[ index ];
}

uint MachGuiDbTextData::nTasks() const
{
	TEST_INVARIANT;
    return pData_->tasks_.size();
}

const string& MachGuiDbTextData::taskText( uint index ) const
{
	TEST_INVARIANT;
    PRE( index < nTasks() );
    return pData_->tasks_[ index ];
}

void MachGuiDbTextData::parseFile( const SysPathName& filename )
{
	TEST_INVARIANT;
    //Open the file
    UtlLineTokeniser parser( filename );

    //Read successive line
    string *pOutputString = NULL;
    string objectiveText, taskText;
	string voicemail;

    while( not parser.finished() )
    {
        //Action depends on whether currently adding to a string
        if( pOutputString == NULL )
        {
            //Check for one of the section keywords
            if( parser.tokens()[0] == "CAMPAIGN" )
                pOutputString = &pData_->campaignText_;
            else if( parser.tokens()[0] == "HINTS" )
                pOutputString = &pData_->hintText_;
            else if( parser.tokens()[0] == "WIN" )
                pOutputString = &pData_->winText_;
            else if( parser.tokens()[0] == "LOSE" )
                pOutputString = &pData_->loseText_;
            else if( parser.tokens()[0] == "OBJECTIVE" )
            {
                //Check the numbers are sequential
                uint index = atol( parser.tokens()[1].c_str() );
                ASSERT_INFO( index );
                ASSERT( index == (pData_->objectives_.size() + 1), "Objective number out of sequence" );
                pOutputString = &objectiveText;
                objectiveText = "";
            }
            else if( parser.tokens()[0] == "TASK" )
            {
                //Check the numbers are sequential
                uint index = atol( parser.tokens()[1].c_str() );
                ASSERT_INFO( index );
                ASSERT( index == (pData_->tasks_.size() + 1), "Task number out of sequence" );
                pOutputString = &taskText;
                taskText = "";

                //Get the initial availability state  - default is available
                bool startsAvailable = true;
                if( parser.tokens().size() == 3 )
                {
                    if( parser.tokens()[2] == "UNAVAILABLE" )
                        startsAvailable = false;
                    else
                    {
                        ASSERT_INFO( parser.tokens()[2] );
                        ASSERT( parser.tokens()[2] == "OPEN", "Invalid task state keyword" );
                    }
                }

                pData_->tasksStartAvailable_.push_back( startsAvailable );
            }
            else
            {
                ASSERT_INFO( parser.tokens()[0] );
                ASSERT_FAIL( "Unrecognised keyword in text file" );
            }

            //Stop ignoring blank lines
            //parser.skipBlankLines( false );
        }
		else if( parser.tokens()[0] == "VOICEMAIL" )
		{
			voicemail = parser.tokens()[1];
        }
        else if( parser.tokens()[0] == "END" )
        {
            //Objective and task strings need to be pushed onto the collections
            if( pOutputString == &objectiveText )
			{
                pData_->objectives_.push_back( objectiveText );
				pData_->objectiveVoicemails_.push_back( voicemail );
				ASSERT( pData_->objectives_.size() == pData_->objectiveVoicemails_.size(),
						" objective data inconsistency" );
            }
            else if( pOutputString == &taskText )
                pData_->tasks_.push_back( taskText );
			else if( pOutputString == &pData_->winText_ )
				pData_->winVoicemail_ = voicemail;
			else if( pOutputString == &pData_->loseText_ )
				pData_->loseVoicemail_ = voicemail;

            //Finished processing current section
            pOutputString = NULL;
			voicemail = "";
            //parser.skipBlankLines( true );
        }
        else
        {
            //Add this line to the output string
            (*pOutputString) += parser.line();
        }
        parser.parseNextLine();
    }

    ASSERT( pOutputString == NULL, "Unexpected end of file" );
	TEST_INVARIANT;
}

bool MachGuiDbTextData::hasWinVoicemail() const
{
	TEST_INVARIANT;
	return pData_->winVoicemail_ != "";
}

bool MachGuiDbTextData::hasLoseVoicemail() const
{
	TEST_INVARIANT;
	return pData_->loseVoicemail_ != "";
}

bool MachGuiDbTextData::hasobjectiveVoicemail( uint index ) const
{
	TEST_INVARIANT;
	PRE( index < nObjectives() );
	return pData_->objectiveVoicemails_[index] != "";
}

bool MachGuiDbTextData::taskStartsAvailable( uint index ) const
{
	TEST_INVARIANT;
    PRE( index < nTasks() );
    return pData_->tasksStartAvailable_[index];
}

/* End DBTEXDAT.CPP *************************************************/
