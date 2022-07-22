/*
 * U N S I G N E D . I P P
 * (c) Charybdis Limited, 1997. All Rights Reserved.
 */

inline
MexUnsigned::MexUnsigned( unsigned d )
: value_( d ),
  cardinality_( FINITE )
{
	// Intentionally Empty
}

inline
MexUnsigned::MexUnsigned( Cardinality c )
: value_( 0 ),
  cardinality_( c )
{
	// Intentionally Empty
}

inline
MexUnsigned::operator unsigned() const
{
	PRE( not isInfinite() );
	return value_;
}

inline
bool MexUnsigned::isInfinite() const
{
	return cardinality_ != FINITE;
}

inline
const MexUnsigned&
MexUnsigned::infinity()
{
	static MexUnsigned result_( PLUS_INFINITY );
	return result_;
}

inline
MexUnsigned::Classification
MexUnsigned::classification( const MexUnsigned& a, const MexUnsigned& b )
{
	return Classification( ( a.cardinality_ << CARD_BITS ) | b.cardinality_ );
}

inline
MexUnsigned MexUnsigned::operator -() const
{
	PRE( not isInfinite() );
	return MexUnsigned( -value_ );
}

inline
const MexUnsigned&
MexUnsigned::operator +=( const MexUnsigned& rhs )
{
	switch( classification( *this, rhs ) )
	{
		case A_FINITE_B_FINITE			:
			value_ += rhs.value_;
		break;
		case A_FINITE_B_PLUS_INF		:
			cardinality_ = PLUS_INFINITY;
		break;
		case A_PLUS_INF_B_FINITE		:
		break;
		case A_PLUS_INF_B_PLUS_INF		:
		break;
		DEFAULT_ASSERT_BAD_CASE( "bad case" );
	}

	return *this;
}

inline
const MexUnsigned&
MexUnsigned::operator -=( const MexUnsigned& rhs )
{
	PRE( not rhs.isInfinite() );

	switch( classification( *this, rhs ) )
	{
		case A_FINITE_B_FINITE			:
			value_ -= rhs.value_;
		break;
		case A_FINITE_B_PLUS_INF		:
			ASSERT_FAIL( "Undefined Operation" );
		break;
		case A_PLUS_INF_B_FINITE		:
		break;
		case A_PLUS_INF_B_PLUS_INF		:
			ASSERT_FAIL( "Cannot subtract +infinity from +infinity" );
		break;
		DEFAULT_ASSERT_BAD_CASE( "bad case" );
	}

	return *this;
}

inline
const MexUnsigned&
MexUnsigned::operator *=( const MexUnsigned& rhs )
{
	switch( classification( *this, rhs ) )
	{
		case A_FINITE_B_FINITE			:
			value_ *= rhs.value_;
		break;
		case A_FINITE_B_PLUS_INF		:
			cardinality_ = PLUS_INFINITY;
		break;
		case A_PLUS_INF_B_FINITE		:
		break;
		case A_PLUS_INF_B_PLUS_INF		:
		break;
		DEFAULT_ASSERT_BAD_CASE( "bad case" );
	}

	return *this;
}

inline
const MexUnsigned&
MexUnsigned::operator /=( const MexUnsigned& rhs )
{
	PRE( implies( isInfinite(), not rhs.isInfinite() ) );
    PRE( implies( not rhs.isInfinite(), rhs.value_ != 0.0 ) );

	switch( classification( *this, rhs ) )
	{
		case A_FINITE_B_FINITE			:
			ASSERT( rhs.value_ != 0.0, "" );
			value_ /= rhs.value_;
		break;
		case A_FINITE_B_PLUS_INF		:
			cardinality_ = FINITE;
			value_ = 0;
		break;
		case A_PLUS_INF_B_FINITE		:
		break;
		case A_PLUS_INF_B_PLUS_INF		:
			ASSERT_FAIL( "Cannot divide +infinity by +infinity" );
		break;
		DEFAULT_ASSERT_BAD_CASE( "bad case" );
	}

	return *this;
}

inline
const MexUnsigned&
MexUnsigned::operator %=( const MexUnsigned& rhs )
{
	PRE( isInfinite() );
	PRE( not rhs.isInfinite() );
	PRE( rhs != 0.0 );

	switch( classification( *this, rhs ) )
	{
		case A_FINITE_B_FINITE			:
            value_ %= rhs.value_;
		break;
		case A_FINITE_B_PLUS_INF		:
			ASSERT_FAIL( "Undefined operation" );
		break;
		case A_PLUS_INF_B_FINITE		:
			ASSERT_FAIL( "Undefined operation" );
		break;
		case A_PLUS_INF_B_PLUS_INF		:
			ASSERT_FAIL( "Undefined operation" );
		break;
		DEFAULT_ASSERT_BAD_CASE( "bad case" );
	}

	return *this;
}

inline
bool operator  <( const MexUnsigned& a, const MexUnsigned& b )
{
	bool result;
	switch( MexUnsigned::classification( a, b ) )
	{
		case MexUnsigned::A_FINITE_B_PLUS_INF		:
			result = true;
		break;
		case MexUnsigned::A_PLUS_INF_B_FINITE		:
		case MexUnsigned::A_PLUS_INF_B_PLUS_INF		:
			result = false;
		break;
		case MexUnsigned::A_FINITE_B_FINITE			:
			result = a.value_ < b.value_;
		break;
		DEFAULT_ASSERT_BAD_CASE( "bad case" );
	}

	return result;
}

inline
bool operator ==( const MexUnsigned& a, const MexUnsigned& b )
{
	bool result;
	switch( MexUnsigned::classification( a, b ) )
	{
		case MexUnsigned::A_PLUS_INF_B_PLUS_INF		:
			result = true;
		break;
		case MexUnsigned::A_FINITE_B_PLUS_INF		:
			result = false;
		break;
		case MexUnsigned::A_FINITE_B_FINITE			:
			result = a.value_ == b.value_;
		break;
		DEFAULT_ASSERT_BAD_CASE( "bad case" );
	}

	return result;
}

inline
MexUnsigned operator +( const MexUnsigned& a, const MexUnsigned& b )
{
	MexUnsigned result = a;
	return MexUnsigned( result += b );
}

inline
MexUnsigned operator -( const MexUnsigned& a, const MexUnsigned& b )
{
	MexUnsigned result = a;
	return MexUnsigned( result -= b );
}

inline
MexUnsigned operator  *( const MexUnsigned& a, const MexUnsigned& b )
{
	MexUnsigned result = a;
	return MexUnsigned( result *= b );
}

inline
MexUnsigned operator /( const MexUnsigned& a, const MexUnsigned& b )
{
	MexUnsigned result = a;
	return MexUnsigned( result /= b );
}

inline
MexUnsigned operator %( const MexUnsigned& a, const MexUnsigned& b )
{
	PRE( b != 0.0 );
	MexUnsigned result = a;
	return MexUnsigned( result %= b );
}

inline
MexUnsigned sqrt( const MexUnsigned& d )
{
	MexUnsigned result = d;
	if( not result.isInfinite() )
		result.value_ = sqrt( result.value_ );

	POST( result >= 0.0 );
    return result;
}

inline
ostream& operator <<( ostream& o, const MexUnsigned& d )
{
	if( d.isInfinite() )
		o << "+infinity";
	else
		o << d.value_;

	return o;
}

/* //////////////////////////////////////////////////////////////// */

/* End UNSIGNED.IPP ******************************************************/
