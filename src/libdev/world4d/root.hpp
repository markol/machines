/*
 * R O O T . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */


/*
    W4dRoot

    Defines the root of the rendering tree. A W4dRoot is the only type of
    W4dEntity which does not have a parent.
*/

#ifndef _REN_ROOT_HPP
#define _REN_ROOT_HPP

#include "base/base.hpp"
#include "base/persist.hpp"

#include "world4d/entity.hpp"

class W4dSceneManager;

class W4dRoot : public W4dEntity
{
public:
    typedef size_t  W4dRootId;

    W4dRoot( W4dRootId id );
    ~W4dRoot();

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const W4dRoot& t );

	int descendantCount() const;
	void adjustDescendantCount(int);

	int domainCount() const;
	void adjustDomainCount(int);

	// get/set the pointer to the scene manager
	W4dSceneManager* sceneManager();
	void sceneManager( W4dSceneManager* );

    //Inherited from W4dEntity
    virtual bool intersectsLine( const MexLine3d& line, MATHEX_SCALAR* pDistance,
                             Accuracy accuracy ) const;

    //returns a useful root entity to act as parent for objects not
    //actually part of the displayed world (eg exemplars)
    static W4dRoot& hiddenRoot();

    PER_MEMBER_PERSISTENT_VIRTUAL( W4dRoot );
    PER_FRIEND_READ_WRITE( W4dRoot );

private:
    virtual void doDebugOutput( ostream& ostr );
    // Write out the details of 'this' for debugging purposes.

	int descendantCount_, domainCount_;

    // Operations deliberately revoked
    W4dRoot( const W4dRoot& );
    W4dRoot& operator =( const W4dRoot& );
    bool operator ==( const W4dRoot& );

    W4dRootId   id_;
	W4dSceneManager* sceneManager_;

};

PER_DECLARE_PERSISTENT( W4dRoot );


#endif

/* End ROOT.HPP *****************************************************/
