/*
 * L O G G E R . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    W4dLogger

    Store flags, streams and general logging information for the render library.
*/

#ifndef _LOGGER_HPP
#define _LOGGER_HPP

#include <fstream>

#include "base/base.hpp"


class W4dLogger
{
public:
    //  Singleton class
    static W4dLogger& instance();
    ~W4dLogger();

    bool    logTextureData() const;
    std::ofstream& textureLogStream();

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const W4dLogger& t );

private:
    // Operation deliberately revoked
    W4dLogger( const W4dLogger& );

    // Operation deliberately revoked
    W4dLogger& operator =( const W4dLogger& );

    // Operation deliberately revoked
    bool operator ==( const W4dLogger& );

    W4dLogger();

    std::ofstream renderLogStream_;
};


#endif

/* End LOGGER.HPP ***************************************************/
