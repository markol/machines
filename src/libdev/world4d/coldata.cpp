/*
 * C O L D A T A . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "world4d/coldata.hpp"

#include "stdlib/string.hpp"
#include "render/texture.hpp"
#include "render/texmgr.hpp"

PER_DEFINE_PERSISTENT(W4dColourPulseData);

W4dColourPulseData::W4dColourPulseData( const string& textureName,
				                        const RenColour& fromCol,
				                        const RenColour& toCol,
				                        const PhysRelativeTime& duration)
:	fromColour_( fromCol ),
	toColour_( toCol ),
	duration_( duration )
{
	size_t nameLength = textureName.length();

	if( nameLength == 0 )
	{
		keyTexture_ = RenTexture();
	}
	else
	{
		string name = textureName;

		if( nameLength <= 4 or (nameLength > 4 and (name.substr(nameLength-4, 4) != ".bmp" and name.substr(nameLength-4, 4) != ".BMP" ) ) )
		{
			name += ".bmp";
		}

	    keyTexture_ = RenTexManager::instance().createTexture( name );
	}

    TEST_INVARIANT;
}

W4dColourPulseData::~W4dColourPulseData()
{
    TEST_INVARIANT;

}

void W4dColourPulseData::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const W4dColourPulseData& t )
{

    o << "W4dColourPulseData " << (void*)&t << " start" << std::endl;
	o << " keyTexture_: " << t.keyTexture_ << std::endl;
	o << " fromColour_ = " << t.fromColour_ << std::endl;
	o << " toColour_ = " << t.toColour_ << std::endl;
	o << " duration = " << t.duration_ << std::endl;
    o << "W4dColourPulseData " << (void*)&t << " end" << std::endl;

    return o;
}

W4dColourPulseData::W4dColourPulseData( PerConstructor )
{
}

void perWrite( PerOstream& str, const W4dColourPulseData& t )
{
	str << t.keyTexture_;
	str << t.fromColour_;
	str << t.toColour_;
	str << t.duration_;
}

void perRead( PerIstream& str, W4dColourPulseData& t )
{
	str >> t.keyTexture_;
	str >> t.fromColour_;
	str >> t.toColour_;
	str >> t.duration_;
}

const RenTexture& W4dColourPulseData::keyTexture() const
{
	return keyTexture_;
}
const RenColour& W4dColourPulseData::fromColour() const
{
	return fromColour_;
}
const RenColour& W4dColourPulseData::toColour() const
{
	return toColour_;
}
const PhysRelativeTime& W4dColourPulseData::duration() const
{
	return duration_;
}

/* End COLDATA.CPP **************************************************/
