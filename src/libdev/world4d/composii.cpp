/*
 * C O M P I M P L . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "stdlib/string.hpp"
#include "world4d/internal/composii.hpp"

#include "ctl/pvector.hpp"
#include "utility/linetok.hpp"

#include "mathex/coordsys.hpp"
#include "mathex/transf3d.hpp"

#include "phys/rampacce.hpp"
#include "phys/linemoti.hpp"

#include "render/hierload.hpp"

#include "world4d/compplan.hpp"
#include "world4d/entyplan.hpp"
#include "world4d/shadow.hpp"


W4dCompositeImpl::W4dCompositeImpl( const W4dCompositeImpl& copyMe, W4dComposite* pComposite )
:	pComposite_( pComposite ),
    pHeldEntities_( NULL ),
	maxLinkId_( 0 ),
	maxRange_(copyMe.maxRange_),
    compositeBoundingVolume_( copyMe.compositeBoundingVolume_ ),
    compositePlans_( copyMe.compositePlans_ ),
	pStoredLinkPositions_( NULL )
{

    TEST_INVARIANT;
}

W4dCompositeImpl::W4dCompositeImpl( W4dComposite* pComposite )
:	pComposite_( pComposite ),
    pHeldEntities_( NULL ),
    maxLinkId_( 0 ),
	maxRange_(HUGE_VAL),
    compositeBoundingVolume_(0,0,0,0,0,0),
    pStoredLinkPositions_( NULL )
{

    TEST_INVARIANT;
}

W4dCompositeImpl::~W4dCompositeImpl()
{
    //Delete all the links
    while( links_.size() != 0 )
    {
        W4dLink* pLink = links_.back();

        links_.pop_back();
        _DELETE( pLink );
    }

    //Delete other items
    _DELETE( pHeldEntities_ );
}


void W4dCompositeImpl::CLASS_INVARIANT
{
    INVARIANT( pComposite_ != NULL );
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const W4dCompositeImpl& t )
{

    o << "W4dCompositeImpl " << (void*)&t << " start" << std::endl;
    o << "W4dCompositeImpl " << (void*)&t << " end" << std::endl;

    return o;
}

void W4dCompositeImpl::parseModel( const SysPathName& directoryName, UtlLineTokeniser* pParser, W4dLOD id )
{
    PRE( pParser->tokens()[ 0 ] == "MODEL" );
    PRE( pParser->tokens().size() == 2 );

    SysPathName pathname( directoryName );
    bool    empty_ = false;

    if( pParser->tokens()[ 1 ] == "EMPTY" )
        empty_ = true;
    else
        pathname.combine( pParser->tokens()[ 1 ] );

    pParser->parseNextLine();

    MATHEX_SCALAR  distance;

    if( not pParser->finished() and pParser->tokens()[ 0 ] == "DISTANCE" )
    {
        ASSERT( pParser->tokens().size() == 2, "" );
        distance = atof( pParser->tokens()[ 1 ].c_str() );
        pParser->parseNextLine();
    }
    else
        distance = HUGE_VAL;

    //  By adding empty meshes at this id we can make sure that every link
    //  will have an entry at this id even if the .x file does not contain
    //  a mesh for it. This will make sure that the switching works properly
    //  if a mesh is switched out then switched back in again at a greater
    //  range.

    addEmptyMeshes( distance, id );

    if( not empty_ )
    {
        RenHierarchyBuilder builder;
        RenHierarchyLoader::load( pathname, &builder );

        buildHierarchy( NULL, builder.pRoot(), distance, id );

    }
}

void W4dCompositeImpl::parseMeshRanges( const SysPathName& /* directoryName */ , UtlLineTokeniser* pParser )
{
    PRE( pParser->tokens()[ 0 ] == "MESH_RANGES" );
    PRE( pParser->tokens().size() >= 2 );

    string  meshName = pParser->tokens()[ 1 ];

    W4dLink*    pLink;

    bool linkFound = findLink( meshName, &pLink );

    ASSERT_INFO( meshName );
    ASSERT( linkFound, "Mesh not found in hierarchy" );

    size_t  lod = 0;

    for( size_t i = 0; i < pParser->tokens().size() - 2; ++i )
    {
        MATHEX_SCALAR   distance = atof( pParser->tokens()[ i + 2 ].c_str() );
        pLink->setDistance( distance, i );
        lod = i + 1;
    }

    //  Don't draw any meshes that exist after the last range specified
    for( ; pLink->hasMesh( lod ); ++lod )
    {
        pLink->add( NULL, lod );
    }

    pParser->parseNextLine();
}

// SHADOW_FIXED		<LOD path>	<z-offset>
// SHADOW_PROJ      <LOD path>	<z-offset>
void W4dCompositeImpl::parseShadow( const SysPathName& directoryName, UtlLineTokeniser* pParser )
{
    PRE(pParser->tokens().size() == 3);

    SysPathName pathname( directoryName );
    pathname.combine( pParser->tokens()[ 1 ] );

    const MATHEX_SCALAR z = atof( pParser->tokens()[2].c_str() );

    if (pParser->tokens()[0] == "SHADOW_PROJ")
    {
		W4dShadowProjected2d* shd = _NEW(W4dShadowProjected2d(pComposite_, MexPoint3d(0,0,z)));
		shd->loadLODFile(pathname);
	    shd->name(pParser->tokens()[0]);
    }
    else if (pParser->tokens()[0] == "SHADOW_FIXED")
    {
		W4dShadowFixed* shd = _NEW(W4dShadowFixed(pComposite_, MexPoint3d(0,0,z)));
		shd->loadLODFile(pathname);
	    shd->name(pParser->tokens()[0]);
	}

    pParser->parseNextLine();
}

//  ANIMATION   <animation name> <filename> <animation set name> <frames per second>
void W4dCompositeImpl::parseAnimation( const SysPathName& directoryName, UtlLineTokeniser* pParser )
{
    PRE( pParser->tokens()[ 0 ] == "ANIMATION" );
    PRE( pParser->tokens().size() == 5 );

    string      planName = pParser->tokens()[ 1 ];

    SysPathName fileName( directoryName );
    fileName.combine( pParser->tokens()[ 2 ] );
    fileName.extension( "x" );

    string      animationName = pParser->tokens()[ 3 ];

    MATHEX_SCALAR   framesPerSecond = atof( pParser->tokens()[ 4 ].c_str() );

    W4dCompositePlan* pCompositePlan = _NEW( W4dCompositePlan( planName ) );

    bool animationRead = readAnimation( fileName, animationName, pCompositePlan, framesPerSecond );

    if( compositePlans_.size() == 0 )
        compositePlans_.reserve( 4 );

    compositePlans_.push_back( pCompositePlan );

    pParser->parseNextLine();
}

//  Read an animation directly from a .x file rather than having to convert
//  .x to a .xan file.
bool W4dCompositeImpl::readAnimation( const SysPathName& fileName,
                                     const string& animationName,
                                     W4dCompositePlan* pCompositePlan,
                                     MATHEX_SCALAR framesPerSecond ) const
{
    //Check arguments
    PRE( pCompositePlan != NULL );
    PRE_INFO( fileName );
    PRE( fileName.existsAsFile() );

    //Construct a parser to use with the input stream
    UtlLineTokeniser   parser( fileName );
    parser.addFixedToken( "{" );
    parser.addFixedToken( "}" );
    parser.addTokenDelimiter( ',' );
    parser.addTokenDelimiter( ';' );

    bool    finished = false;

    while( not parser.finished() and not finished )
    {
        if( ( parser.tokens().size() >= 2 ) and
            parser.tokens()[ 0 ] == "AnimationSet"  and
            parser.tokens()[1] == animationName)
        {
            parser.parseNextLine();

            //Next line either defines a link animation, or end-of-set
            while( not parser.finished() )
            {
                if( parser.tokens()[0] == "Animation" )
                {
                    //We have another link animation. This line should contain 3 tokens
                    ASSERT( parser.tokens().size() == 3, "" );
                    ASSERT( parser.tokens()[2] == "{", "" )
                    parser.parseNextLine();
                    ASSERT( not parser.finished(), "" )

                    //The current line contains the link name. Map to link id.
                    POST( parser.tokens().size() == 3 );

                    string linkName = parser.tokens()[1];

                    //  3D studio puts "x3ds_" in front of names sometimes - remove
                    //  this if necessary.
                    if( linkName.substr( 0, 5 ) == "x3ds_" )
                        linkName = linkName.substr( 5, linkName.length() - 5 );

					//TBD: This is a hack for the deliverable.
					//Find the length of the substring before an 'X' character.
					//This separates out the link name from the mesh shared name.
					size_t length = 0;
					while( length < linkName.length()  and  linkName[length] != 'X' )
						++length;

                    linkName = linkName.substr( 0, length );

                    W4dLink* pLink;
                    bool foundLink = findLink( linkName, &pLink );
                    ASSERT_INFO( linkName );
                    ASSERT( foundLink && pLink != NULL, "Unknown link name in XAN file");
                    W4dLink::Id id = pLink->id();

                    parser.parseNextLine();
                    ASSERT( not parser.finished(), "" )

                    //Get the link animation as a PhysMotionPlan

                    PhysMotionPlanPtr planPtr;
                    if( parseLinkAnimation( &parser, framesPerSecond, linkName, &planPtr ) )
                    {
                        W4dEntityPlan entityPlan;
                        entityPlan.absoluteMotion( planPtr, PhysAbsoluteTime( 0 ), 0 );

                        //Add this plan to the composite plan
                        pCompositePlan->linkPlan( id, entityPlan );
                    }

                }
                else
                {
                    //End of animation set
                    if( parser.tokens()[0] != "}" )
                    {
                        ASSERT_INFO( parser.tokens()[ 0 ] );
                        ASSERT_INFO( parser.line() );
                        ASSERT_FAIL( "Unexpected data in XAN file" );
                    }

                    finished = true;
                }

                if( not parser.finished() )
                    parser.parseNextLine();
            }
        }

        //Load the next line
        if( not parser.finished() )
            parser.parseNextLine();
    }

    return true;
}

bool W4dCompositeImpl::parseLinkAnimation(
    UtlLineTokeniser* pParser,
    MATHEX_SCALAR framesPerSecond,
    const string& linkName,
    PhysMotionPlanPtr* pPlanPtr ) const
{
    PRE( pParser != NULL )

    //The next line must be either the terminating brace, or an AnimationKey
    enum KeyType {ORIENTATION, SCALE, LOCATION, UNKNOWN};
    W4dCompositeImpl::KeyFrameOrientations orientations;
    W4dCompositeImpl::KeyFrameLocations locations;

    orientations.reserve( 30 );
    locations.reserve( 30 );

    while( pParser->tokens()[0] == "AnimationKey" )
    {
        POST( pParser->tokens().size() == 2  and  pParser->tokens()[1] == "{" );
        pParser->parseNextLine();
        POST( not pParser->finished() )

        //The next line is the key type
        PRE( pParser->tokens().size() == 1 )
        int type = atoi( pParser->tokens()[0].c_str() );
        KeyType keyType = UNKNOWN;
        size_t nExpectedValues;
        switch( type )
        {
            case 0:
            {
                keyType = ORIENTATION;
                nExpectedValues = 4;
                ASSERT( orientations.size() == 0, "Repeated orientation data" );
                break;
            }

            case 1:
            {
                keyType = SCALE;
                nExpectedValues = 3;
                break;
            }

            case 2:
            {
                keyType = LOCATION;
                nExpectedValues = 3;
                ASSERT( locations.size() == 0, "Repeated location data" );
                break;
            }

            default: ASSERT_FAIL( "Unknown XAN key type" );
        }
        pParser->parseNextLine();
        POST( not pParser->finished() )

        //Now get the number of frames defined
        PRE( pParser->tokens().size() == 1 )
        int nFrames = atoi( pParser->tokens()[0].c_str() );
        POST( nFrames > 0 );
        pParser->parseNextLine();
        POST( not pParser->finished() )

        //Parse the actual key frame data
        while ( nFrames-- )
        {
            //Check correct value count
//             ASSERT_INFO( pParser->tokens().size() );
//             ASSERT_INFO( nExpectedValues );
//             ASSERT( pParser->tokens().size() == nExpectedValues + 4, "" );

            //Get the key frame id
            W4dCompositeImpl::KeyFrameId frameId = atoi( pParser->tokens()[0].c_str() );

            //Check the value count matches the expected count
            PRE( atoi( pParser->tokens()[1].c_str() ) == nExpectedValues );

            //Get the type dependent data
            switch( keyType )
            {
                case ORIENTATION:
                {
                    MATHEX_SCALAR s = atof( pParser->tokens()[2].c_str() );
                    MATHEX_SCALAR vx = atof( pParser->tokens()[3].c_str() );
                    MATHEX_SCALAR vy = atof( pParser->tokens()[4].c_str() );
                    MATHEX_SCALAR vz = atof( pParser->tokens()[5].c_str() );

					// Swap the y and z components of rotation.  This also seems to require
					// the sense of the rotation to be changed.  Why?
					if (MexCoordSystem::instance() == MexCoordSystem::RIGHT_HANDED)
					{
						std::swap(vy, vz);
						s = -s;
					}

                    MexQuaternion params;
                    params.set( -vx, -vy, -vz, s );
                    orientations.push_back( W4dCompositeImpl::KeyFrameOrientation( frameId, params ) );
                    break;
                }

                case LOCATION:
                {
                    MATHEX_SCALAR x = atof( pParser->tokens()[2].c_str() );
                    MATHEX_SCALAR y = atof( pParser->tokens()[3].c_str() );
                    MATHEX_SCALAR z = atof( pParser->tokens()[4].c_str() );

					// Note y and z may be swapped during reading.  The conv3ds program appears
					// to swap the values in an attempt to switch from 3DS's right-handed
					// system to DirectX's left-handed.  If we require a right-handed system,
					// we need to undo this.
					if (MexCoordSystem::instance() == MexCoordSystem::RIGHT_HANDED)
						std::swap(y, z);

                    locations.push_back( W4dCompositeImpl::KeyFrameLocation( frameId, MexVec3( x, y, z ) ) );
                    break;
                }
            }

            //Must have another line
            pParser->parseNextLine();
            POST( not pParser->finished() )
        }

        //Next line must be closing brace
        POST( pParser->tokens().size() == 1 and pParser->tokens()[0] == "}" );
        pParser->parseNextLine();
        POST( not pParser->finished() )
    }

    //Must be the end of the animation
    ASSERT( pParser->tokens().size() == 1 and pParser->tokens()[0] == "}", "" );

    bool    result = false;

    if( animationValid( linkName, orientations, locations ) )
    {
        result = true;
        *pPlanPtr = makeAnimationPlan( orientations, locations, framesPerSecond );
    }

    return result;
}


void W4dCompositeImpl::buildHierarchy(
    W4dEntity* pParent,
    const RenHierarchyBuilder::Node* pNode,
    double distance,
    W4dLOD id )
{
	TEST_INVARIANT;
	PRE(pComposite_);

    if( pNode )
    {
        W4dEntity* pEntity = NULL;

        if( id == 0 )
        {
            //  This hierarchy is being built for the first time

            if( pParent )
            {
                W4dLink* pLink = pComposite_->addLink( pNode->transform(), W4dEntity::SOLID );
                pLink->attachTo( pParent, pNode->transform() );
                pLink->name( pNode->instanceName() );
                pEntity= pLink;

            }
            else
            {
                pEntity = pComposite_;
                pComposite_->localTransform( pNode->transform() );
            }
        }
        else if( pNode->instanceName() != "" )
        {
            //  The hierarchy has already been built, we are now adding
            //  additional levels of detail to it. Since lower LOD models
            //  do not need to have the same hierarchy as the higher LOD
            //  models we have to do a search every time to find the
            //  corresponding links.

            W4dLink*    pLink;

            bool linkFound = findLink( pNode->instanceName(), &pLink );

            pEntity = pLink;

            ASSERT_INFO( pNode->instanceName() );
            ASSERT( linkFound, "could not find link" );
        }

        if( pEntity != NULL )
        {
            RenMeshInstance* pInstance = pNode->pMeshInstance();

            if( pInstance )
                pEntity->add( pInstance, distance, id );
        }

        for( RenHierarchyBuilder::Node::const_iterator i = pNode->begin(); i != pNode->end(); ++i )
            buildHierarchy( pEntity, *i, distance, id );
    }
}

void W4dCompositeImpl::addEmptyMeshes( double distance, W4dLOD id )
{
    for( W4dComposite::W4dLinks::const_iterator i = links_.begin(); i != links_.end(); ++i )
    {
        W4dEntity* pEntity = *i;

        pEntity->add( NULL, distance, id );
    }
}

//  Return true iff this animation actually means something - i.e. it will
//  lead to a change in position for the link involved. 3D Studio tends to
//  kick out a lot of unnecessary animations so we need to check for them.

bool W4dCompositeImpl::animationValid(
    const string& linkName,
    const KeyFrameOrientations& orientations,
    const KeyFrameLocations& locations ) const
{
    bool    valid = false;

    W4dLink*    pLink;

    bool    linkFound = findLink( linkName, &pLink );

    ASSERT_INFO( linkName );
    ASSERT( linkFound, "Link not found" );

    MexTransform3d  linkTransform = pLink->localTransform();

    if( orientations.size() > 0 )
    {
        //  Don't check against the link's original orientation because of
        //  rounding errors. This means that an animation which just moves
        //  the link to a new orientation and stays there will be rejected
        //  - we are happy with this.

//         MexQuaternion   linkOrientation = linkTransform.rotationAsQuaternion();
//
//         if( linkOrientation != orientations.front().second )
//             valid = true;

        for( KeyFrameOrientations::const_iterator i = orientations.begin();
          i != orientations.end() and not valid; ++i )
        {
            if( (*i).second != orientations.front().second )
                valid = true;
        }
    }

    if( locations.size() > 0 and not valid )
    {
        //  Don't check against the link's original position because of
        //  rounding errors. This means that an animation which just moves
        //  the link to a new position and stays there will be rejected
        //  - we are happy with this.

//         MexVec3   linkLocation = linkTransform.position();
//
//         if( linkLocation != locations.front().second )
//             valid = true;

        for( KeyFrameLocations::const_iterator i = locations.begin();
          i != locations.end() and not valid; ++i )
        {
            if( (*i).second != locations.front().second )
                valid = true;
        }
    }

    return valid;
}

bool W4dCompositeImpl::findLink( const string& name, W4dLink** ppLink ) const
{
    *ppLink = NULL;

    for( W4dComposite::W4dLinks::const_iterator i = links_.begin(); i != links_.end() and *ppLink == NULL; ++i )
    {
        if( (*i)->name() == name )
            *ppLink = *i;
    }

    return *ppLink != NULL;
}

PhysMotionPlanPtr W4dCompositeImpl::makeAnimationPlan( const KeyFrameOrientations& orientations,
                                             const KeyFrameLocations& locations,
                                             MATHEX_SCALAR framesPerSecond ) const
{
    PRE( framesPerSecond > 0 )

    size_t i = 0;
    size_t j = 0;

	PRE(orientations[i].first == 0 or locations[i].first == 0);

    //Get the start transform from first orientation and location,
    MexTransform3d startTransform( orientations.front().second, locations.front().second );

    //Now construct the next transform
	++i;
	++j;

    MexTransform3d currentTransform( startTransform );
    PhysRelativeTime time;

    if( locations.size() == 1 )
    {
        time = orientations[i].first / framesPerSecond;
    	currentTransform.rotation( orientations[i].second );
    }
    else if( orientations.size() == 1 )
    {
        time = locations[j].first / framesPerSecond;
        currentTransform.position( locations[j].second );
    }

    else
    {
		if(locations[j].first < orientations[i].first)
		{
			time = locations[j].first / framesPerSecond;
			MexQuaternion rotation = interpolatedOrientation(orientations, locations[j].first);
			currentTransform = MexTransform3d( rotation, locations[j].second );
			++j;
		}
		else if(locations[j].first > orientations[i].first)
		{
			time = orientations[i].first / framesPerSecond;
			MexPoint3d position = interpolatedPosition(locations, orientations[i].first);
			currentTransform = MexTransform3d( orientations[i].second,  position);
			++i;
		}
		else
		{
			time = orientations[i].first / framesPerSecond;
			currentTransform = MexTransform3d( orientations[i].second, locations[j].second );
			++i;
			++j;
		}

	}

    //Construct a linear motion plan
    PhysLinearMotionPlan* pPlan = _NEW( PhysLinearMotionPlan( startTransform, currentTransform, time ) );

    //Add any further transforms
    while(i < orientations.size() and j < locations.size() )
    {
		if(locations[j].first < orientations[i].first)
		{
			time = locations[j].first / framesPerSecond;
			MexQuaternion rotation = interpolatedOrientation(orientations, locations[j].first);
			currentTransform = MexTransform3d( rotation, locations[j].second );
			++j;
		}
		else if(locations[j].first > orientations[i].first)
		{
			time = orientations[i].first / framesPerSecond;
			MexPoint3d position = interpolatedPosition(locations, orientations[i].first);
			currentTransform = MexTransform3d( orientations[i].second,  position);
			++i;
		}
		else
		{
			time = orientations[i].first / framesPerSecond;
			currentTransform = MexTransform3d( orientations[i].second,  locations[j].second);
			++i;
			++j;
		}

        pPlan->add( currentTransform, time );
    }


	//add any trailing locations
   	PhysRelativeTime lastOrientationTime = time;
    while( j < locations.size() )
    {
        time = locations[j].first / framesPerSecond;
        if( time >= lastOrientationTime )
        {
            currentTransform.position( locations[j].second );
            pPlan->add( currentTransform, time );
        }
		++j;
    }

	//add anu trailing orientations
   	PhysRelativeTime lastLocationTime = time;
    while( i < orientations.size() )
    {
        time = orientations[i].first / framesPerSecond;
        if( time >= lastLocationTime )
        {
        	currentTransform.rotation( orientations[i].second );
        	pPlan->add( currentTransform, time );
		}
		++i;
    }

    return PhysMotionPlanPtr( pPlan );
}

//local
static void linearInterpolation(const double& y1, const double& y2, const double& x1, const double& x2, const double& x, double* y)
{
	double s = x2-x1;
	double b = (y2-y1)/s;
	double a = (y1*x2-y2*x1)/s;

	*y = a + b*x;
}

MexQuaternion W4dCompositeImpl::interpolatedOrientation(const KeyFrameOrientations& orientations,
                                                           const KeyFrameId& id)  const
{
	uint size = orientations.size();

	PRE_INFO(size);
	PRE(size>0);

	if(size == 1)
		return orientations.front().second;
	else
	{
		uint element = 0;
		if(id < orientations[0].first)
				element = 0;
		else if(id > orientations[size-1].first)
				element = size-2;
		else
		{
			bool found = false;
			for(uint i = 0; i<size && found == false; ++i)
			{
				if( id >= orientations[i].first && id <= orientations[i+1].first )
				{
					element = i;
					found = true;
				}
			}
		}
    	MexVec3 vector1 =  orientations[element].second.vector();
   		MexVec3 vector2 =  orientations[element+1].second.vector();

     	MATHEX_SCALAR scalar1 = orientations[element].second.scalar();
     	MATHEX_SCALAR scalar2 = orientations[element+1].second.scalar();

		MATHEX_SCALAR left = orientations[element].first;
		MATHEX_SCALAR right = orientations[element+1].first;

		MATHEX_SCALAR x, y, z, s;

		linearInterpolation(vector1.x(), vector2.x(), left, right, id, &x);
		linearInterpolation(vector1.y(), vector2.y(), left, right, id, &y);
		linearInterpolation(vector1.z(), vector2.z(), left, right, id, &z);
		linearInterpolation(scalar1, scalar2, left, right, id, &s);

    	s =2.0 * acos(s);

		MexQuaternion newRotation(MexVec3(x, y, z), MexRadians(s));

		return newRotation;
	}
}

MexPoint3d W4dCompositeImpl::interpolatedPosition(const KeyFrameLocations& locations, const KeyFrameId& id)	const
{
	uint size = locations.size();

	PRE_INFO(size);
	PRE(size>0);

	if(size == 1)
		return locations.front().second;
	else
	{
		uint element = 0;
		if(id < locations[0].first)
				element = 0;
		else if(id > locations[size-1].first)
				element = size-2;
		else
		{
			bool found = false;
			for(uint i = 0; i<size && found == false; ++i)
			{
				if( id >= locations[i].first && id <= locations[i+1].first )
				{
					element = i;
					found = true;
				}
			}
		}
    	MexVec3 vector1 =  locations[element].second;
   		MexVec3 vector2 =  locations[element+1].second;

		MATHEX_SCALAR left = locations[element].first;
		MATHEX_SCALAR right = locations[element+1].first;

		MATHEX_SCALAR x, y, z;

		linearInterpolation(vector1.x(), vector2.x(), left, right, id, &x);
		linearInterpolation(vector1.y(), vector2.y(), left, right, id, &y);
		linearInterpolation(vector1.z(), vector2.z(), left, right, id, &z);

		return MexPoint3d(x, y, z);
	}
}


PER_DEFINE_PERSISTENT( W4dCompositeImpl );

W4dCompositeImpl::W4dCompositeImpl( PerConstructor )
:   pComposite_( NULL ),
    pStoredLinkPositions_( NULL ),
	pHeldEntities_( NULL )
{
}

void perWrite( PerOstream& ostr, const W4dCompositeImpl& t )
{
    ostr << t.pComposite_;
    ostr << t.links_;
    ostr << t.maxLinkId_;

    //This is necessary because writing a ctl_pvector* causes a crash
    bool hasHeldEntities = t.pHeldEntities_ != NULL;
    PER_WRITE_RAW_OBJECT( ostr, hasHeldEntities );
    if( hasHeldEntities )
        ostr << *t.pHeldEntities_;

    ostr << t.maxRange_;
    ostr << t.compositeBoundingVolume_;
    ostr << t.compositePlans_;
}

void perRead( PerIstream& istr, W4dCompositeImpl& t )
{
    istr >> t.pComposite_;
    istr >> t.links_;
    istr >> t.maxLinkId_;

	bool hasHeldEntities;
	PER_READ_RAW_OBJECT( istr, hasHeldEntities );
	if( hasHeldEntities )
	{
		if ( t.pHeldEntities_ == NULL )
		{
			t.pHeldEntities_ = _NEW( W4dComposite::HeldEntities() );
		}
	    istr >> *t.pHeldEntities_;
	}
	else
		t.pHeldEntities_ = NULL;

    istr >> t.maxRange_;

    istr >> t.compositeBoundingVolume_;
    istr >> t.compositePlans_;
}


/* End COMPIMPL.CPP *************************************************/
