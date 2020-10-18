/*
 * D I S P M E S S . C P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machgui/dispmess.hpp"
#include "sim/manager.hpp"

// static
MachGuiMessageDisplay& MachGuiMessageDisplay::instance()
{
    static MachGuiMessageDisplay instance_;
    return instance_;
}

MachGuiMessageDisplay::MachGuiMessageDisplay()
{

    TEST_INVARIANT;
}

MachGuiMessageDisplay::~MachGuiMessageDisplay()
{
	while( messages_.size() )
	{
		_DELETE( messages_.front() );
		messages_.erase( messages_.begin() );
	}

    TEST_INVARIANT;

}

void MachGuiMessageDisplay::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

void MachGuiMessageDisplay::addMessage( const char* displayText, PhysRelativeTime timeToDisplay )
{
	DisplayMessage* dm = _NEW( DisplayMessage );
	dm->messageText_ = displayText;
	dm->duration_ = timeToDisplay;
	dm->startTime_ = SimManager::instance().currentTime();
	if( messages_.size() > 10 )
	{
		_DELETE( messages_.front() );
		messages_.erase( messages_.begin() );
	}
	messages_.push_back( dm );
}

void MachGuiMessageDisplay::doOutput( ostream& o )
{
	if( messages_.size() == 0 )
		return;
	for( int i = messages_.size() - 1; i > -1; )
	{
		if( ( messages_[i]->startTime_ + messages_[i]->duration_ ) < SimManager::instance().currentTime() )
		{
			_DELETE( messages_[i] );
			messages_.erase( messages_.begin() + i );
			if( i == 0 or i == messages_.size() )
				i = -1;
		}
		else
		{
			o << messages_[i]->messageText_ << std::endl;
			--i;
		}
	}
}

ostream& operator <<( ostream& o, const MachGuiMessageDisplay& t )
{

    o << "MachGuiMessageDisplay " << (void*)&t << " start" << std::endl;
    o << "MachGuiMessageDisplay " << (void*)&t << " end" << std::endl;

    return o;
}

/* End DISPMESS.CPP *************************************************/
