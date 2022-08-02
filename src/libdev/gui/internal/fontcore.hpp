#ifndef _GUI_FONTCORE
#define _GUI_FONTCORE

#include "gui/gui.hpp"
#include "system/pathname.hpp"
#include "ctl/vector.hpp"

struct GuiBmpFontCoreCharData 
{ 
	size_t offset_; 
	size_t width_; 
	PER_MEMBER_PERSISTENT_DEFAULT( GuiBmpFontCoreCharData );
};

PER_READ_WRITE( GuiBmpFontCoreCharData );
PER_DECLARE_PERSISTENT( GuiBmpFontCoreCharData );

class GuiBmpFontCore
{
public:
	GuiBmpFontCore( const SysPathName& fontPath );

	void calculateProportionalFontWidthData( const SysPathName& persistFontPath );
	void createFromBinaryFile( const SysPathName& persistFontPath );

	bool endOfChar( int );

	GuiBitmap fontBmp_;
	ctl_vector< GuiBmpFontCoreCharData > charData_;
	SysPathName fontPath_;
	size_t coreCount_;
	size_t maxCharWidth_;
	size_t charHeight_;
};

#endif
