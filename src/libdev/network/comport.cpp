/*
 * C O M P O R T . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "network/comport.hpp"

NetComPortAddress::NetComPortAddress( size_t portNumber, BaudRate baudRate , StopBits stopBits, Parity parity, FlowControl flowControl )
: portNumber_( portNumber ),
  baudRate_( baudRate ),
  stopBits_( stopBits ),
  parity_( parity ),
  flowControl_( flowControl )
{

    TEST_INVARIANT;
}

NetComPortAddress::~NetComPortAddress()
{
    TEST_INVARIANT;

}

void NetComPortAddress::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const NetComPortAddress& t )
{

    o << "NetComPortAddress " << (void*)&t << " start" << std::endl;
    o << "NetComPortAddress " << (void*)&t << " end" << std::endl;

    return o;
}

size_t NetComPortAddress::portNumber() const
{
	return portNumber_;
}

void NetComPortAddress::portNumber( size_t newPortNumber )
{
	PRE( newPortNumber > 0 and newPortNumber < 5 );
	portNumber_ = newPortNumber;
}

NetComPortAddress::BaudRate NetComPortAddress::baudRate() const
{
	return baudRate_;
}

void NetComPortAddress::baudRate( NetComPortAddress::BaudRate newBaudRate )
{
	baudRate_ = newBaudRate;
}

NetComPortAddress::StopBits	NetComPortAddress::stopBits() const
{
	return stopBits_;
}

void NetComPortAddress::stopBits( NetComPortAddress::StopBits newStopBits )
{
	stopBits_ = newStopBits;
}

NetComPortAddress::Parity NetComPortAddress::parity() const
{
	return parity_;
}

void NetComPortAddress::parity( NetComPortAddress::Parity newParity )
{
	parity_ = newParity;
}

NetComPortAddress::FlowControl	NetComPortAddress::flowControl() const
{
	return flowControl_;
}

void NetComPortAddress::flowControl( NetComPortAddress::FlowControl newFlowControl )
{
	flowControl_ = newFlowControl;
}

/* End COMPORT.CPP **************************************************/
