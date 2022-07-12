/*
 * L O G G E R . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "world4d/logger.hpp"

// static
W4dLogger& W4dLogger::instance()
{
    static W4dLogger instance_;
    return instance_;
}

W4dLogger::W4dLogger()
: renderLogStream_( "render.log" )
{

    TEST_INVARIANT;
}

W4dLogger::~W4dLogger()
{
    TEST_INVARIANT;

}

bool    W4dLogger::logTextureData() const
{
    return true;
}

std::ofstream& W4dLogger::textureLogStream()
{
    return renderLogStream_;
}

void W4dLogger::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const W4dLogger& t )
{

    o << "W4dLogger " << (void*)&t << " start" << std::endl;
    o << "W4dLogger " << (void*)&t << " end" << std::endl;

    return o;
}

/* End LOGGER.CPP ***************************************************/
