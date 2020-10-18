/*
 *  S T A T S . I P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

#ifdef _INLINE
    #define _CODE_INLINE    inline
#else
    #define _CODE_INLINE
#endif

_CODE_INLINE
void RenStats::incrPolyCount(ulong number)
{
	polygonsDrawn_ += number;
}

_CODE_INLINE
void RenStats::incrLineCount(ulong number)
{
	linesDrawn_ += number;
}

_CODE_INLINE
void RenStats::incrTTFCount(ulong number)
{
	TTFsDrawn_ += number;
}

_CODE_INLINE
void RenStats::incrSTFCount(ulong number)
{
	STFsDrawn_ += number;
}

_CODE_INLINE
void RenStats::incrPointsCount(ulong number)
{
	pointsDrawn_ += number;
}

_CODE_INLINE
ulong RenStats::frameCount() const
{
	return frameCount_;
}	

/* End STATS.IPP **************************************************/
