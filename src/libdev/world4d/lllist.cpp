/*
 * L L L I S T . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

#include "world4d/lllist.hpp"
#include "world4d/light.hpp"
#include "world4d/manager.hpp"
#include "world4d/scenemgr.hpp"

#include "ctl/pvector.hpp"
#include <algorithm>

PER_DEFINE_PERSISTENT( W4dLocalLightList );

//#define DO_CULL_TRACING
#ifdef DO_CULL_TRACING
#define LIGHT_CULL_STREAM(x)	IAIN_STREAM(x)
#define LIGHT_CULL_TRACE(x)		x
#define LIGHT_CULL_INDENT(x)	IAIN_INDENT(x)
#else
#define LIGHT_CULL_STREAM(x)	;
#define LIGHT_CULL_TRACE(x)		;
#define LIGHT_CULL_INDENT(x)	;
#endif

W4dLocalLightList::W4dLocalLightList():
	pLights_(_NEW(Lights))
{
	pLights_->reserve(4);
    TEST_INVARIANT;
}

W4dLocalLightList::~W4dLocalLightList()
{
    TEST_INVARIANT;
	_DELETE(pLights_);

}

/*bool W4dLocalLightList::isInSceneMgrsList(W4dLight* pLightItem) const
{
	return W4dManager::instance().sceneManager()->pDevice()->pLights()->isInList(pLightItem);
}
*/

void W4dLocalLightList::removeLight(W4dLight* pLightItem)
{
    PRE( pLightItem );
    Lights::iterator it = find( pLights_->begin(), pLights_->end(), pLightItem );
    if( it != pLights_->end() )
    {
        pLights_->erase( it );
    }
}

void W4dLocalLightList::addLight(W4dLight* pLightItem)
{
    PRE(!pLightItem->isGlobal());
    pLights_->push_back( pLightItem );
}

void W4dLocalLightList::turnOnAll()
{
   	LIGHT_CULL_STREAM("Enabling light list " << (void*) this << " containing " << pLights_->size() << " lights\n");

    for( Lights:: iterator i = pLights_->begin(); i != pLights_->end(); ++i )
    {
        W4dLight& light = *(*i);
        if( light.hierarchicallyVisible() )
        {
            light.enable();
        	LIGHT_CULL_STREAM("  enabled light " << (W4dEntity*) &light << "\n");
        }
        else
		{
	        light.disable();
	    	LIGHT_CULL_STREAM("  disabled light " << (W4dEntity*) &light << "\n");
        }
    }
}

void W4dLocalLightList::turnOffAll()
{
   	LIGHT_CULL_STREAM("Disabling light list " << (void*) this << " containing " << pLights_->size() << " lights\n");

	// These are turned off in reverse order.  Thus the RenDevice's vector of
	// enabled lights should behave like a stack which is more efficient.
    Lights::iterator i = pLights_->end();
    while (i != pLights_->begin())
    {
		--i;
        W4dLight& light = *(*i);
        light.disable();
    	LIGHT_CULL_STREAM("  disabled light " << (W4dEntity*) &light << "\n");
    }
}

void W4dLocalLightList::CLASS_INVARIANT
{
	INVARIANT(this != NULL);
}

ostream& operator <<( ostream& o, const W4dLocalLightList& t )
{

    o << "W4dLocalLightList " << (void*)&t << " start" << std::endl;
    o << "W4dLocalLightList " << (void*)&t << " end" << std::endl;

    return o;
}

void perWrite( PerOstream& ostr, const W4dLocalLightList& lightList )
{
    //This is necessary because writing a ctl_pvector* causes a crash
    bool hasLights = lightList.pLights_ != NULL;
    PER_WRITE_RAW_OBJECT( ostr, hasLights );
    if( hasLights )
        ostr << *lightList.pLights_;
}

void perRead( PerIstream& istr, W4dLocalLightList& lightList )
{
	bool hasLights;
	PER_READ_RAW_OBJECT( istr, hasLights );
	if( hasLights )
	    istr >> *lightList.pLights_;
	else
		lightList.pLights_ = NULL;
}

void W4dLocalLightList::clearAllLights( W4dEntity* pEntity )
{
	while( pLights_->size() != 0 )
		pLights_->back()->dontIlluminate( pEntity );
}

/* End LLLIST.CPP ***************************************************/
