/*
 * W 9 5 T I M E I . I P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of inline non-template methods and inline global functions

#ifdef _INLINE
    #define _CODE_INLINE    inline
#else
    #define _CODE_INLINE
#endif

// TODO old code
unsigned int RDTSC2(uint32[ 2 ]);
#pragma aux RDTSC2 =   \
0x0f 0x31 /* RDTSC */ \
" mov [esi],eax   "   \
" mov [esi+4],edx "   \
parm [esi] modify [eax edx] value [eax];


// static
_CODE_INLINE
DevW95TimeInternal& DevW95TimeInternal::instance()
{
    static DevW95TimeInternal instance_;
    return instance_;
}

_CODE_INLINE
double  DevW95TimeInternal::resolution() const
{
    return resolution_;
}

_CODE_INLINE
DevW95TimeInternal::TimeMethod DevW95TimeInternal::method() const
{
    return method_;
}

_CODE_INLINE
double DevW95TimeInternal::pentiumTicks( void ) const
{
    uint32  timer[ 2 ];

    //RDTSC2( timer );

    double  ticks;

    ticks = timer[ 1 ];

    ticks *= 2 * 2 * 2 * 2 * 2 * 2 * 2 * 2;
    ticks *= 2 * 2 * 2 * 2 * 2 * 2 * 2 * 2;
    ticks *= 2 * 2 * 2 * 2 * 2 * 2 * 2 * 2;
    ticks *= 2 * 2 * 2 * 2 * 2 * 2 * 2 * 2;
    ticks += timer[ 0 ];

    return ticks;
}

/* End W95TIMEI.IPP *************************************************/
