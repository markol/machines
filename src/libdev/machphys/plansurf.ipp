/*
 * P L A N S U R F . I P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of inline non-template methods and inline global functions

#ifdef _INLINE
    #define _CODE_INLINE    inline
#else
    #define _CODE_INLINE
#endif

//////////////////////////////////////////////////////////////////////////////////////////
_CODE_INLINE
size_t MachPhysPlanetSurface::nXTiles() const
{
    return nTilesX_;
}
//////////////////////////////////////////////////////////////////////////////////////////
_CODE_INLINE
size_t MachPhysPlanetSurface::nYTiles() const
{
    return nTilesY_;
}
//////////////////////////////////////////////////////////////////////////////////////////
_CODE_INLINE
MATHEX_SCALAR MachPhysPlanetSurface::xTileSpacing() const
{
    return edgeLength_;
}
//////////////////////////////////////////////////////////////////////////////////////////
_CODE_INLINE
MATHEX_SCALAR MachPhysPlanetSurface::yTileSpacing() const
{
    return edgeLength_;
}
//////////////////////////////////////////////////////////////////////////////////////////
_CODE_INLINE
const SysPathName& MachPhysPlanetSurface::backgroundFile() const
{
	return backgroundFile_;
}	
//////////////////////////////////////////////////////////////////////////////////////////
_CODE_INLINE
size_t MachPhysPlanetSurface::nTileXVertices() const
{
	return nTileXVertices_;
}
//////////////////////////////////////////////////////////////////////////////////////////
_CODE_INLINE
size_t MachPhysPlanetSurface::nTileYVertices() const
{
	return nTileYVertices_;
}
//////////////////////////////////////////////////////////////////////////////////////////

_CODE_INLINE
bool MachPhysPlanetSurface::hasEnvironment() const
{
    return pEnvironment_ != NULL;
}

_CODE_INLINE
EnvPlanetEnvironment& MachPhysPlanetSurface::environment() const
{
    PRE( hasEnvironment() );
    return *pEnvironment_;
}

_CODE_INLINE
int MachPhysPlanetSurface::xIndex(const MexPoint3d& position) const
{
	// TBD: this ought to use multiplication by 1/edgeLength_.	
    return _STATIC_CAST(int, position.x() / edgeLength_);
}

_CODE_INLINE
int MachPhysPlanetSurface::yIndex(const MexPoint3d& position) const
{
	// TBD: this ought to use multiplication by 1/edgeLength_.	
    return _STATIC_CAST(int, position.y() / edgeLength_);
}

/* End PLANSURF.IPP *************************************************/
