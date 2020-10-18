/*
 * M E S S A G E . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
 * Classes to support inter-actor messaging.
 */

#ifndef _MACHLOG_MESSAGE_HPP
#define _MACHLOG_MESSAGE_HPP

#include "machlog/machlog.hpp"

/* //////////////////////////////////////////////////////////////// */

class MachLogMessage
{
public:

	enum Text
	{
		IDLE,
		ENEMY_SIGHTED,
		UNABLE_TO_COMPLY
	};

	typedef MachLogCommsId Sender;
	
	MachLogMessage( Sender s, Text t )
	: sender_( s ),
	  text_( t ) {}

	virtual ~MachLogMessage() {};

	Text text() const { return text_; }
	Sender sender() const { return sender_; }

private:

	Sender	sender_;
	Text	text_;
};

/* //////////////////////////////////////////////////////////////// */

class MachLogCanCommunicate
{
public:

	MachLogCanCommunicate() {};	
	virtual ~MachLogCanCommunicate() {};

	virtual void sendMessage( MachLogCommsId to, const MachLogMessage& ) const = 0;
	virtual void receiveMessage( const MachLogMessage& message ) = 0;
	PER_MEMBER_PERSISTENT_DEFAULT_ABSTRACT( MachLogCanCommunicate );
};

PER_READ_WRITE( MachLogCanCommunicate );
PER_DECLARE_PERSISTENT( MachLogCanCommunicate );

/* //////////////////////////////////////////////////////////////// */

#endif	/*	#ifndef 	_MACHLOG_MESSAGE_HPP	*/

/* End MESSAGE. HPP *************************************************/
