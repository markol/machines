/*
 * E N T Y F I L T . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    W4dEntityFilter

    A functionoid ABC for applying filters to entities (while processing a tree)


    W4dStandardFilter

    A filter based on standard features such as solidity and stationary flags.
*/

#ifndef _WORLD4D_ENTYFILT_HPP
#define _WORLD4D_ENTYFILT_HPP

#include "base/base.hpp"

//forward refs
class W4dEntity;

class W4dEntityFilter
// Canonical form revoked
{
public:
    virtual ~W4dEntityFilter();

    //Expresses options for continuing the tree processing
    enum TreeOption
    {
        PROCESS_SUBTREE, //Continue to process the tree normally
        SKIP_SUBTREE //Don't process the entity's children
    };

    //Override to perform specific filtering.
    //True if entity should be processed. 
    //Returns option for continued tree processing in pOption.
    virtual bool check( const W4dEntity& entity, TreeOption* pOption ) = 0;

    //True if no further processing should be performed
    bool isFinished() const;

    void CLASS_INVARIANT;

protected:
    W4dEntityFilter();

    //set the finished flag
    void finish();

private:
    //data members
    bool finished_; //True if all processing to be stopped
};

///////////////////////////////////////////////////////////////

class W4dStandardFilter : public W4dEntityFilter
{
public:
    //Set up the filter based on filterFlags, which should be a bitwise combination
    //of W4dDomain::EntityFilter enum members.
    W4dStandardFilter( int filterFlags );

    virtual ~W4dStandardFilter();

    //////////////////////////////////////////////
    // Inherited from W4dEntityFilter

    //True if entity should be processed. 
    //Returns option for continued tree processing in pOption.
    virtual bool check( const W4dEntity& entity, TreeOption* pOption );

    //////////////////////////////////////////////

private:
    int filterFlags_; //Flags as described in W4dDomain
};

#endif

/* End ENTYFILT.HPP *************************************************/
