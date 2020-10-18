/*
 * O P T L A Y O U T . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include <stdlib.h>
#include "stdlib/string.hpp"
#include "machgui/optlayout.hpp"
#include "system/pathname.hpp"

MachGuiOptionsLayout::MachGuiOptionsLayout( const SysPathName& filename) : parser_( filename )
{
	ASSERT( filename.existsAsFile(), filename.c_str() )
	parse();
    TEST_INVARIANT;
}

MachGuiOptionsLayout::~MachGuiOptionsLayout()
{
    TEST_INVARIANT;

	// Empty slidebar info list
	for ( Slidebars::iterator i = slidebars_.begin(); i != slidebars_.end(); i++ )
		_DELETE( *i );
	// Empty menutext info list
	for ( MenuTexts::iterator i = menuTexts_.begin(); i != menuTexts_.end(); i++ )
		_DELETE( *i );
	// Empty checkBox info list
	for ( CheckBoxes::iterator i = checkBoxes_.begin(); i != checkBoxes_.end(); i++ )
		_DELETE( *i );
}

void MachGuiOptionsLayout::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

const MachGuiOptionsLayout::SlidebarInfo& MachGuiOptionsLayout::slidebarInfo( uint index ) const
{
	PRE( nSlidebars() );
	PRE( index < nSlidebars() );
	return *( slidebars_[ index ] );
}

const MachGuiOptionsLayout::MenuTextInfo& MachGuiOptionsLayout::menuTextInfo( uint index ) const
{
	PRE( nMenuTexts() );
	PRE( index < nMenuTexts() );
	return *( menuTexts_[ index ] );
}

const MachGuiOptionsLayout::CheckBoxInfo& MachGuiOptionsLayout::checkBoxInfo( uint index ) const
{
	PRE( nCheckBoxes() );
	PRE( index < nCheckBoxes() );
	return *( checkBoxes_[ index ] );
}

void MachGuiOptionsLayout::parse( void )
{
	while (	not parser_.finished() )
	{
		if ( parser_.tokens()[ 0 ] == "SB" )
			parseSlidebar( parser_.tokens() );
		else if ( parser_.tokens()[ 0 ] == "MT" )
			parseMenuText( parser_.tokens() );
		else if( parser_.tokens()[ 0 ] == "CB" )
			parseCheckBox( parser_.tokens() );
		parser_.parseNextLine();
	}
}

void MachGuiOptionsLayout::parseSlidebar( const UtlLineTokeniser::Tokens& tokens )
{
	ASSERT( tokens.size() == 4, "wrong number of arguments" );

	MexPoint2d topLeft( atoi( tokens[ 1 ].c_str() ), atoi( tokens[ 2 ].c_str() ) );
	uint range = atoi( tokens[ 3 ].c_str() );
	slidebars_.push_back( _NEW( SlidebarInfo( topLeft, range ) ) );
}

void MachGuiOptionsLayout::parseMenuText( const UtlLineTokeniser::Tokens& tokens )
{
	ASSERT( tokens.size() == 7, "wrong number of arguments" );

	int idsString( atoi( tokens[ 1 ].c_str() ) );
	MexPoint2d topLeft( atoi( tokens[ 2 ].c_str() ), atoi( tokens[ 3 ].c_str() ) );
	MexPoint2d bottomRight( atoi( tokens[ 4 ].c_str() ), atoi( tokens[ 5 ].c_str() ) );
	string fontPath( tokens[ 6 ] );
	ASSERT( SysPathName( fontPath ).existsAsFile(), " " );
	menuTexts_.push_back( _NEW( MenuTextInfo( idsString, topLeft, bottomRight, fontPath ) ) );
}

void MachGuiOptionsLayout::parseCheckBox( const UtlLineTokeniser::Tokens& tokens )
{
	ASSERT( tokens.size() == 4, "wrong number of arguments" );

	int stringId( atoi( tokens[ 1 ].c_str() ) );
	MexPoint2d topLeft( atoi( tokens[ 2 ].c_str() ), atoi( tokens[ 3 ].c_str() ) );
	checkBoxes_.push_back( _NEW( CheckBoxInfo( topLeft, stringId ) ) );
}

ostream& operator <<( ostream& o, const MachGuiOptionsLayout& t )
{

    o << "MachGuiOptionsLayout " << (void*)&t << " start" << std::endl;
	for ( uint	i = 0; i < t.slidebars_.size(); i++ )
	{
		const MachGuiOptionsLayout::SlidebarInfo* info = t.slidebars_[ i ];
		o << "topleft :" << info->topLeft << std::endl;
		o << "range: " << info->range << std::endl;
	}
	for ( uint	i = 0; i < t.menuTexts_.size(); i++ )
	{
		const MachGuiOptionsLayout::MenuTextInfo* info = t.menuTexts_[ i ];
		o << "topleft :" << info->topLeft << std::endl;
		o << "bottom right: " << info->bottomRight << std::endl;
		o << "font: " << info->font << std::endl;
	}

    o << "MachGuiOptionsLayout " << (void*)&t << " end" << std::endl;

    return o;
}

/* End OPTLAYOUT.CPP ************************************************/
