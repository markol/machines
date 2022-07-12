/*
 * C O M P O R T . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    NetComPortAddress

    This is a descriptor class for a COM port.
*/

#ifndef _NETWORK_COMPORT_HPP
#define _NETWORK_COMPORT_HPP

#include "base/base.hpp"

class NetComPortAddress
// Canonical form revoked
{
public:
	enum BaudRate {
		NET_BAUD_110, NET_BAUD_300, NET_BAUD_600,
		NET_BAUD_1200, NET_BAUD_2400, NET_BAUD_4800, 
		NET_BAUD_9600, NET_BAUD_14400, NET_BAUD_19200, 
		NET_BAUD_38400, NET_BAUD_56000, NET_BAUD_57600,
		NET_BAUD_115200, NET_BAUD_128000, NET_BAUD_256000 };
	enum StopBits { ONE_STOPBIT, ONE5_STOPBIT, TWO_STOPBITS };
	enum Parity { NO_PARITY, ODD_PARITY, EVEN_PARITY, MARK_PARITY };
	enum FlowControl { DTR_FLOW, NO_FLOW, RTSDTR_FLOW, RTS_FLOW, XONXOFF_FLOW };

    NetComPortAddress( size_t portNumber, BaudRate = NET_BAUD_19200, StopBits = ONE_STOPBIT, Parity = NO_PARITY, FlowControl = RTS_FLOW );
    ~NetComPortAddress();
	
	size_t		portNumber() const;
	void		portNumber( size_t );

	BaudRate 	baudRate() const;
	void		baudRate( BaudRate );

	StopBits	stopBits() const;
	void		stopBits( StopBits );

	Parity		parity() const;
	void		parity( Parity );

	FlowControl	flowControl() const;
	void		flowControl( FlowControl );

    void CLASS_INVARIANT;

private:
    friend ostream& operator <<( ostream& o, const NetComPortAddress& t );

	size_t		portNumber_;
	BaudRate 	baudRate_;
	StopBits	stopBits_;
	Parity		parity_;
	FlowControl	flowControl_;

};


#endif

/* End COMPORT.HPP **************************************************/
