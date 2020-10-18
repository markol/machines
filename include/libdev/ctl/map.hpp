/*
 * M A P . H P P
 * Copyright (c) Charybdis Limited, 1996, 1997. All Rights Reserved.
 */

/*
    ctl_map

    This is a version of the STL map class that can be derived from (it
    has a virtual destructor). It has
    an added precondition on the [] operator that the map must
    already contain the value being looked up. This means that it is not
    possible to use the [] operator as follows:

        ctl_map< string, int, less< string > >  testMap;

        testMap[ "one" ] = 1;

    Instead, use the following:

        ctl_map< string, int, less< string > >  testMap;

        testMap.insert( "one", 1 );

    It is, however, possible to do this:

        ctl_map< string, int, less< string > >  testMap;

        testMap.insert( "one", 1 );
        testMap[ "one" ] = 100;


    ctl_multimap

    This is a version of the STL multimap class that can be derived from (it
    has a virtual destructor).
*/

#ifndef _CTL_MAP_HPP
#define _CTL_MAP_HPP

//////////////////////////////////////////////////////////////////////

#include <map>

//////////////////////////////////////////////////////////////////////

template < class KEY, class T, class COMPARE >
class ctl_map
: public std::map< KEY, T, COMPARE >
{
public:

    typedef std::map< KEY, T, COMPARE >                  rep_type;

    ///////////////////////////////

    using std::map< KEY, T, COMPARE >::map;

    virtual ~ctl_map( void ) { /* Intentionally empty */ }

    using std::map< KEY, T, COMPARE >::insert;
    std::pair<typename ctl_map<KEY,T,COMPARE>::iterator,bool>  insert (KEY key, T val)
    {
        return insert(std::pair< KEY, T >(key, val));
    }

    //ctl_map< KEY, T, COMPARE >&
    //operator =( const ctl_map< KEY, T, COMPARE >& );


protected:

    // value to insert if key is not present
    // in associative lookup
    //virtual T new_value() const;
};

template < class KEY, class T, class COMPARE >
class ctl_multimap
: public std::multimap< KEY, T, COMPARE >
{
public:

    typedef std::multimap< KEY, T, COMPARE >                  rep_type;

    ///////////////////////////////

    using std::multimap< KEY, T, COMPARE >::multimap;

    virtual ~ctl_multimap( void ) { /* Intentionally empty */ }

    //ctl_multimap< KEY, T, COMPARE >&
    //operator =( const ctl_multimap< KEY, T, COMPARE >& );


protected:

    // value to insert if key is not present
    // in associative lookup
    //virtual T new_value() const;
};

//////////////////////////////////////////////////////////////////////F

#ifdef  _INLINE
//    #include "ctl/map.itp"
 //   #include "ctl/map.itf"
#endif

#ifdef _INSTANTIATE_TEMPLATE_CLASSES
 //   #include "ctl/map.ctp"
#endif

#ifdef _INSTANTIATE_TEMPLATE_FUNCTIONS
 //   #include "ctl/map.ctf"
#endif

//////////////////////////////////////////////////////////////////////

// #undef Allocator

//////////////////////////////////////////////////////////////////////

#endif  /* #ifndef _ctl_multimap_HPP */

