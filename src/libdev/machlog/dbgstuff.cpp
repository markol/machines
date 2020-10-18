/*
 * D B G S T U F F . C P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machlog/dbgstuff.hpp"
#include <windows.h>

MachLogDebugStream::MachLogDebugStream()
:	active_( NULL != FindWindow( NULL, "recdbg" ) )
{
	sBuffer_[0]= 0;
    pStream_ = _NEW( ostrstream( sBuffer_, 1023, ios::out ) );
    TEST_INVARIANT;
}

//static
MachLogDebugStream& MachLogDebugStream::instance()
{
	static MachLogDebugStream m;
	return m;
}

MachLogDebugStream::~MachLogDebugStream()
{
	_DELETE( pStream_ );
    TEST_INVARIANT;

}

void MachLogDebugStream::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& MachLogDebugStream::ostr()
{
	return *pStream_;
}

void MachLogDebugStream::sendCBMessage()
{
	HWND hwndSend = FindWindow( NULL, "recdbg" );
	static int number = 0;
	++number;
	if( number > 999 )
		number = 0;
	if( hwndSend )
	{
		char sFileName[256];
		wsprintf( sFileName,"CBDBG%d",number);
		HANDLE handle = CreateFileMapping( (HANDLE)0xFFFFFFFF,
											NULL,
											PAGE_READWRITE,
											0,
											1000,
											sFileName );
		LPVOID map = MapViewOfFile( handle,
									FILE_MAP_WRITE,
									0,
									0,
									0 );
//		HAL_STREAM("MLDebugStream::sendCBMessage count " << pStream_->pcount() << std::endl );
		sBuffer_[ pStream_->pcount() ] = 0;
		lstrcpy( (char*)map,pStream_->str() );
		DWORD length = 0;
		WriteFile( handle, map, lstrlen( (char*)map ), &length, NULL );
		const UINT cb_debug_message = RegisterWindowMessage( "CB_DBG_MESSAGE" );
		SendMessage( hwndSend, cb_debug_message, 0, number );
		UnmapViewOfFile( map );
		CloseHandle( handle );
	}
//	HAL_STREAM("MLDebugStream pStream_->pcount() " << pStream_->pcount() << std::endl );
//	HAL_STREAM("MLDebugStream SBUFFER_ just sent:\n" << sBuffer_ << std::endl );
//	HAL_STREAM("MLDebugStream pStream_->str() just sent:\n" << pStream_->str() << std::endl );
//	MachLogDebugStream::sBuffer_[0] = 0;

	//move put buffer to back begining of buffer
	pStream_->seekp( ios::beg );
}

ostream& operator <<( ostream& o, const MachLogDebugStream& t )
{

    o << "MachLogDebugStream " << (void*)&t << " start" << std::endl;
    o << "MachLogDebugStream " << (void*)&t << " end" << std::endl;

    return o;
}

bool	MachLogDebugStream::active()
{
	return active_;
}

void	MachLogDebugStream::active( bool newActive )
{
	active_ = newActive;
}

/* End DBGSTUFF.CPP *************************************************/
