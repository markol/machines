/*
 * D I A G S T R . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include <stdlib.h>
#include <stdlib.h>

#include "base/internal/diagstr.hpp"
#include "base/internal/windstr.hpp"
#include "base/diag.hpp"
#include <cstring>

DiagStream::DiagStream()
: pDependantStream_( NULL ),
  pNextStream_( NULL ),
  exists_( false ),
  append_( false ),
  name_( NULL ),
  cout_( false ),
  pWindowStream_( NULL )///
{
}

DiagStream::~DiagStream()
{
    close();

    if( name_ )
    {
        free( name_ );
    }

    exists_ = false;
}

void DiagStream::setup(
    const char* environmentVariable,
    const char* defaultSetting )
{
    interpretEnvironmentVariable( environmentVariable, defaultSetting );

    setupStream();

    exists_ = true;
}

void DiagStream::setupStream()
{
    //  Look for DiagStreams already using this filename so that
    //  we don't end up fighting over the file.

    DiagStream* pStream = DiagStream::pFirstStream();

    bool    firstFileOfThisName = true;

    while( pStream != NULL )
    {
        if( /* pStream->exists() and */
          ( name() != NULL ) and
          ( pStream->name() != NULL ) and
          strcmp( pStream->name(), name() ) == 0 )
        {
            //  We have our match. Now check for whether this is an append
            //  file or not. If a file is an append file for any debug stream
            //  it must be an append file for them all.

            if( append() )
                pStream->forceAppendFile();

            if( pStream->append() )
                append_ = true;

            //  If this is not an append stream then make it dependant.
            if( not append() )
                pDependantStream_ = pStream;

            firstFileOfThisName = false;
        }

        pStream = pStream->pNextStream();
    }

    //  Add this stream to the internal list

    pNextStream( DiagStream::pFirstStream() );

    DiagStream::pFirstStream() = this;

    //  Set up the master ostream which does any prepending that might be necessary

    if( pDependantStream_ )
    {
        masterOstr_.stream( &pDependantStream_->nonPrependOstr() );
    }
    else
    {
        if( name() != NULL )
        {
            if( append() )
            {
                appendOstr_.name( name_ );

                if( firstFileOfThisName )
                    appendOstr_.clear();

                masterOstr_.stream( &appendOstr_ );
            }
            else
            {
                std::ios::openmode openMode;

                if( firstFileOfThisName )
                {
                    openMode = std::ios::out;
                }
                else
                {
                    openMode = std::ios::app;
                }

                ostr_.open( name(), openMode );

                masterOstr_.stream( &ostr_ );
            }
        }

        if( cout_ )
            masterOstr_.stream( &std::cout );

        if( pWindowStream_ )
            masterOstr_.stream( pWindowStream_ );
    }
}

void    DiagStream::close()
{
    const char* separator = "======================";
    ostr() << std::endl << separator;
    ostr() << " Stream closed under control ";
    ostr() << separator << std::endl;

    if( not pDependantStream_ and name() != NULL and not append() )
        ostr_.close();

    pDependantStream_ = NULL;
    pNextStream_ = NULL;
    exists_ = false;
    append_ = false;
    if( name_ )
        free(name_);
    name_ = NULL;
    cout_ = false;
    pWindowStream_ = NULL;
}

// bool    DiagStream::exists()
// {
//     PRE( this != NULL );
//
//     if( pDependantStream_ )
//         return pDependantStream_->exists() and exists_;
//
//     return exists_;
// }

ostream&    DiagStream::ostr()
{
//     if( pDependantStream_ )
//     {
//         ASSERT( pDependantStream_->exists(), "" );
//     }

    return masterOstr_;

//     if( append() )
//         return appendOstr_;
//
//     if( cout_ )
//         return cout;
//
//     return ostr_;
}

ostream&    DiagStream::nonPrependOstr()
{
    if( pDependantStream_ )
    {
//        ASSERT( pDependantStream_->exists(), "" );

        return pDependantStream_->nonPrependOstr();
    }

    if( append() )
        return appendOstr_;

    if( cout_ )
        return std::cout;

    return ostr_;
}

void DiagStream::interpretEnvironmentVariable(
    const char* environmentVariable,
    const char* defaultSetting )
{
    //const char* envText = Diag::instance().getenv( environmentVariable );
    // TODO: some recursive error above
    const char* envText = getenv( environmentVariable );

    if( envText == NULL )
        envText = defaultSetting;

    if( envText != NULL )
    {
        char* copyText = (char*)malloc( strlen( envText ) + 1 );
        strcpy( copyText, envText );

        char* token = strtok( copyText, ";" );

        while( token != NULL )
        {
            if( strcmp( token, "append" ) == 0 )
            {
                append_ = true;
            }
            else if( strcmp( token, "cout" ) == 0 )
            {
                cout_ = true;
            }
            else if( strcmp( token, "clock" ) == 0 )
            {
                masterOstr_.prependClock( true );
            }
            else if( strcmp( token, "date" ) == 0 )
            {
                masterOstr_.prependDate( true );
            }
            else if( strcmp( token, "none" ) == 0 )
            {
                //  Don't have to do anything, none simply allows
                //  the user to stop the default setting being used.
            }
            //TODO what window stream does?
            /*else if( strcmp( token, "window" ) == 0 or
                     strncmp( token, "window:", 7 ) == 0 )
            {
                char*  windowName = "";

                if( strlen( token ) > 7 )
                    windowName = token + 7;

                pWindowStream_ = _NEW( BaseWindowStream( windowName ) );
            }*/
            else
            {
                name_ = (char*)malloc( strlen( token ) + 1 );
                strcpy( name_, token );
            }

            token = strtok( NULL, ";" );
        }
        free(copyText);
    }
}

const char* DiagStream::name() const
{
    return name_;
}

bool DiagStream::append() const
{
    return append_;
}

void DiagStream::forceAppendFile()
{
    if( not append_ )
    {
        ostr_.close();

        append_ = true;

        appendOstr_.name( name_ );
    }
}

DiagStream* DiagStream::pNextStream()
{
    return pNextStream_;
}

void DiagStream::pNextStream( DiagStream* pNext )
{
    pNextStream_ = pNext;
}

// static
DiagStream*& DiagStream::pFirstStream()
{
    static  DiagStream* pFirstStream_ = NULL;

    return pFirstStream_;
}

bool    DiagStream::hasDestination() const
{
    return masterOstr_.hasStream();
}

void    DiagStream::indent( int nSpaces )
{
    masterOstr_.nSpacesToIndent( nSpaces );
}


/* End DIAGSTR.CPP **************************************************/
