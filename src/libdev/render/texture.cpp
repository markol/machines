/*
 * T E X T U R E . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

#include "render/texture.hpp"

#include "stdlib/string.hpp"
#include "render/surfmgr.hpp"
#include "render/texmgr.hpp"
#include "render/internal/texbody.hpp"
#include "render/internal/surfmgri.hpp"

PER_DEFINE_PERSISTENT( RenTexture );

RenTexture::RenTexture(Ren::TexId id):
	RenSurface(id)
{
	PRE(Ren::initialised());
    TEST_INVARIANT;
}

RenTexture::RenTexture()
{
    TEST_INVARIANT;
	POST(isNull() && sharable() && readOnly());
}

// virtual
RenTexture::~RenTexture()
{
    TEST_INVARIANT;
}

void RenTexture::CLASS_INVARIANT
{
	INVARIANT(RenTexManager::instance().impl().getTexBody(myId_) != NULL);
	INVARIANT(RenTexManager::instance().impl().getTexBody(myId_)->refCount() > 0);
}

void perWrite( PerOstream& ostr, const RenTexture& texture )
{
    //  Because we are simply using the name as a label we must
    //  bypass the normal persistence mechanism. Also make sure
    //  that we only write out the filename, not the full path.
    //  If we write out the full path it messes up the reading
    //  mechanism.

    //SysPathName pathName = texture.name();
    SysPathName pathName = string(texture.name());

    if( pathName.existsAsFile() )
        PER_WRITE_RAW_OBJECT( ostr, pathName.filename() );
    else
        //PER_WRITE_RAW_OBJECT( ostr, texture.name() );
        PER_WRITE_RAW_OBJECT( ostr, string(texture.name()) );
}

void perRead( PerIstream& istr, RenTexture& texture )
{
    string name;
    //  Because we are simply using the name as a label we must
    //  bypass the normal persistence mechanism.

    PER_READ_RAW_OBJECT( istr, name );

    if( name.length() > 0 )
        texture = RenTexManager::instance().createTexture( name );
}

void RenTexture::print(ostream& o) const
{
	if (isNull())
		o << "no texture";
	else
    	o << *(RenTexManager::instance().impl().getTexBody(myId()));
}

ostream& operator <<( ostream& o, const RenTexture& t )
{
	t.print(o);
    return o;
}

void RenTexture::incHits()
{
    TEST_INVARIANT;
	RenTexManager::instance().impl().getTexBody(myId())->incHits();
}

// Most methods are delegated to RenITexBody using these macros.
#define TEX_FORWARD_CONST(RETTYPE, METHOD)		\
RETTYPE RenTexture::METHOD() const				\
{												\
    TEST_INVARIANT;								\
	return RenSurfaceManager::instance().impl().getTexBody(myId())->METHOD();	\
}

TEX_FORWARD_CONST(uint,	 hitsPerFrame)
TEX_FORWARD_CONST(bool,	 hasAlphaTransparency)
TEX_FORWARD_CONST(bool,	 usesBilinear)

/* End TEXTURE.CPP **************************************************/
