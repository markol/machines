/*
 * G X F I L E . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    GXFile

        This class should reproduce the behavior
        of the AGT file reading procedures of Gamut-X (agt_ut.c)
        without the mess of global variables and access functions
        spread everywhere. This class could be made singleton if we
                were sure one file at a time will be accessed in the
                application
*/

#ifndef _GXFILE_HPP
#define _GXFILE_HPP

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>

#include "base/base.hpp"
#include "ctl/vector.hpp"
#include <string>
#include "gxin/gx.hpp"
#include "gxin/gxerror.hpp"
#include "gxin/gxidpos.hpp"
#include "gxin/gxlabel.hpp"
#include "gxin/gxname.hpp"
#include "gxin/gxmesh.hpp"
#include "gxin/gxtextures.hpp"
#include "gxin/gxhier.hpp"
#include "gxin/gxvect4.hpp"
#include "gxin/gxvect3.hpp"

#define LINESIZE        1024

enum
{
  SECTION_START,
  SECTION_END,
  LABEL_TAG,
  AGID_TAG
};

typedef enum
{
 CHAR_T,
 SHORT_T,
 USHORT_T,
 LONG_T,
 ULONG_T,
 FLOAT_T,
 DOUBLE_T,
} GXNumTypes;


class GXFile
// Canonical form revoked
{
public:

    GXFile();
    ~GXFile();

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const GXFile& t );

    // Open a file for reading
    GXError open(const std::string & );
	// PRE(not isOpen())
	// POST(iff(isOpen(),result!=NULL)

    // Test whether file is opened or not
    bool isOpen();

    // Close the file
    void close();
    // POST(not isOpen())

    // Skip header section
    GXError skipHeader();
	// PRE(isOpen());

	// Find AGT section tag '{' '}' '&' '@'
	GXError findTag(short, bool);
	// PRE(isOpen());

	// Skip section that we don't want to read
	GXError skipSection(void);
	// PRE(isOpen());

	// Read Gamut-X mesh @TriMesh or @QuadMesh
	GXError readMesh(GXMesh& , const GXLabel& );
	// PRE(isOpen());

    // Read Gamut-X section &identifier
	GXError readGXIdPos(GXIdPos& , bool , bool , void* );
	// PRE(isOpen());

	// Read a Gamut-X label @label
	GXError readLabel(GXLabel& , bool );
	// PRE(isOpen());

	// Read the Gamut-X textures @Textures
	GXError readTextures(GXTextures& );

	GXError readHierarchy(GXHier& );

    GXError reportAGTError(GXError , void *);

private:
	// deliberately revoked: do not want two instances to access the same file
    GXFile( const GXFile& );
    GXFile& operator =( const GXFile& );
    bool operator ==( const GXFile& );

	// For all the following methods :
	// PRE(isOpen());

// @Hierarchy field

	GXError readChild( GXHier& );
	GXError readChildInfo( GXHier& , GXIdPos );
	GXError readTransform(GXHier& );
	GXError readTransformFromMatrix3X3(GXHier& );
	GXError readTransformFromMatrix4X4( GXHier& );
	GXError readComponentTransform( GXHier& );


// @Textures field

	GXError readBitmapTexture(GXTexture& );
	// Read a mesh made-up of triangles, method called by
	// readMesh which is the regular interface for
	// building meshes


// @Objects field

	GXError readTriMesh(GXMesh& );

    // Read a mesh made-up of triangles and quadrangles
	GXError readQuadMesh(GXMesh& );

    // Read the mesh's set of points
	GXError read3DPointArray(GXMesh& );

	// Read a point of the mesh
	GXError read3DPoint(GXPoint3& );

    // Read the mesh's set of normals
	GXError readNormalArray(GXMesh& );

	// Read an array of polygons
	GXError	readPolygonArray(GXMesh& , UCHAR );
    GXError readPolygon(GXPolygon3& , UCHAR );
    GXError readPolygonVertex(GXPolyVert3& );

    // Read a RGB color
	GXError readColor(GXColor& );

    // Read uv coords of texture
	GXError readUVCoords(GXUVCoords &uv);


// Common to multiple @ fields

    // Read a vector (normal), so far same function as
	// read3DPoint() but could be subject to changes if
	// Gamut-X file format evolves
	GXError readVector3(GXVect3& );

	GXError readVector4(GXVect4& );


    // Skip spaces and comments until first valid character is found
    GXError skipSpaces();

    // Get current char then increment cursor
    char getCharMoveToNext();

    // Get current char
    char getChar(void);

    // Increment cursor then read current character
    char moveToNextGetChar(void);

	// Increment column cursor (+1)
    void moveToNext();

 	// Increment column cursor (amount)
    void moveCursor(short );

	GXError getNextValidChar(char, bool);

    // Read a line from file in buffer
    GXError readLine();
	// POST(lineCount_==old(lineCount_+1));

    GXError readChar(char *);
    GXError readUChar(UCHAR *);
	GXError readShort(short *);
	GXError readUShort(USHORT *);
	GXError readLong(long *);
	// PRE(number);

    GXError readNumber(ULONG *, const GXNumTypes& );
    // PRE(number);

    GXError readFloat(float* );
	// PRE(number);

    GXError readFloatNum(double* , const GXNumTypes& );
	// PRE(number);

    GXError readName(GXName&, bool );

    // Could be out of this class
    bool validNumberDigit(char, char, short, GXNumTypes);
    // could be out of this class
    bool charIsWhiteSpace( char );
	// could be out of this class
	bool validHexDigit(char );

    static char numTypeChar_[8];


    FILE *inFile_;
    char currentLine_[LINESIZE]; // line buffer
    ULONG sectionCount_;
    short linePos_; // Current position in line buffer
    ULONG lineCount_;
};

#endif

/* End GXIFILE.HPP **************************************************/
