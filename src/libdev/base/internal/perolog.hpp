/*
 * P E R O L O G . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    PersistenceObjectLog

    This class stores a classname and a pointer. It is for use in tracking
    whether an object has already been written out (debug versions only).
    It needs to store the class name as well as the pointer so that base
    classes with the same address are treated differently to their derived
    classes.
*/

#ifndef _BASE_PEROLOG_HPP
#define _BASE_PEROLOG_HPP

#include "base/base.hpp"

#include <string>

class PersistenceObjectLog
// Memberwise canonical
{
public:
    PersistenceObjectLog();
    PersistenceObjectLog( const char* classname, const void* ptr );

    ~PersistenceObjectLog();

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const PersistenceObjectLog& t );

private:

    std::string  classname_;
    const void*   ptr_;

    friend bool operator ==( const PersistenceObjectLog&, const PersistenceObjectLog& );
    friend bool operator <( const PersistenceObjectLog&, const PersistenceObjectLog& );
};



#endif

/* End PEROLOG.HPP **************************************************/
