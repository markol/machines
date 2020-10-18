/*
 * C A M E R A I . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachLogCameraImpl

    A brief description of the class should go in here
*/

#ifndef _MACHLOG_CAMERAI_HPP
#define _MACHLOG_CAMERAI_HPP

#include "base/base.hpp"
#include "ctl/vector.hpp"
//#include "ctl/stack.hpp"
#include "machlog/camera.hpp"
#include "machphys/plansurf.hpp"
#include <stack>

class MachLogCameraImpl
// Canonical form revoked
{
public:
    MachLogCameraImpl( void );
    ~MachLogCameraImpl( void );

    void CLASS_INVARIANT;

private:
    friend ostream& operator <<( ostream& o, const MachLogCameraImpl& t );

    MachLogCameraImpl( const MachLogCameraImpl& );
    MachLogCameraImpl& operator =( const MachLogCameraImpl& );

	friend class MachLogCamera;

	// Data members...
    bool                    onPad_;
    MachLogConstruction*    pCurrentPadConstruction_;
    size_t                  currentPadEntrance_;
    MachLogCamera::Type     type_;
    bool                    insideConstruction_;
    Mathex::Side            outsideThreshold_;
    PhysConfigSpace2d*      pConfigSpace_;
    bool                    observingConstruction_; //True if have observer relation with pCurrentPadConstruction_

    //typedef ctl_stack< ctl_vector< std::pair< W4dEntity*, PhysConfigSpace2d* > > >   ParentStack;
    typedef std::stack< std::pair< W4dEntity*, PhysConfigSpace2d* > >   ParentStack;

    ParentStack parentStack_;
    W4dDomain* pForceDomain_; //camera must stay in this domain if not null
    MachPhysPlanetSurface::Floors   floors_;
};

#define CB_MachLogCamera_DEPIMPL() \
	CB_DEPIMPL( bool, onPad_ ); \
    CB_DEPIMPL( MachLogConstruction*, pCurrentPadConstruction_ ); \
    CB_DEPIMPL( size_t, currentPadEntrance_ ); \
    CB_DEPIMPL( Type, type_ ); \
    CB_DEPIMPL( bool, insideConstruction_ ); \
    CB_DEPIMPL( Mathex::Side, outsideThreshold_ ); \
    CB_DEPIMPL( PhysConfigSpace2d*, pConfigSpace_ ); \
    CB_DEPIMPL( bool, observingConstruction_ ); \
    CB_DEPIMPL( MachLogCameraImpl::ParentStack, parentStack_ ); \
    CB_DEPIMPL( W4dDomain*, pForceDomain_ ); \
    CB_DEPIMPL( MachPhysPlanetSurface::Floors, floors_ );

#endif

/* End CAMERAI.HPP **************************************************/
