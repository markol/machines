/*
 * M E S H I D . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

#include "render/internal/meshid.hpp"

RenIMeshID::RenIMeshID()
{
}

RenIMeshID::RenIMeshID( const SysPathName& pathName, const std::string & combinedName, double scale )
: pathName_( pathName ),
  scale_( scale )
{
    ASSERT_INFO( pathName );
    ASSERT_INFO( combinedName );

	bool	separatorFound = false;
	size_t  separatorPosition;

	for( size_t i = 0; i < combinedName.length() and not separatorFound; ++i )
	{
		if( combinedName[ i ] == separator() )
        {
			separatorFound = true;
            separatorPosition = i;
        }
	}

	if( separatorFound )
	{
		instanceName_ = combinedName.substr( 0, separatorPosition );
		meshName_ = combinedName.substr( separatorPosition + 1, combinedName.length() - separatorPosition );
	}
	else
	{
		instanceName_ = combinedName;
	}
}

RenIMeshID::~RenIMeshID()
{
}

const SysPathName& RenIMeshID::pathName() const
{
	return pathName_;
}

const std::string   RenIMeshID::combinedName() const
{
    ASSERT_INFO( meshName_ );
    ASSERT_INFO( meshName_.length() );

    std::string  result;

	if( meshName_.length() == 0 )
		result = instanceName();
	else
		result = instanceName() + separator() + meshName();

    return result;
}

const std::string &  RenIMeshID::instanceName() const
{
	return instanceName_;
}

const std::string &  RenIMeshID::meshName() const
{
    if( meshName_ == "" )
        return instanceName_;
    else
        return meshName_;
}

double		RenIMeshID::scale() const
{
	return scale_;
}

bool RenIMeshID::operator< (const RenIMeshID& rhs) const
{
    bool    result;

    //  The equality check must be done first because
    //  of the odd nature of mesh id equality
    if( *this == rhs )
        result = false;
    else if( scale_ < rhs.scale_ )
        result = true;
    else if( rhs.scale_ < scale_ )
        result = false;
	else if (meshName() < rhs.meshName())
		result = true;
	else if( rhs.meshName() < meshName())
		result = false;
	else if (instanceName() < rhs.instanceName())
		result = true;
	else if (rhs.instanceName() < instanceName())
		result = false;
	else if (pathName() < rhs.pathName())
		result = true;
	else
        result = false;

    return result;
}

bool RenIMeshID::operator==(const RenIMeshID& rhs) const
{
    //  Compare initially on mesh name since this will allow us to share meshes

    bool    result = false;

    if( meshName_ != "" )
    {
        //  There is a specific mesh (as opposed to instance) name
        if( meshName() == rhs.meshName() && scale() == rhs.scale() )
            result = true;
    }
    else
    {
        result = pathName() == rhs.pathName() && instanceName() == rhs.instanceName() && meshName() == rhs.meshName() && scale() == rhs.scale();
    }

    return result;
}

// static
char RenIMeshID::separator( void )
{
    return 'X';
}

ostream& operator <<( ostream& o, const RenIMeshID& id )
{
    o << id.pathName() << " " << id.combinedName() << " " << id.scale();

    return o;
}
