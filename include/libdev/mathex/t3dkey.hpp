/*
 * T 3 D K E Y . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */


/*
    MexTransform3dKey

    Represents a unique key value which can be associated with a MexTransform3d.
	Each MexTransform3D contains a MexTransform3dKey by value, and uses the ++
	operator to modify the key each time the value of the transform is modified.
	At 10000 new or modified transforms per second, the key id will not repeat
	for 4.6 days. (If that is insufficient, this class can be modified to use a
	64-bit value, and not repeat for 50 million years or so).
*/

#ifndef _T3DKEY_HPP
#define _T3DKEY_HPP

//#include "base/persist.hpp"
#include "base/base.hpp"

//Forward refs
//class ostream;

//Orthodox canonical
class MexTransform3dKey
{
public:
    //Constructors
    MexTransform3dKey( void );
    MexTransform3dKey( const MexTransform3dKey& );

    //Operator overloads
    MexTransform3dKey& operator=( const MexTransform3dKey&);
    friend bool operator==( const MexTransform3dKey&, const MexTransform3dKey&);
    friend bool operator!=( const MexTransform3dKey&, const MexTransform3dKey&);

    //Use the ++ operator to generate a new unique key
    MexTransform3dKey& operator++();

    //Debugging
    void CLASS_INVARIANT;
    friend ostream& operator <<( ostream& o, const MexTransform3dKey& t );

/*    //Persistence
    PER_MEMBER_PERSISTENT;
    friend void perWrite( PerOstream& stream, const MexTransform3dKey& t );
    friend void perRead( PerIstream& stream, MexTransform3dKey& t );
    MexTransform3dKey( PerIstream& stream ); //ctor
*/
private:
    static ulong lastKey_; //Incremented on each use to keep unique
    ulong key_; //The unique key id, derived from lastKey_
};

//PER_DECLARE_PERSISTENT( MexTransform3dKey );

#ifdef _INLINE
    #include "mathex/t3dkey.ipp"
#endif


#endif

/* End T3DKEY.HPP ***************************************************/
