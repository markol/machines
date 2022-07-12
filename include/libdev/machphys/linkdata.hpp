/*
 * L I N K D A T A . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    LinkData

    A brief description of the class should go in here
*/

#ifndef _MACHPHYS_MEXPDATA_HPP
    #error This file must only be included from mexpdata.hpp
#endif

#ifndef _MACHPHYS_LINKDATA_HPP
#define _MACHPHYS_LINKDATA_HPP

class LinkData
{
public:
    //  Void constructor here purely to keep ctl_vector happy
    LinkData();
    //  POST( not initialised_ );

    //  shootOffProbability is the probability that the link
    //  will become detached from its parent and shoot off on
    //  its own. 
    LinkData(
        W4dLinkId linkId,
        MATHEX_SCALAR size,
        MATHEX_SCALAR shootOffProbability,
        const PhysRelativeTime& minExplosionTime,
        const PhysRelativeTime& maxExplosionTime );
    //  PRE( 0.0 <= shootOffProbability and shootOffProbability <= 1.0 );
    //  PRE( maxTime >= minTime );
    //  POST( initialised_ );
        
    //  Copy this data "as is"
    LinkData( const LinkData& copyMe );
    // PRE( copyMe.initialised_ );

    LinkData& operator =( const LinkData& copyMe );

    ~LinkData();

    //  Return the link to which this data refers        
    W4dLinkId linkId() const;
    //  PRE( initialised_ );
    //  POST( result != NULL );
         
    //  Return the size of the link
    MATHEX_SCALAR size() const;
    //  PRE( initialised_ );

    void size( MATHEX_SCALAR newSize );
         
    //  Return the size as one of 10 discrete sizes. All parts
    //  will be categorised in one of these sizes
    uint    discreteSize() const;
    // POST( 1 <= result and result <= 10 );

    void discreteSize( uint newDiscreteSize );
    //  PRE( 1 <= newDiscreteSize and newDiscreteSize <= 10 );
        
    //  Return the probability of this link becoming detached
    //  from its parent and shooting off
    MATHEX_SCALAR   shootOffProbability() const;
    //  PRE( initialised_ );
    // POST( 0.0 <= result and result <= 1.0 );

    void shootOffProbability( MATHEX_SCALAR newProbability );
    // PRE( 0.0 <= newProbability and newProbability <= 1.0 );

    //  Return the earliest time at which the link should explode
    const PhysRelativeTime& minExplosionTime() const;
    //  PRE( initialised_ );

    void minExplosionTime( const PhysRelativeTime& newTime );

    //  Return the latest time at which the link should explode
    const PhysRelativeTime& maxExplosionTime() const;
    //  PRE( initialised_ );

    void maxExplosionTime( const PhysRelativeTime& newTime );

    void CLASS_INVARIANT;

    PER_MEMBER_PERSISTENT_DEFAULT_INLINE( LinkData );
    PER_FRIEND_READ_WRITE( LinkData );
    
private:

    //  Data members

    bool    initialised_;
        
    friend bool operator ==( const LinkData& a, const LinkData& b );

    //  This is here purely for the ctl_list instantiation
    friend bool operator <( const LinkData& a, const LinkData& b );

    W4dLinkId        linkId_;
    MATHEX_SCALAR   size_;
    uint            discreteSize_;
    MATHEX_SCALAR   shootOffProbability_;
    PhysRelativeTime        minExplosionTime_;  // min Time for link explosion
    PhysRelativeTime        maxExplosionTime_;  // max time for link explosion

    friend ostream& operator <<( ostream& o, const LinkData& t );
};


#endif

/* End LINKDATA.HPP *************************************************/
