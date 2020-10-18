/*
 * L I G H T B U F . I P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

#ifdef _INLINE
    #define _CODE_INLINE    inline
#else
    #define _CODE_INLINE
#endif

_CODE_INLINE
void RenIFloatLightingBuffer::checkMaxLambertian(float r, float g, float b)
{
	if (r > maxLambertianR_)
		maxLambertianR_ = r;
	if (g > maxLambertianG_)
		maxLambertianG_ = g;
	if (b > maxLambertianB_)
		maxLambertianB_ = b;
}

_CODE_INLINE
void RenIFloatLightingBuffer::resetMaxLambertian()
{
	maxLambertianR_ = -10000;
	maxLambertianG_ = -10000;
	maxLambertianB_ = -10000;
}

_CODE_INLINE
float RenIFloatLightingBuffer::maxLambertianR() const
{
	return maxLambertianR_;
}	

_CODE_INLINE
float RenIFloatLightingBuffer::maxLambertianG() const
{
	return maxLambertianG_;
}	

_CODE_INLINE
float RenIFloatLightingBuffer::maxLambertianB() const
{
	return maxLambertianB_;
}	

/* End LIGHTBUF.IPP **************************************************/
