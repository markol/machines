/* 
 * K E Y . H P P
 * (c) Charybdis Limited, 1995. All Rights Reserved
 */
 
#ifndef _DEV_KEY_HPP
#define _DEV_KEY_HPP

#include "base/base.hpp"

// #include <iostream.h>
// #include <ctype.h>
// #include "clibplus/clibplus.hpp"
// #include "clibplus/limitw32.hpp"

//////////////////////////////////////////////////////////////////////

typedef int32 DevKeyCode;

//////////////////////////////////////////////////////////////////////

class DevKey
{
public:

	enum
	{
		BACKSPACE = 8,
		RETURN = 13,
		ESC = 27,
		SPACE = 32,
		F1 = 256, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,
		PGUP, PGDN, INSERT, HOME, END, DELETE,
		UP_ARROW, DOWN_ARROW, LEFT_ARROW, RIGHT_ARROW
	};

	DevKey( const DevKey& );
	DevKey( DevKeyCode value=0);

	~DevKey () {};

	DevKey &operator =( DevKeyCode );
	DevKey &operator =( const DevKey& );

	bool operator ==( const DevKey& ) const;
	bool operator !=( const DevKey& ) const;

	bool operator ==( DevKeyCode ) const;
	bool operator !=( DevKeyCode ) const;

	DevKeyCode value() const;
	void	setValue(DevKeyCode);

	uint32	status () const;
	void	setStatus (uint32);

	bool	shiftPressed () const;
	bool	altPressed () const;
	bool	ctrlPressed () const;

	friend ostream& operator <<( ostream&, const DevKey& );

	bool	getKey (void);
	bool	getKey (double waitTime);
	void	waitForKey (void);

private:
	
	DevKeyCode 	vk_value;
	uint32		vk_status;
};


inline DevKey::DevKey(DevKeyCode value)
: vk_value( value )
{
}

inline DevKey::DevKey( const DevKey& kk )
: vk_value( kk.vk_value ),
  vk_status( kk.vk_status )
{
}

inline DevKey &DevKey::operator =( DevKeyCode value )
{ 
	vk_value = value;
	vk_status = 0;

	return (*this);
}

inline DevKey &DevKey::operator=( const DevKey& k )
{ 
	vk_value = k.vk_value;
	vk_status = k.vk_status;

	return (*this);
}

inline bool DevKey::operator==( const DevKey& k ) const 
{
	return (vk_value == k.vk_value) && (vk_status == k.vk_status);
}

inline bool DevKey::operator !=( const DevKey& k ) const 
{
	return (vk_value != k.vk_value) || (vk_status != k.vk_status);
}

inline bool DevKey::operator ==( DevKeyCode ch ) const 
{
	return (vk_value == ch);
}

inline bool DevKey::operator !=( DevKeyCode ch ) const 
{
	return (vk_value != ch);
}

inline DevKeyCode DevKey::value() const
{
	return vk_value;
}

inline void DevKey::setValue(DevKeyCode newValue)
{
	vk_value = newValue;
}

inline uint32 DevKey::status() const
{
	return vk_status;
}

inline void DevKey::setStatus(uint32 newStatus)
{
	vk_status = newStatus;
}

inline bool DevKey::shiftPressed() const
{
	return ((vk_status & 0x03) ? true : false);
}

inline bool DevKey::altPressed() const
{
	return ((vk_status & 0xa00) ? true : false);
}

inline bool DevKey::ctrlPressed() const
{
	return ((vk_status & 0x500) ? true : false);
}

#endif
