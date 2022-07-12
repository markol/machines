/*
 * G X I D . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

#ifndef _GXIDPos_HPP
#define _GXIDPos_HPP

#include "base/base.hpp"

#include "gx.hpp"
#include "gxerror.hpp"

typedef unsigned long GXId;

class GXIdPos {
// Canonical form revoked
public:
    GXIdPos();
    ~GXIdPos();
    GXIdPos( const GXIdPos& );
    GXIdPos& operator =( const GXIdPos& );
    friend bool operator ==(const GXIdPos&, const GXIdPos& );
    friend bool operator !=(const GXIdPos&, const GXIdPos& );
    friend bool operator < (const GXIdPos&, const GXIdPos& );

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const GXIdPos& t );

	const short& pos() const {
		return pos_;
	}

	void pos(short newpos) {
		pos_=newpos;
	}

	void gxid(GXId newgxid) {
		gxid_=newgxid;
	}

	const GXId& gxid() const {
		return gxid_;
	}

private:

	short pos_;
	GXId gxid_;

};

#endif

/* End GXIDPOS.HPP **************************************************/
