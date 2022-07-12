/*
 * R O O T . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "world4d/root.hpp"

PER_DEFINE_PERSISTENT( W4dRoot );

W4dRoot::W4dRoot( W4dRootId id )
: id_( id ),
  descendantCount_(0),
  domainCount_(0),
  sceneManager_( NULL )
{
    TEST_INVARIANT;
}

W4dRoot::~W4dRoot()
{
    TEST_INVARIANT;
}

W4dSceneManager *W4dRoot::sceneManager()
{
	return sceneManager_;
}

void W4dRoot::sceneManager( W4dSceneManager* sceneManager )
{
	sceneManager_=sceneManager;
}

void W4dRoot::CLASS_INVARIANT
{
	INVARIANT(!hasParent());
// 	INVARIANT(descendantCount_ >= 1);		// includes self
// 	INVARIANT(descendantCount_ == computeDescendantCount());
// 	INVARIANT(descendantCount_ >= domainCount_);
}

// virtual
void W4dRoot::doDebugOutput( ostream& ostr )
// Write out the details of 'this' for debugging purposes.
{
    TEST_INVARIANT;
    ostr << "Start W4dRoot " << (void*)this << std::endl;
    ostr << "End   W4dRoot " << (void*)this << std::endl;
}

ostream& operator <<( ostream& o, const W4dRoot& t )
{

    o << "W4dRoot " << (void*)&t << " start" << std::endl;
    o << "W4dRoot " << (void*)&t << " end" << std::endl;

    return o;
}

int W4dRoot::descendantCount() const
{
    TEST_INVARIANT;
	return descendantCount_;
}

void W4dRoot::adjustDescendantCount(int adjustment)
{
	// The descendant count invariant won't work before the adjustment.
	descendantCount_ += adjustment;
    TEST_INVARIANT;
}

int W4dRoot::domainCount() const
{
    TEST_INVARIANT;
	return domainCount_;
}

void W4dRoot::adjustDomainCount(int adjustment)
{
	// The descendant count invariant won't work before the adjustment.
	domainCount_ += adjustment;
    TEST_INVARIANT;
}

bool W4dRoot::intersectsLine( const MexLine3d&, MATHEX_SCALAR*, Accuracy ) const
{
    return false;
}

//static
W4dRoot& W4dRoot::hiddenRoot()
{
    static W4dRoot theRoot( 777 );
    return theRoot;
}

void perWrite( PerOstream& ostr, const W4dRoot& root )
{
    const W4dEntity& base = root;

    ostr << base;
	ostr << root.descendantCount_;
    ostr << root.domainCount_;
    ostr << root.id_;
}

void perRead( PerIstream& istr, W4dRoot& root )
{
    W4dEntity& base = root;

    istr >> base;
	istr >> root.descendantCount_;
    istr >> root.domainCount_;
    istr >> root.id_;
}

W4dRoot::W4dRoot( PerConstructor con )
: W4dEntity( con )
{
}
/* End ROOT.CPP *****************************************************/
