/*
 * B A C K G R N D . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

#ifndef _RENDER_BACKGRND_HPP
#define _RENDER_BACKGRND_HPP

#include "base/base.hpp"

class RenBackground
{
public:
    RenBackground();
    ~RenBackground();
	void render() const;

private:
	RenIVertex* vertices_;

    RenBackground( const RenBackground& );
    RenBackground& operator =( const RenBackground& );
    bool operator ==( const RenBackground& );
};


#endif

/* End BACKGRND.HPP *************************************************/
