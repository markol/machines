/*
---------------------------------------------------------------------------
Open Asset Import Library (assimp)
---------------------------------------------------------------------------

Copyright (c) 2006-2019, assimp team



All rights reserved.

Redistribution and use of this software in source and binary forms,
with or without modification, are permitted provided that the following
conditions are met:

* Redistributions of source code must retain the above
  copyright notice, this list of conditions and the
  following disclaimer.

* Redistributions in binary form must reproduce the above
  copyright notice, this list of conditions and the
  following disclaimer in the documentation and/or other
  materials provided with the distribution.

* Neither the name of the assimp team, nor the names of its
  contributors may be used to endorse or promote products
  derived from this software without specific prior
  written permission of the assimp team.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
---------------------------------------------------------------------------
*/

/** @file Implementation of the XFile parser helper class */


#include "xin/XFileParser.hpp"
#include "xin/XFileHelper.hpp"
#include <cstring>
#include <iostream>
#include "xin/fastatof.hpp"
#include "xin/formatter.hpp"
#include "base/base.hpp"

using namespace Formatter;

// ------------------------------------------------------------------------------------------------
// Constructor. Creates a data structure out of the XFile given in the memory block.
XFileParser::XFileParser( const std::vector<char>& pBuffer)
: mMajorVersion( 0 )
, mMinorVersion( 0 )
, mIsBinaryFormat( false )
, mBinaryNumCount( 0 )
, mP( NULL )
, mEnd( NULL )
, mLineNumber( 0 )
, mScene( NULL ) {
    // vector to store uncompressed file for INFLATE'd X files
    std::vector<char> uncompressed;

    // set up memory pointers
    mP = &pBuffer.front();
    mEnd = mP + pBuffer.size() - 1;

    // check header
    if ( 0 != strncmp( mP, "xof ", 4 ) )
    {
        throw XFile::DeadlyImportError( "Header mismatch, file is not an XFile." );
    }

    // read version. It comes in a four byte format such as "0302"
    mMajorVersion = (unsigned int)(mP[4] - 48) * 10 + (unsigned int)(mP[5] - 48);
    mMinorVersion = (unsigned int)(mP[6] - 48) * 10 + (unsigned int)(mP[7] - 48);

    bool compressed = false;

    // txt - pure ASCII text format
    if( strncmp( mP + 8, "txt ", 4) == 0)
        mIsBinaryFormat = false;

    // bin - Binary format
    else if( strncmp( mP + 8, "bin ", 4) == 0)
        mIsBinaryFormat = true;

    // tzip - Inflate compressed text format
    else if( strncmp( mP + 8, "tzip", 4) == 0)
    {
        mIsBinaryFormat = false;
        compressed = true;
    }
    // bzip - Inflate compressed binary format
    else if( strncmp( mP + 8, "bzip", 4) == 0)
    {
        mIsBinaryFormat = true;
        compressed = true;
    }
    else ThrowException( format() << "Unsupported xfile format '" <<
       mP[8] << mP[9] << mP[10] << mP[11] << "'");

    // float size
    mBinaryFloatSize = (unsigned int)(mP[12] - 48) * 1000
        + (unsigned int)(mP[13] - 48) * 100
        + (unsigned int)(mP[14] - 48) * 10
        + (unsigned int)(mP[15] - 48);

    if( mBinaryFloatSize != 32 && mBinaryFloatSize != 64)
        ThrowException( format() << "Unknown float size " << mBinaryFloatSize << " specified in xfile header." );

    // The x format specifies size in bits, but we work in bytes
    mBinaryFloatSize /= 8;

    mP += 16;

    // If this is a compressed X file, apply the inflate algorithm to it
    if (compressed)
    {
        throw XFile::DeadlyImportError("Assimp was built without compressed X support");
    }
    else
    {
        // start reading here
        ReadUntilEndOfLine();
    }

    mScene = new XFile::Scene;
    ParseFile();

    // filter the imported hierarchy for some degenerated cases
    if( mScene->mRootNode)
    {
        FilterHierarchy( mScene->mRootNode);
    }
}

// ------------------------------------------------------------------------------------------------
// Destructor. Destroys all imported data along with it
XFileParser::~XFileParser()
{
    // kill everything we created
    //delete mScene;
}

// ------------------------------------------------------------------------------------------------
void XFileParser::ParseFile()
{
    bool running = true;
    while( running )
    {
        // read name of next object
        std::string objectName = GetNextToken();
        if (objectName.length() == 0)
            break;

        // parse specific object
        if( objectName == "template")
            ParseDataObjectTemplate();
        else
        if( objectName == "Frame")
            ParseDataObjectFrame( NULL);
        else
        if( objectName == "Mesh")
        {
            // some meshes have no frames at all
            XFile::Mesh* mesh = new XFile::Mesh;
            ParseDataObjectMesh( mesh);
            mScene->mGlobalMeshes.push_back( mesh);
        }
        else
        if( objectName == "AnimTicksPerSecond")
            ParseDataObjectAnimTicksPerSecond();
        else
        if( objectName == "AnimationSet")
            ParseDataObjectAnimationSet();
        else
        if( objectName == "Material")
        {
            // Material outside of a mesh or node
            XFile::Material* material = new XFile::Material;
            ParseDataObjectMaterial( material);
            mScene->mGlobalMaterials.push_back( material);
        }
        else
        if( objectName == "}")
        {
            // whatever?
            LOG_WARN("} found in dataObject");
        }
        else
        {
            // unknown format
            LOG_WARN("Unknown data object in animation of .x file");
            ParseUnknownDataObject();
        }
    }
}

// ------------------------------------------------------------------------------------------------
void XFileParser::ParseDataObjectTemplate()
{
    // parse a template data object. Currently not stored.
    std::string name;
    readHeadOfDataObject( &name);

    // read GUID
    std::string guid = GetNextToken();

    // read and ignore data members
    bool running = true;
    while ( running )
    {
        std::string s = GetNextToken();

        if( s == "}")
            break;

        if( s.length() == 0)
            ThrowException( "Unexpected end of file reached while parsing template definition");
    }
}

// ------------------------------------------------------------------------------------------------
void XFileParser::ParseDataObjectFrame( XFile::Node* pParent)
{
    // A coordinate frame, or "frame of reference." The Frame template
    // is open and can contain any object. The Direct3D extensions (D3DX)
    // mesh-loading functions recognize Mesh, FrameTransformMatrix, and
    // Frame template instances as child objects when loading a Frame
    // instance.
    std::string name;
    readHeadOfDataObject(&name);

    // create a named node and place it at its parent, if given
    XFile::Node* node = new XFile::Node( pParent);
    node->mName = name;
    if( pParent)
    {
        pParent->mChildren.push_back( node);
    }
    else
    {
        // there might be multiple root nodes
        if( mScene->mRootNode != NULL)
        {
            // place a dummy root if not there
            if( mScene->mRootNode->mName != "$dummy_root")
            {
                XFile::Node* exroot = mScene->mRootNode;
                mScene->mRootNode = new XFile::Node( NULL);
                mScene->mRootNode->mName = "$dummy_root";
                mScene->mRootNode->mChildren.push_back( exroot);
                exroot->mParent = mScene->mRootNode;
            }
            // put the new node as its child instead
            mScene->mRootNode->mChildren.push_back( node);
            node->mParent = mScene->mRootNode;
        }
        else
        {
            // it's the first node imported. place it as root
            mScene->mRootNode = node;
        }
    }

    // Now inside a frame.
    // read tokens until closing brace is reached.
    bool running = true;
    while ( running )
    {
        std::string objectName = GetNextToken();
        if (objectName.size() == 0)
            ThrowException( "Unexpected end of file reached while parsing frame");

        if( objectName == "}")
            break; // frame finished
        else
        if( objectName == "Frame")
            ParseDataObjectFrame( node); // child frame
        else
        if( objectName == "FrameTransformMatrix")
            ParseDataObjectTransformationMatrix( node->mTrafoMatrix);
        else
        if( objectName == "Mesh")
        {
            XFile::Mesh* mesh = new XFile::Mesh(name);
            node->mMeshes.push_back( mesh);
            ParseDataObjectMesh( mesh);
        }
        else
        {
            LOG_WARN("Unknown data object in frame in x file");
            ParseUnknownDataObject();
        }
    }
}

// ------------------------------------------------------------------------------------------------
void XFileParser::ParseDataObjectTransformationMatrix( XFile::Matrix4x4& pMatrix)
{
    // read header, we're not interested if it has a name
    readHeadOfDataObject();

    // read its components
    pMatrix.a1 = ReadFloat(); pMatrix.b1 = ReadFloat();
    pMatrix.c1 = ReadFloat(); pMatrix.d1 = ReadFloat();
    pMatrix.a2 = ReadFloat(); pMatrix.b2 = ReadFloat();
    pMatrix.c2 = ReadFloat(); pMatrix.d2 = ReadFloat();
    pMatrix.a3 = ReadFloat(); pMatrix.b3 = ReadFloat();
    pMatrix.c3 = ReadFloat(); pMatrix.d3 = ReadFloat();
    pMatrix.a4 = ReadFloat(); pMatrix.b4 = ReadFloat();
    pMatrix.c4 = ReadFloat(); pMatrix.d4 = ReadFloat();

    // trailing symbols
    CheckForSemicolon();
    CheckForClosingBrace();
}

// ------------------------------------------------------------------------------------------------
void XFileParser::ParseDataObjectMesh( XFile::Mesh* pMesh)
{
    std::string name;
    readHeadOfDataObject( &name);
    pMesh->mName = name;

    // read vertex count
    unsigned int numVertices = ReadInt();
    pMesh->mPositions.resize( numVertices);

    // read vertices
    for( unsigned int a = 0; a < numVertices; a++)
        pMesh->mPositions[a] = ReadVector3();

    // read position faces
    unsigned int numPosFaces = ReadInt();
    pMesh->mPosFaces.resize( numPosFaces);
    for( unsigned int a = 0; a < numPosFaces; ++a) {
        // read indices
        unsigned int numIndices = ReadInt();
        XFile::Face& face = pMesh->mPosFaces[a];
        for (unsigned int b = 0; b < numIndices; ++b) {
            const int idx( ReadInt() );
            if ( static_cast<unsigned int>( idx ) <= numVertices )
            {
                face.mIndices.push_back( idx );
            }
        }
        TestForSeparator();
    }

    // here, other data objects may follow
    bool running = true;
    while ( running ) {
        std::string objectName = GetNextToken();

        if( objectName.empty() )
            ThrowException( "Unexpected end of file while parsing mesh structure");
        else
        if( objectName == "}")
            break; // mesh finished
        else
        if( objectName == "MeshNormals")
            ParseDataObjectMeshNormals( pMesh);
        else
        if( objectName == "MeshTextureCoords")
            ParseDataObjectMeshTextureCoords( pMesh);
        else
        if( objectName == "MeshVertexColors")
            ParseDataObjectMeshVertexColors( pMesh);
        else
        if( objectName == "MeshMaterialList")
            ParseDataObjectMeshMaterialList( pMesh);
        else
        if( objectName == "VertexDuplicationIndices")
            ParseUnknownDataObject(); // we'll ignore vertex duplication indices
        else
        if( objectName == "XSkinMeshHeader")
            ParseDataObjectSkinMeshHeader( pMesh);
        else
        if( objectName == "SkinWeights")
            ParseDataObjectSkinWeights( pMesh);
        else
        {
            LOG_WARN("Unknown data object in mesh in x file");
            ParseUnknownDataObject();
        }
    }
}

// ------------------------------------------------------------------------------------------------
void XFileParser::ParseDataObjectSkinWeights( XFile::Mesh *pMesh) {
    if ( NULL == pMesh )
    {
        return;
    }
    readHeadOfDataObject();

    std::string transformNodeName;
    GetNextTokenAsString( transformNodeName);

    pMesh->mBones.push_back( XFile::Bone());
    XFile::Bone& bone = pMesh->mBones.back();
    bone.mName = transformNodeName;

    // read vertex weights
    unsigned int numWeights = ReadInt();
    bone.mWeights.reserve( numWeights);

    for( unsigned int a = 0; a < numWeights; a++)
    {
        XFile::BoneWeight weight;
        weight.mVertex = ReadInt();
        bone.mWeights.push_back( weight);
    }

    // read vertex weights
    for( unsigned int a = 0; a < numWeights; a++)
        bone.mWeights[a].mWeight = ReadFloat();

    // read matrix offset
    bone.mOffsetMatrix.a1 = ReadFloat(); bone.mOffsetMatrix.b1 = ReadFloat();
    bone.mOffsetMatrix.c1 = ReadFloat(); bone.mOffsetMatrix.d1 = ReadFloat();
    bone.mOffsetMatrix.a2 = ReadFloat(); bone.mOffsetMatrix.b2 = ReadFloat();
    bone.mOffsetMatrix.c2 = ReadFloat(); bone.mOffsetMatrix.d2 = ReadFloat();
    bone.mOffsetMatrix.a3 = ReadFloat(); bone.mOffsetMatrix.b3 = ReadFloat();
    bone.mOffsetMatrix.c3 = ReadFloat(); bone.mOffsetMatrix.d3 = ReadFloat();
    bone.mOffsetMatrix.a4 = ReadFloat(); bone.mOffsetMatrix.b4 = ReadFloat();
    bone.mOffsetMatrix.c4 = ReadFloat(); bone.mOffsetMatrix.d4 = ReadFloat();

    CheckForSemicolon();
    CheckForClosingBrace();
}

// ------------------------------------------------------------------------------------------------
void XFileParser::ParseDataObjectSkinMeshHeader( XFile::Mesh* /*pMesh*/ )
{
    readHeadOfDataObject();

    /*unsigned int maxSkinWeightsPerVertex =*/ ReadInt();
    /*unsigned int maxSkinWeightsPerFace =*/ ReadInt();
    /*unsigned int numBonesInMesh = */ReadInt();

    CheckForClosingBrace();
}

// ------------------------------------------------------------------------------------------------
void XFileParser::ParseDataObjectMeshNormals( XFile::Mesh* pMesh)
{
    readHeadOfDataObject();

    // read count
    unsigned int numNormals = ReadInt();
    pMesh->mNormals.resize( numNormals);

    // read normal vectors
    for( unsigned int a = 0; a < numNormals; a++)
        pMesh->mNormals[a] = ReadVector3();

    // read normal indices
    unsigned int numFaces = ReadInt();
    if( numFaces != pMesh->mPosFaces.size())
        ThrowException( "Normal face count does not match vertex face count.");

    for( unsigned int a = 0; a < numFaces; a++)
    {
        unsigned int numIndices = ReadInt();
        pMesh->mNormFaces.push_back( XFile::Face());
        XFile::Face& face = pMesh->mNormFaces.back();

        for( unsigned int b = 0; b < numIndices; b++)
            face.mIndices.push_back( ReadInt());

        TestForSeparator();
    }

    CheckForClosingBrace();
}

// ------------------------------------------------------------------------------------------------
void XFileParser::ParseDataObjectMeshTextureCoords( XFile::Mesh* pMesh)
{
    readHeadOfDataObject();
    if( pMesh->mNumTextures + 1 > MAX_NUMBER_OF_TEXTURECOORDS)
        ThrowException( "Too many sets of texture coordinates");

    std::vector<XFile::Vector2D>& coords = pMesh->mTexCoords[pMesh->mNumTextures++];

    unsigned int numCoords = ReadInt();
    if( numCoords != pMesh->mPositions.size())
        ThrowException( "Texture coord count does not match vertex count");

    coords.resize( numCoords);
    for( unsigned int a = 0; a < numCoords; a++)
        coords[a] = ReadVector2();

    CheckForClosingBrace();
}

// ------------------------------------------------------------------------------------------------
void XFileParser::ParseDataObjectMeshVertexColors( XFile::Mesh* pMesh)
{
    readHeadOfDataObject();
    if( pMesh->mNumColorSets + 1 > MAX_NUMBER_OF_COLOR_SETS)
        ThrowException( "Too many colorsets");
    std::vector<XFile::Color4D>& colors = pMesh->mColors[pMesh->mNumColorSets++];

    unsigned int numColors = ReadInt();
    if( numColors != pMesh->mPositions.size())
        ThrowException( "Vertex color count does not match vertex count");

    colors.resize( numColors, XFile::Color4D( 0, 0, 0, 1));
    for( unsigned int a = 0; a < numColors; a++)
    {
        unsigned int index = ReadInt();
        if( index >= pMesh->mPositions.size())
            ThrowException( "Vertex color index out of bounds");

        colors[index] = ReadRGBA();
        // HACK: (thom) Maxon Cinema XPort plugin puts a third separator here, kwxPort puts a comma.
        // Ignore gracefully.
        if( !mIsBinaryFormat)
        {
            FindNextNoneWhiteSpace();
            if( *mP == ';' || *mP == ',')
                mP++;
        }
    }

    CheckForClosingBrace();
}

// ------------------------------------------------------------------------------------------------
void XFileParser::ParseDataObjectMeshMaterialList( XFile::Mesh* pMesh)
{
    readHeadOfDataObject();

    // read material count
    /*unsigned int numMaterials =*/ ReadInt();
    // read non triangulated face material index count
    unsigned int numMatIndices = ReadInt();

    // some models have a material index count of 1... to be able to read them we
    // replicate this single material index on every face
    if( numMatIndices != pMesh->mPosFaces.size() && numMatIndices != 1)
        ThrowException( "Per-Face material index count does not match face count.");

    // read per-face material indices
    for( unsigned int a = 0; a < numMatIndices; a++)
        pMesh->mFaceMaterials.push_back( ReadInt());

    // in version 03.02, the face indices end with two semicolons.
    // commented out version check, as version 03.03 exported from blender also has 2 semicolons
    if( !mIsBinaryFormat) // && MajorVersion == 3 && MinorVersion <= 2)
    {
        if(mP < mEnd && *mP == ';')
            ++mP;
    }

    // if there was only a single material index, replicate it on all faces
    while( pMesh->mFaceMaterials.size() < pMesh->mPosFaces.size())
        pMesh->mFaceMaterials.push_back( pMesh->mFaceMaterials.front());

    // read following data objects
    bool running = true;
    while ( running )
    {
        std::string objectName = GetNextToken();
        if( objectName.size() == 0)
            ThrowException( "Unexpected end of file while parsing mesh material list.");
        else
        if( objectName == "}")
            break; // material list finished
        else
        if( objectName == "{")
        {
            // template materials
            std::string matName = GetNextToken();
            XFile::Material material;
            // create reference if global materials exist
            material.mIsReference = !mScene->mGlobalMaterials.empty();
            material.sceneIndex = 0;
            material.mName = matName;

            for( uint i = 0; i < mScene->mGlobalMaterials.size(); ++i)
            {
                if(mScene->mGlobalMaterials[i]->mName == matName)
                {
                    material.sceneIndex = i;
                    break;
                }
            }

            pMesh->mMaterials.push_back( material);

            CheckForClosingBrace(); // skip }
        }
        else if( objectName == "Material")
        {
            pMesh->mMaterials.push_back( XFile::Material());
            ParseDataObjectMaterial( &pMesh->mMaterials.back());
        }
        else if( objectName == ";")
        {
            // ignore
        }
        else
        {
            LOG_WARN("Unknown data object in material list in x file");
            ParseUnknownDataObject();
        }
    }
}

// ------------------------------------------------------------------------------------------------
void XFileParser::ParseDataObjectMaterial( XFile::Material* pMaterial)
{
    std::string matName;
    readHeadOfDataObject( &matName);
    if( matName.empty())
        matName = std::string( "material") + to_string( mLineNumber );
    pMaterial->mName = matName;
    pMaterial->mIsReference = false;

    // read material values
    pMaterial->mDiffuse = ReadRGBA();
    pMaterial->mSpecularExponent = ReadFloat();
    pMaterial->mSpecular = ReadRGB();
    pMaterial->mEmissive = ReadRGB();

    // read other data objects
    bool running = true;
    while ( running )
    {
        std::string objectName = GetNextToken();
        if( objectName.size() == 0)
            ThrowException( "Unexpected end of file while parsing mesh material");
        else
        if( objectName == "}")
            break; // material finished
        else
        if( objectName == "TextureFilename" || objectName == "TextureFileName")
        {
            // some exporters write "TextureFileName" instead.
            std::string texname;
            ParseDataObjectTextureFilename( texname);
            pMaterial->mTextures.push_back( XFile::TexEntry( texname));
        } else
        if( objectName == "NormalmapFilename" || objectName == "NormalmapFileName")
        {
            // one exporter writes out the normal map in a separate filename tag
            std::string texname;
            ParseDataObjectTextureFilename( texname);
            pMaterial->mTextures.push_back( XFile::TexEntry( texname, true));
        }
        else
        {
            LOG_WARN("Unknown data object in material in x file");
            ParseUnknownDataObject();
        }
    }
}

// ------------------------------------------------------------------------------------------------
void XFileParser::ParseDataObjectAnimTicksPerSecond()
{
    readHeadOfDataObject();
    mScene->mAnimTicksPerSecond = ReadInt();
    CheckForClosingBrace();
}

// ------------------------------------------------------------------------------------------------
void XFileParser::ParseDataObjectAnimationSet()
{
    std::string animName;
    readHeadOfDataObject( &animName);

    XFile::Animation* anim = new XFile::Animation;
    mScene->mAnims.push_back( anim);
    anim->mName = animName;

    bool running = true;
    while ( running )
    {
        std::string objectName = GetNextToken();
        if( objectName.length() == 0)
            ThrowException( "Unexpected end of file while parsing animation set.");
        else
        if( objectName == "}")
            break; // animation set finished
        else
        if( objectName == "Animation")
            ParseDataObjectAnimation( anim);
        else
        {
            LOG_WARN("Unknown data object in animation set in x file");
            ParseUnknownDataObject();
        }
    }
}

// ------------------------------------------------------------------------------------------------
void XFileParser::ParseDataObjectAnimation( XFile::Animation* pAnim)
{
    readHeadOfDataObject();
    XFile::AnimBone* banim = new XFile::AnimBone;
    pAnim->mAnims.push_back( banim);

    bool running = true;
    while( running )
    {
        std::string objectName = GetNextToken();

        if( objectName.length() == 0)
            ThrowException( "Unexpected end of file while parsing animation.");
        else
        if( objectName == "}")
            break; // animation finished
        else
        if( objectName == "AnimationKey")
            ParseDataObjectAnimationKey( banim);
        else
        if( objectName == "AnimationOptions")
            ParseUnknownDataObject(); // not interested
        else
        if( objectName == "{")
        {
            // read frame name
            banim->mBoneName = GetNextToken();
            CheckForClosingBrace();
        }
        else
        {
            LOG_WARN("Unknown data object in animation in x file");
            ParseUnknownDataObject();
        }
    }
}

// ------------------------------------------------------------------------------------------------
void XFileParser::ParseDataObjectAnimationKey( XFile::AnimBone* pAnimBone)
{
    readHeadOfDataObject();

    // read key type
    unsigned int keyType = ReadInt();

    // read number of keys
    unsigned int numKeys = ReadInt();

    for( unsigned int a = 0; a < numKeys; a++)
    {
        // read time
        unsigned int time = ReadInt();

        // read keys
        switch( keyType)
        {
            case 0: // rotation quaternion
            {
                // read count
                if( ReadInt() != 4)
                    ThrowException( "Invalid number of arguments for quaternion key in animation");

                XFile::QuatKey key;
                key.mTime = double( time);
                key.mValue.w = ReadFloat();
                key.mValue.x = ReadFloat();
                key.mValue.y = ReadFloat();
                key.mValue.z = ReadFloat();
                pAnimBone->mRotKeys.push_back( key);

                CheckForSemicolon();
                break;
            }

            case 1: // scale vector
            case 2: // position vector
            {
                // read count
                if( ReadInt() != 3)
                    ThrowException( "Invalid number of arguments for vector key in animation");

                XFile::VectorKey key;
                key.mTime = double( time);
                key.mValue = ReadVector3();

                if( keyType == 2)
                    pAnimBone->mPosKeys.push_back( key);
                else
                    pAnimBone->mScaleKeys.push_back( key);

                break;
            }

            case 3: // combined transformation matrix
            case 4: // denoted both as 3 or as 4
            {
                // read count
                if( ReadInt() != 16)
                    ThrowException( "Invalid number of arguments for matrix key in animation");

                // read matrix
                XFile::MatrixKey key;
                key.mTime = double( time);
                key.mMatrix.a1 = ReadFloat(); key.mMatrix.b1 = ReadFloat();
                key.mMatrix.c1 = ReadFloat(); key.mMatrix.d1 = ReadFloat();
                key.mMatrix.a2 = ReadFloat(); key.mMatrix.b2 = ReadFloat();
                key.mMatrix.c2 = ReadFloat(); key.mMatrix.d2 = ReadFloat();
                key.mMatrix.a3 = ReadFloat(); key.mMatrix.b3 = ReadFloat();
                key.mMatrix.c3 = ReadFloat(); key.mMatrix.d3 = ReadFloat();
                key.mMatrix.a4 = ReadFloat(); key.mMatrix.b4 = ReadFloat();
                key.mMatrix.c4 = ReadFloat(); key.mMatrix.d4 = ReadFloat();
                pAnimBone->mTrafoKeys.push_back( key);

                CheckForSemicolon();
                break;
            }

            default:
                ThrowException( format() << "Unknown key type " << keyType << " in animation." );
                break;
        } // end switch

        // key separator
        CheckForSeparator();
    }

    CheckForClosingBrace();
}

// ------------------------------------------------------------------------------------------------
void XFileParser::ParseDataObjectTextureFilename( std::string& pName)
{
    readHeadOfDataObject();
    GetNextTokenAsString( pName);
    CheckForClosingBrace();

    // FIX: some files (e.g. AnimationTest.x) have "" as texture file name
    if (!pName.length())
    {
        LOG_WARN("Length of texture file name is zero. Skipping this texture.");
    }

    // some exporters write double backslash paths out. We simply replace them if we find them
    while( pName.find( "\\\\") != std::string::npos)
        pName.replace( pName.find( "\\\\"), 2, "\\");
}

// ------------------------------------------------------------------------------------------------
void XFileParser::ParseUnknownDataObject()
{
    // find opening delimiter
    bool running = true;
    while( running )
    {
        std::string t = GetNextToken();
        if( t.length() == 0)
            ThrowException( "Unexpected end of file while parsing unknown segment.");

        if( t == "{")
            break;
    }

    unsigned int counter = 1;

    // parse until closing delimiter
    while( counter > 0)
    {
        std::string t = GetNextToken();

        if( t.length() == 0)
            ThrowException( "Unexpected end of file while parsing unknown segment.");

        if( t == "{")
            ++counter;
        else
        if( t == "}")
            --counter;
    }
}

// ------------------------------------------------------------------------------------------------
//! checks for closing curly brace
void XFileParser::CheckForClosingBrace()
{
    if( GetNextToken() != "}")
        ThrowException( "Closing brace expected.");
}

// ------------------------------------------------------------------------------------------------
//! checks for one following semicolon
void XFileParser::CheckForSemicolon()
{
    if( mIsBinaryFormat)
        return;

    if( GetNextToken() != ";")
        ThrowException( "Semicolon expected.");
}

// ------------------------------------------------------------------------------------------------
//! checks for a separator char, either a ',' or a ';'
void XFileParser::CheckForSeparator()
{
    if( mIsBinaryFormat)
        return;

    std::string token = GetNextToken();
    if( token != "," && token != ";")
        ThrowException( "Separator character (';' or ',') expected.");
}

// ------------------------------------------------------------------------------------------------
// tests and possibly consumes a separator char, but does nothing if there was no separator
void XFileParser::TestForSeparator()
{
  if( mIsBinaryFormat)
    return;

  FindNextNoneWhiteSpace();
  if( mP >= mEnd)
    return;

  // test and skip
  if( *mP == ';' || *mP == ',')
    mP++;
}

// ------------------------------------------------------------------------------------------------
void XFileParser::readHeadOfDataObject( std::string* poName)
{
    std::string nameOrBrace = GetNextToken();
    if( nameOrBrace != "{")
    {
        if( poName)
            *poName = nameOrBrace;

        if ( GetNextToken() != "{" ) {
            delete mScene;
            ThrowException( "Opening brace expected." );
        }
    }
}

// ------------------------------------------------------------------------------------------------
std::string XFileParser::GetNextToken() {
    std::string s;

    // process binary-formatted file
    if( mIsBinaryFormat) {
        // in binary mode it will only return NAME and STRING token
        // and (correctly) skip over other tokens.
        if ( mEnd - mP < 2 ) {
            return s;
        }
        unsigned int tok = ReadBinWord();
        unsigned int len;

        // standalone tokens
        switch( tok ) {
            case 1: {
                // name token
                if ( mEnd - mP < 4 ) {
                    return s;
                }
                len = ReadBinDWord();
                const int bounds = int( mEnd - mP );
                const int iLen   = int( len );
                if ( iLen < 0 ) {
                    return s;
                }
                if ( bounds < iLen ) {
                    return s;
                }
                s = std::string( mP, len );
                mP += len;
            }
            return s;

            case 2:
                // string token
                if( mEnd - mP < 4) return s;
                len = ReadBinDWord();
                if( mEnd - mP < int(len)) return s;
                s = std::string(mP, len);
                mP += (len + 2);
                return s;
            case 3:
                // integer token
                mP += 4;
                return "<integer>";
            case 5:
                // GUID token
                mP += 16;
                return "<guid>";
            case 6:
                if( mEnd - mP < 4) return s;
                len = ReadBinDWord();
                mP += (len * 4);
                return "<int_list>";
            case 7:
                if( mEnd - mP < 4) return s;
                len = ReadBinDWord();
                mP += (len * mBinaryFloatSize);
                return "<flt_list>";
            case 0x0a:
                return "{";
            case 0x0b:
                return "}";
            case 0x0c:
                return "(";
            case 0x0d:
                return ")";
            case 0x0e:
                return "[";
            case 0x0f:
                return "]";
            case 0x10:
                return "<";
            case 0x11:
                return ">";
            case 0x12:
                return ".";
            case 0x13:
                return ",";
            case 0x14:
                return ";";
            case 0x1f:
                return "template";
            case 0x28:
                return "WORD";
            case 0x29:
                return "DWORD";
            case 0x2a:
                return "FLOAT";
            case 0x2b:
                return "DOUBLE";
            case 0x2c:
                return "CHAR";
            case 0x2d:
                return "UCHAR";
            case 0x2e:
                return "SWORD";
            case 0x2f:
                return "SDWORD";
            case 0x30:
                return "void";
            case 0x31:
                return "string";
            case 0x32:
                return "unicode";
            case 0x33:
                return "cstring";
            case 0x34:
                return "array";
        }
    }
    // process text-formatted file
    else
    {
        FindNextNoneWhiteSpace();
        if( mP >= mEnd)
            return s;

        while( (mP < mEnd) && !isspace( (unsigned char) *mP))
        {
            // either keep token delimiters when already holding a token, or return if first valid char
            if( *mP == ';' || *mP == '}' || *mP == '{' || *mP == ',')
            {
                if( !s.size())
                    s.append( mP++, 1);
                break; // stop for delimiter
            }
            s.append( mP++, 1);
        }
    }
    return s;
}

// ------------------------------------------------------------------------------------------------
void XFileParser::FindNextNoneWhiteSpace()
{
    if( mIsBinaryFormat)
        return;

    bool running = true;
    while( running )
    {
        while( mP < mEnd && isspace( (unsigned char) *mP))
        {
            if( *mP == '\n')
                mLineNumber++;
            ++mP;
        }

        if( mP >= mEnd)
            return;

        // check if this is a comment
        if( (mP[0] == '/' && mP[1] == '/') || mP[0] == '#')
            ReadUntilEndOfLine();
        else
            break;
    }
}

// ------------------------------------------------------------------------------------------------
void XFileParser::GetNextTokenAsString( std::string& poString)
{
    if( mIsBinaryFormat)
    {
        poString = GetNextToken();
        return;
    }

    FindNextNoneWhiteSpace();
    if ( mP >= mEnd ) {
        delete mScene;
        ThrowException( "Unexpected end of file while parsing string" );
    }

    if ( *mP != '"' ) {
        delete mScene;
        ThrowException( "Expected quotation mark." );
    }
    ++mP;

    while( mP < mEnd && *mP != '"')
        poString.append( mP++, 1);

    if ( mP >= mEnd - 1 ) {
        delete mScene;
        ThrowException( "Unexpected end of file while parsing string" );
    }

    if ( mP[ 1 ] != ';' || mP[ 0 ] != '"' ) {
        delete mScene;
        ThrowException( "Expected quotation mark and semicolon at the end of a string." );
    }
    mP+=2;
}

// ------------------------------------------------------------------------------------------------
void XFileParser::ReadUntilEndOfLine()
{
    if( mIsBinaryFormat)
        return;

    while( mP < mEnd)
    {
        if( *mP == '\n' || *mP == '\r')
        {
            ++mP; mLineNumber++;
            return;
        }

        ++mP;
    }
}

// ------------------------------------------------------------------------------------------------
unsigned short XFileParser::ReadBinWord()
{
//    ai_assert(mEnd - mP >= 2);
    const unsigned char* q = (const unsigned char*) mP;
    unsigned short tmp = q[0] | (q[1] << 8);
    mP += 2;
    return tmp;
}

// ------------------------------------------------------------------------------------------------
unsigned int XFileParser::ReadBinDWord() {
//    ai_assert(mEnd - mP >= 4);

    const unsigned char* q = (const unsigned char*) mP;
    unsigned int tmp = q[0] | (q[1] << 8) | (q[2] << 16) | (q[3] << 24);
    mP += 4;
    return tmp;
}

// ------------------------------------------------------------------------------------------------
unsigned int XFileParser::ReadInt()
{
   if( mIsBinaryFormat)
    {
        if( mBinaryNumCount == 0 && mEnd - mP >= 2)
        {
            unsigned short tmp = ReadBinWord(); // 0x06 or 0x03
            if( tmp == 0x06 && mEnd - mP >= 4) // array of ints follows
                mBinaryNumCount = ReadBinDWord();
            else // single int follows
                mBinaryNumCount = 1;
        }

        --mBinaryNumCount;
        const size_t len( mEnd - mP );
        if ( len >= 4)
        {
            return ReadBinDWord();
        }
        else
        {
            mP = mEnd;
            return 0;
        }
    }
    else
    {
        FindNextNoneWhiteSpace();

        // TODO: consider using strtol10 instead???

        // check preceding minus sign
        bool isNegative = false;
        if( *mP == '-')
        {
            isNegative = true;
            mP++;
        }

        // at least one digit expected
        if( !isdigit( *mP))
            ThrowException( "Number expected.");

        // read digits
        unsigned int number = 0;
        while( mP < mEnd)
        {
            if( !isdigit( *mP))
                break;
            number = number * 10 + (*mP - 48);
            mP++;
        }

        CheckForSeparator();

        return isNegative ? ((unsigned int) -int( number)) : number;
    }
}

// ------------------------------------------------------------------------------------------------
float XFileParser::ReadFloat()
{
    if( mIsBinaryFormat)
    {
        if( mBinaryNumCount == 0 && mEnd - mP >= 2)
        {
            unsigned short tmp = ReadBinWord(); // 0x07 or 0x42
            if( tmp == 0x07 && mEnd - mP >= 4) // array of floats following
                mBinaryNumCount = ReadBinDWord();
            else // single float following
                mBinaryNumCount = 1;
        }

        --mBinaryNumCount;
        if( mBinaryFloatSize == 8) {
            if( mEnd - mP >= 8) {
                double res;
                ::memcpy( &res, mP, 8 );
                mP += 8;
                const float result( static_cast<float>( res ) );
                return result;
            }
            else
            {
                mP = mEnd;
                return 0;
            }
        }
        else
        {
            if( mEnd - mP >= 4) {
                float result;
                ::memcpy( &result, mP, 4 );
                mP += 4;
                return result;
            }
            else
            {
                mP = mEnd;
                return 0;
            }
        }
    }

    // text version
    FindNextNoneWhiteSpace();
    // check for various special strings to allow reading files from faulty exporters
    // I mean you, Blender!
    // Reading is safe because of the terminating zero
    if( strncmp( mP, "-1.#IND00", 9) == 0 || strncmp( mP, "1.#IND00", 8) == 0)
    {
        mP += 9;
        CheckForSeparator();
        return 0.0;
    }
    else
    if( strncmp( mP, "1.#QNAN0", 8) == 0)
    {
        mP += 8;
        CheckForSeparator();
        return 0.0;
    }

    float result = 0.0;
    mP = XFile::fast_atoreal_move<float>( mP, result);

    CheckForSeparator();

    return result;
}

// ------------------------------------------------------------------------------------------------
XFile::Vector2D XFileParser::ReadVector2()
{
    XFile::Vector2D vector;
    vector.x = ReadFloat();
    vector.y = ReadFloat();
    TestForSeparator();

    return vector;
}

// ------------------------------------------------------------------------------------------------
XFile::Vector3D XFileParser::ReadVector3()
{
    XFile::Vector3D vector;
    vector.x = ReadFloat();
    vector.y = ReadFloat();
    vector.z = ReadFloat();
    TestForSeparator();

    return vector;
}

// ------------------------------------------------------------------------------------------------
XFile::Color4D XFileParser::ReadRGBA()
{
    XFile::Color4D color;
    color.r = ReadFloat();
    color.g = ReadFloat();
    color.b = ReadFloat();
    color.a = ReadFloat();
    TestForSeparator();

    return color;
}

// ------------------------------------------------------------------------------------------------
XFile::Color3D XFileParser::ReadRGB()
{
    XFile::Color3D color;
    color.r = ReadFloat();
    color.g = ReadFloat();
    color.b = ReadFloat();
    TestForSeparator();

    return color;
}

// ------------------------------------------------------------------------------------------------
// Throws an exception with a line number and the given text.
void XFileParser::ThrowException( const std::string& pText) {
    if ( mIsBinaryFormat )
    {
        throw XFile::DeadlyImportError( pText );
    }
    else
    {
        throw XFile::DeadlyImportError( format() << "Line " << mLineNumber << ": " << pText );
    }
}

// ------------------------------------------------------------------------------------------------
// Filters the imported hierarchy for some degenerated cases that some exporters produce.
void XFileParser::FilterHierarchy( XFile::Node* pNode)
{
    // if the node has just a single unnamed child containing a mesh, remove
    // the anonymous node between. The 3DSMax kwXport plugin seems to produce this
    // mess in some cases
    if( pNode->mChildren.size() == 1 && pNode->mMeshes.empty() )
    {
        XFile::Node* child = pNode->mChildren.front();
        if( child->mName.length() == 0 && child->mMeshes.size() > 0)
        {
            // transfer its meshes to us
            for( unsigned int a = 0; a < child->mMeshes.size(); a++)
                pNode->mMeshes.push_back( child->mMeshes[a]);
            child->mMeshes.clear();

            // transfer the transform as well
//            pNode->mTrafoMatrix = pNode->mTrafoMatrix * child->mTrafoMatrix;

            // then kill it
            delete child;
            pNode->mChildren.clear();
        }
    }

    // recurse
    for( unsigned int a = 0; a < pNode->mChildren.size(); a++)
        FilterHierarchy( pNode->mChildren[a]);
}
