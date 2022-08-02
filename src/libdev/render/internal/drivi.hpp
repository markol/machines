/*
 * D R I V I . H P P
 * (c) Charybdis Limited, 1999. All Rights Reserved
 */

/*
    RenIDriverImpl

    A brief description of the class should go in here
*/

#ifndef _DRIVI_HPP
#define _DRIVI_HPP

#include <string>
#include "base/base.hpp"

// In an ideal world this class should be derived in two subclasses:
// RenIDDrawDriverImpl, RenID3DDriverImpl... I can't be bothered.
// Actually I could!

class RenIDDrawDriverImpl;
class RenID3dDeviceImpl;

class RenIDriverImpl
{
public:

	// double dispatch here
	virtual bool isBetterChoiceThan( const RenIDriverImpl& ) const = 0;
	virtual bool isBetterChoiceThan( const RenIDDrawDriverImpl& ) const;
	virtual bool isBetterChoiceThan( const RenID3dDeviceImpl& ) const;

	bool hasGuid() const;
	bool isAutomatic() const;
	bool isHardware() const;
	virtual int zBufferBitDepth() const;
	// return -1

protected:
	// Direct draw driver
	RenIDriverImpl( const RenIDriverImpl& );
	virtual ~RenIDriverImpl();

	virtual RenIDriverImpl* clone() const = 0;

	const std::string& description() const;

	const std::string& name() const;
	void name( const std::string& );

	void isAutomatic( bool );

	virtual void writeToRegistry() = 0;


private:
	friend class RenDriver;
	friend class RenDriverSelector;
	friend class RenDriverSelectorImpl;

	std::string				name_;
	const std::string		description_;
	const bool 			hw_;
	bool				isAutomatic_;
};

//directdraw driver
class RenIDDrawDriverImpl :public RenIDriverImpl
{
public:


private:
	friend class RenDriverSelector;
	friend class RenDriverSelectorImpl;

	RenIDDrawDriverImpl( const RenIDDrawDriverImpl& );
	~RenIDDrawDriverImpl();

	virtual RenIDriverImpl* clone() const;

	virtual void writeToRegistry();

	// double dispatch here
	virtual bool isBetterChoiceThan( const RenIDriverImpl& ) const;
	virtual bool isBetterChoiceThan( const RenIDDrawDriverImpl& ) const;

};

// direct3d device
class RenID3dDeviceImpl : public RenIDriverImpl
{
public:
private:
	friend class RenDriverSelector;
	friend class RenDriverSelectorImpl;

	virtual int zBufferBitDepth() const;

	RenID3dDeviceImpl( const RenID3dDeviceImpl& );
	virtual ~RenID3dDeviceImpl();

	virtual RenIDriverImpl* clone() const;

	virtual void writeToRegistry();

	// double dispatch here
	virtual bool isBetterChoiceThan( const RenIDriverImpl& ) const;
	virtual bool isBetterChoiceThan( const RenID3dDeviceImpl& ) const;

};

#endif
/* End DRIVI.HPP **************************************************/
