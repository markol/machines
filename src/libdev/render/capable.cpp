/*
 * C A P A B L E . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

#include <string>
#include "system/pathname.hpp"
#include "utility/linetok.hpp"
#include "render/capable.hpp"
#include "render/internal/capablei.hpp"

#define CB_RenCapabilities_DEPIMPL() \
	CB_DEPIMPL( ulong,	maxAvailableTextureMemory_ ); \
	CB_DEPIMPL( bool,	maxAvailableTextureMemoryDirty_ ); \
	CB_DEPIMPL( ulong,	maxAvailableDisplayMemoryAfterTextures_ ); \
	CB_DEPIMPL( bool,	maxAvailableDisplayMemoryAfterTexturesDirty_ ); \
	CB_DEPIMPL( bool,	supports8BitsTexture_ ); \
	CB_DEPIMPL( bool,	supportsSharedVideoMemory_ ); \
	CB_DEPIMPL( bool,	supportsTextureSysMemory_ ); \
	CB_DEPIMPL( ulong, 	totalVideoMemory_ ); \
	CB_DEPIMPL( ulong, 	minDisplayMemory_ ); \
	CB_DEPIMPL( ulong, 	totalTextureMemory_ );

RenCapabilities::RenCapabilities(const RenDevice* dev, bool h):
	pImpl_(_NEW(RenICapabilities(dev, h)))
{
	ASSERT(pImpl_, "No pImpl for RenCapabilities");
}

RenCapabilities::~RenCapabilities()
{
	_DELETE(pImpl_);
}

bool RenCapabilities::supportsMMX() const
{
	PRE(pImpl_);
	return pImpl_->supportsMMX();
}

bool RenCapabilities::hardware() const
{
	PRE(pImpl_);
	return pImpl_->hardware();
}

bool RenCapabilities::supportsBilinear() const
{
	PRE(pImpl_);
	return pImpl_->supportsBilinear();
}

bool RenCapabilities::supportsFlatAlpha() const
{
	PRE(pImpl_);
	return pImpl_->supportsFlatAlpha();
}

bool RenCapabilities::supportsTextureAlpha() const
{
	PRE(pImpl_);
	return pImpl_->supportsTextureAlpha();
}

bool RenCapabilities::supportsStippledAlpha() const
{
	PRE(pImpl_);
	return pImpl_->supportsStippledAlpha();
}

bool RenCapabilities::supportsColourKey() const
{
	PRE(pImpl_);
	return pImpl_->supportsColourKey();
}

bool RenCapabilities::supportsFog() const
{
	PRE(pImpl_);
	return pImpl_->supportsFog();
}

bool RenCapabilities::supportsEdgeAntiAliasing() const
{
	PRE(pImpl_);
	return pImpl_->supportsEdgeAntiAliasing();
}

bool RenCapabilities::supportsGammaCorrection() const
{
	PRE(pImpl_);
	return pImpl_->supportsGammaCorrection();
}

const RenICapabilities* RenCapabilities::internal() const
{
	return pImpl_;
}

void RenCapabilities::parseCardSpecificationFile( const SysPathName& fileName )
{
	CB_RenCapabilities_DEPIMPL();

    PRE_INFO( fileName.pathname() );
    PRE( fileName.existsAsFile() );

	RENDER_INDENT(2);

    UtlLineTokeniser   parser( fileName );

    while( not parser.finished() )
    {
        if( parser.tokens()[0] == "MEMORY_AVAILABLE_FOR_TEXTURES" )
		{
		    ASSERT( parser.tokens().size() == 2, "Wrong file format" );
	        ulong mem = atof( parser.tokens()[ 1 ].c_str() );
			maxAvailableTextureMemory_ = mem;
			maxAvailableTextureMemoryDirty_ = false;
			RENDER_STREAM( "Read Memory available for textures: " << maxAvailableTextureMemory_ << " (" << ( maxAvailableTextureMemory_ / 0x100000 ) << " MBytes)" << std::endl);
		}
		else if( parser.tokens()[0] == "MEMORY_AVAILABLE_FOR_DISPLAY" )
		{
		    ASSERT( parser.tokens().size() == 2, "Wrong file format" );
	        ulong mem = atof( parser.tokens()[ 1 ].c_str() );
			maxAvailableDisplayMemoryAfterTextures_	= mem;
			maxAvailableDisplayMemoryAfterTexturesDirty_ = false;
			RENDER_STREAM( "Read Memory available for display: " << maxAvailableDisplayMemoryAfterTextures_ << " (" << ( maxAvailableDisplayMemoryAfterTextures_ / 0x100000 ) << " MBytes)" << std::endl);
		}
		else
		{
			ASSERT_INFO( parser.tokens()[0] );
        	ASSERT_FAIL( "Illegal token" );
		}
	    parser.parseNextLine();
	}
	RENDER_INDENT(-2);
}

void RenCapabilities::updateMaxAvailableDisplayMemoryAfterTextures()
{
	CB_RenCapabilities_DEPIMPL();

	SysPathName cardSpecsFile = "./vidcard.cap";

	RENDER_STREAM(std::endl);
	if( cardSpecsFile.existsAsFile() )
	{
		RENDER_STREAM("Attempting to read from specification file " << cardSpecsFile << " the max amount of memory that can be used for display" << std::endl);
		parseCardSpecificationFile( cardSpecsFile );
	}

	// is the display memory still unspecified ? if yes get it
	if( maxAvailableDisplayMemoryAfterTexturesDirty_ )
	{
		RENDER_STREAM("Working out how much memory is left for display from the memory used by textures" << std::endl);
		RENDER_INDENT(2);
		if( supportsSharedVideoMemory_ )
		{
			ulong totalTextureSystemMemory = 0;
			ulong memoryRequiredByTexSet = memoryRequiredByTextureSet();
			totalTextureSystemMemory = totalTextureMemory_ - totalVideoMemory_;
			RENDER_STREAM( "Total system memory available for textures: " << totalTextureSystemMemory << " (" << ( totalTextureSystemMemory / 0x100000 ) << " MBytes)" << std::endl);
			ulong videoMemoryRequiredForTextureSet = ( memoryRequiredByTexSet > totalTextureSystemMemory ) ? memoryRequiredByTexSet-totalTextureSystemMemory : 0;
			RENDER_STREAM( "Video memory required by texture set: " << videoMemoryRequiredForTextureSet << " (" << ( videoMemoryRequiredForTextureSet / 0x100000 ) << " MBytes)" << std::endl);
			maxAvailableDisplayMemoryAfterTextures_ = totalVideoMemory_ - videoMemoryRequiredForTextureSet;
			RENDER_STREAM( "Total video memory: " << totalVideoMemory_ << " (" << ( totalVideoMemory_ / 0x100000 ) << " MBytes)" << std::endl);
		}
		else
		{
			// Warning: this does not work if the device supports system texture memory
			maxAvailableDisplayMemoryAfterTextures_ = totalVideoMemory_ - totalTextureMemory_;
			RENDER_STREAM( "Total texture memory: " << totalTextureMemory_ << " (" << ( totalTextureMemory_ / 0x100000 ) << " MBytes)" << std::endl);
			RENDER_STREAM( "Total video memory: " << totalVideoMemory_ << " (" << ( totalVideoMemory_ / 0x100000 ) << " MBytes)" << std::endl);
		}
		RENDER_INDENT(-2);
	}

	maxAvailableDisplayMemoryAfterTexturesDirty_=false;
}

ulong RenCapabilities::maxAvailableDisplayMemoryAfterTextures() const
{
	CB_RenCapabilities_DEPIMPL();
	if( maxAvailableDisplayMemoryAfterTexturesDirty_ )
	{
		_CONST_CAST( RenCapabilities*, this )->updateMaxAvailableDisplayMemoryAfterTextures();
	}
	RENDER_STREAM("Memory available for display " << maxAvailableDisplayMemoryAfterTextures_ << " ("  << ( maxAvailableDisplayMemoryAfterTextures_ / 0x100000 ) << " MBytes)" <<  std::endl);
	return maxAvailableDisplayMemoryAfterTextures_;
}

ulong RenCapabilities::memoryRequiredByTextureSet() const
{
	CB_RenCapabilities_DEPIMPL();
	RENDER_STREAM( "Working out how much memory will be used by textures: " << std::endl);
	RENDER_INDENT(2);
	ulong result = 0x200000;
	if( supports4MBytesTextureSet() )
		result*=2;

	if( not supports8BitsTexture_ )
		result*=2;
	RENDER_STREAM( ((supports8BitsTexture_) ? "Supports " : "Does not support ") << "8 bit textures" << std::endl);

	RENDER_INDENT(-2);
	RENDER_STREAM( "Memory required by texture set: " << result << " (" << ( result / 0x100000 ) << " MBytes)" << std::endl);
	return result;
}

bool RenCapabilities::supports4MBytesTextureSet() const
{
	RENDER_STREAM(std::endl);
	RENDER_STREAM( "Does the card support a 4 mbytes texture set" << std::endl);
	RENDER_INDENT(2);

	ulong memoryRequiredBy4MBytesTexSet = pImpl_->memoryRequiredBy4MBytesTextureSet();
	RENDER_STREAM( "Memory required by the four Mbytes texture set: " << memoryRequiredBy4MBytesTexSet << " (" <<  ( memoryRequiredBy4MBytesTexSet / 0x100000 ) << " MBytes)" << std::endl);
	bool result = maxAvailableTextureMemory() > memoryRequiredBy4MBytesTexSet;

	RENDER_INDENT(-2);
	RENDER_STREAM( ((result) ? "Supports " : "Does not support ") << "4MB texture set" << std::endl);
	return result;
}

void RenCapabilities::updateMaxAvailableTextureMemory()
{
	CB_RenCapabilities_DEPIMPL();
	// return sthg else if we find a specs file

	SysPathName cardSpecsFile = "./vidcard.cap";

	if( cardSpecsFile.existsAsFile() )
	{
		RENDER_STREAM("Attempting to read from specification file " << cardSpecsFile << " the max amount of memory that can be used by textures" << std::endl);
		parseCardSpecificationFile( cardSpecsFile );
	}

	// is the display memory still unspecified ? if yes get it
	if( maxAvailableTextureMemoryDirty_ )
	{
		RENDER_STREAM("Working out how much memory can be used by textures" << std::endl);
		if( supportsSharedVideoMemory_ )
		{
			RENDER_STREAM( "Total shared texture memory: " << totalTextureMemory_ << " (" << ( totalTextureMemory_ / 0x100000 ) << " MBytes)" << std::endl);
			RENDER_STREAM( "Memory required for lowest allowed display: " << minDisplayMemory_ << " (" << ( minDisplayMemory_ / 0x100000 ) << " MBytes)" << std::endl);
			maxAvailableTextureMemory_ = totalTextureMemory_ - minDisplayMemory_;
		}
		else
		{
			maxAvailableTextureMemory_ = totalTextureMemory_;
		}
	}

	maxAvailableTextureMemoryDirty_=false;
}

ulong RenCapabilities::maxAvailableTextureMemory() const
{
	CB_RenCapabilities_DEPIMPL();
	if( maxAvailableTextureMemoryDirty_ )
	{
		_CONST_CAST( RenCapabilities*, this )->updateMaxAvailableTextureMemory();
	}
	RENDER_STREAM( "Memory available for textures: " << maxAvailableTextureMemory_ << " (" << ( maxAvailableTextureMemory_ / 0x100000 ) << " MBytes)" << std::endl);
	return maxAvailableTextureMemory_;
}

RenICapabilities* RenCapabilities::internal()
{
	return pImpl_;
}

ostream& operator <<( ostream& o, const RenCapabilities& t )
{
	PRE(t.pImpl_);
	t.pImpl_->write(o);
	return o;
}

/* End CAPABLE.CPP **************************************************/
