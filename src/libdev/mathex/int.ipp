/*
 * I N T . I P P
 * (c) Charybdis Limited, 1997. All Rights Reserved.
 */

inline
MexInt::MexInt( int d )
: value_( d ),
  cardinality_( FINITE )
{
	// Intentionally Empty
}

inline
MexInt::MexInt( Cardinality c )
: value_( 0 ),
  cardinality_( c )
{
	// Intentionally Empty
}

inline
MexInt::operator int() const
{
	PRE( not isInfinite() );
	return value_;
}

inline
bool MexInt::isPlusInfinity() const
{
	return cardinality_ == PLUS_INFINITY;
}

inline
bool MexInt::isMinusInfinity() const
{
	return cardinality_ == MINUS_INFINITY;
}

inline
bool MexInt::isInfinite() const
{
	return cardinality_ != FINITE;
}

inline
const MexInt&
MexInt::plusInfinity()
{
	static MexInt result_( PLUS_INFINITY );
	return result_;
}

inline
const MexInt& MexInt::minusInfinity()
{
	static MexInt result_( MINUS_INFINITY );
	return result_;
}

inline
MexInt::Classification
MexInt::classification( const MexInt& a, const MexInt& b )
{
	return Classification( ( a.cardinality_ << CARD_BITS ) | b.cardinality_ );
}

inline
MexInt MexInt::operator -() const
{
	MexInt result;
	switch( cardinality_ )
	{
		case MINUS_INFINITY	:
			result = MexInt( PLUS_INFINITY );
		break;
		case FINITE	:
			result = MexInt( -value_ );
		break;
		case PLUS_INFINITY	:
			result = MexInt( MINUS_INFINITY );
		break;
	}

	return result;
}

inline
const MexInt&
MexInt::operator +=( const MexInt& rhs )
{
	PRE( implies( isMinusInfinity(), not rhs.isPlusInfinity() ) );
	PRE( implies( isPlusInfinity(), not rhs.isMinusInfinity() ) );

	switch( classification( *this, rhs ) )
	{
		case A_MINUS_INF_B_MINUS_INF	:
		break;
		case A_MINUS_INF_B_FINITE		:
		break;
		case A_MINUS_INF_B_PLUS_INF		:
			ASSERT_FAIL( "Cannot add +infinity to -infinity" );
		break;
		case A_FINITE_B_MINUS_INF		:
			cardinality_ = MINUS_INFINITY;
		break;
		case A_FINITE_B_FINITE			:
			value_ += rhs.value_;
		break;
		case A_FINITE_B_PLUS_INF		:
			cardinality_ = PLUS_INFINITY;
		break;
		case A_PLUS_INF_B_MINUS_INF		:
			ASSERT_FAIL( "Cannot add +infinity to -infinity" );
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
const MexInt&
MexInt::operator -=( const MexInt& rhs )
{
	PRE( implies( isMinusInfinity(), not rhs.isPlusInfinity() ) );
	PRE( implies( isPlusInfinity(), not rhs.isMinusInfinity() ) );

	switch( classification( *this, rhs ) )
	{
		case A_MINUS_INF_B_MINUS_INF	:
			ASSERT_FAIL( "Cannot subtract -infinity from -infinity" );
		break;
		case A_MINUS_INF_B_FINITE		:
		break;
		case A_MINUS_INF_B_PLUS_INF		:
		break;
		case A_FINITE_B_MINUS_INF		:
			cardinality_ = PLUS_INFINITY;
		break;
		case A_FINITE_B_FINITE			:
			value_ -= rhs.value_;
		break;
		case A_FINITE_B_PLUS_INF		:
			cardinality_ = MINUS_INFINITY;
		break;
		case A_PLUS_INF_B_MINUS_INF		:
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
const MexInt&
MexInt::operator *=( const MexInt& rhs )
{
	switch( classification( *this, rhs ) )
	{
		case A_MINUS_INF_B_MINUS_INF	:
			cardinality_ = PLUS_INFINITY;
		break;
		case A_MINUS_INF_B_FINITE		:
			if( rhs.value_ < 0 )
				cardinality_ = PLUS_INFINITY;
		break;
		case A_MINUS_INF_B_PLUS_INF		:
		break;
		case A_FINITE_B_MINUS_INF		:
			cardinality_ = value_ < 0 ? PLUS_INFINITY : MINUS_INFINITY;
		break;
		case A_FINITE_B_FINITE			:
			value_ *= rhs.value_;
		break;
		case A_FINITE_B_PLUS_INF		:
			cardinality_ = value_ < 0 ? MINUS_INFINITY : PLUS_INFINITY;
		break;
		case A_PLUS_INF_B_MINUS_INF		:
			cardinality_ = MINUS_INFINITY;
		break;
		case A_PLUS_INF_B_FINITE		:
			if( rhs.value_ < 0 )
				cardinality_ = MINUS_INFINITY;
		break;
		case A_PLUS_INF_B_PLUS_INF		:
		break;
		DEFAULT_ASSERT_BAD_CASE( "bad case" );
	}

	return *this;
}

inline
const MexInt&
MexInt::operator /=( const MexInt& rhs )
{
	PRE( implies( isInfinite(), not rhs.isInfinite() ) );
    PRE( implies( not isInfinite(), value_ != 0.0 ) );

	switch( classification( *this, rhs ) )
	{
		case A_MINUS_INF_B_MINUS_INF	:
			ASSERT_FAIL( "Cannot divide -infinity by -infinity" );
		break;
		case A_MINUS_INF_B_FINITE		:
			if( rhs.value_ < 0 )
				cardinality_ = PLUS_INFINITY;
		break;
		case A_MINUS_INF_B_PLUS_INF		:
			ASSERT_FAIL( "Cannot divide -infinity by +infinity" );
		break;
		case A_FINITE_B_MINUS_INF		:
			cardinality_ = FINITE;
			value_ = 0;
		break;
		case A_FINITE_B_FINITE			:
			ASSERT( rhs.value_ != 0.0, "" );
			value_ /= rhs.value_;
		break;
		case A_FINITE_B_PLUS_INF		:
			cardinality_ = FINITE;
			value_ = 0;
		break;
		case A_PLUS_INF_B_MINUS_INF		:
			ASSERT_FAIL( "Cannot divide +infinity by -infinity" );
		break;
		case A_PLUS_INF_B_FINITE		:
			if( rhs.value_ < 0 )
				cardinality_ = MINUS_INFINITY;
		break;
		case A_PLUS_INF_B_PLUS_INF		:
			ASSERT_FAIL( "Cannot divide +infinity by +infinity" );
		break;
		DEFAULT_ASSERT_BAD_CASE( "bad case" );
	}

	return *this;
}

inline
const MexInt&
MexInt::operator %=( const MexInt& rhs )
{
	PRE( not isInfinite() );
	PRE( not rhs.isInfinite() );
	PRE( rhs != 0.0 );

	switch( classification( *this, rhs ) )
	{
		case A_MINUS_INF_B_MINUS_INF	:
			ASSERT_FAIL( "Undefined operation" );
		break;
		case A_MINUS_INF_B_FINITE		:
			ASSERT_FAIL( "Undefined operation" );
		break;
		case A_MINUS_INF_B_PLUS_INF		:
			ASSERT_FAIL( "Undefined operation" );
		break;
		case A_FINITE_B_MINUS_INF		:
			ASSERT_FAIL( "Undefined operation" );
		break;
		case A_FINITE_B_FINITE			:
            value_ %= rhs.value_;
		break;
		case A_FINITE_B_PLUS_INF		:
			ASSERT_FAIL( "Undefined operation" );
		break;
		case A_PLUS_INF_B_MINUS_INF		:
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
bool operator  <( const MexInt& a, const MexInt& b )
{
	bool result;
	switch( MexInt::classification( a, b ) )
	{
		case MexInt::A_MINUS_INF_B_FINITE		:
		case MexInt::A_MINUS_INF_B_PLUS_INF		:
		case MexInt::A_FINITE_B_PLUS_INF		:
			result = true;
		break;
		case MexInt::A_FINITE_B_MINUS_INF		:
		case MexInt::A_PLUS_INF_B_MINUS_INF		:
		case MexInt::A_PLUS_INF_B_FINITE		:
		case MexInt::A_PLUS_INF_B_PLUS_INF		:
		case MexInt::A_MINUS_INF_B_MINUS_INF	:
			result = false;
		break;
		case MexInt::A_FINITE_B_FINITE			:
			result = a.value_ < b.value_;
		break;
		DEFAULT_ASSERT_BAD_CASE( "bad case" );
	}

	return result;
}

inline
bool operator ==( const MexInt& a, const MexInt& b )
{
	bool result;
	switch( MexInt::classification( a, b ) )
	{
		case MexInt::A_MINUS_INF_B_MINUS_INF	:
		case MexInt::A_PLUS_INF_B_PLUS_INF		:
			result = true;
		break;
		case MexInt::A_MINUS_INF_B_FINITE		:
		case MexInt::A_MINUS_INF_B_PLUS_INF		:
		case MexInt::A_FINITE_B_MINUS_INF		:
		case MexInt::A_FINITE_B_PLUS_INF		:
		case MexInt::A_PLUS_INF_B_MINUS_INF		:
			result = false;
		break;
		case MexInt::A_FINITE_B_FINITE			:
			result = a.value_ == b.value_;
		break;
		DEFAULT_ASSERT_BAD_CASE( "bad case" );
	}

	return result;
}

inline
MexInt operator +( const MexInt& a, const MexInt& b )
{
	MexInt result = a;
	return MexInt( result += b );
}

inline
MexInt operator -( const MexInt& a, const MexInt& b )
{
	MexInt result = a;
	return MexInt( result -= b );
}

inline
MexInt operator  *( const MexInt& a, const MexInt& b )
{
	MexInt result = a;
	return MexInt( result *= b );
}

inline
MexInt operator /( const MexInt& a, const MexInt& b )
{
	MexInt result = a;
	return MexInt( result /= b );
}

inline
MexInt operator %( const MexInt& a, const MexInt& b )
{
	PRE( b != 0.0 );
	MexInt result = a;
	return MexInt( result %= b );
}

inline
MexInt sqrt( const MexInt& d )
{
    PRE( not d.isMinusInfinity() );
    PRE( implies( not d.isInfinite(), d >= 0.0 ) );

    MexInt   result;

    if( d.isPlusInfinity() )
        result = d;
    else
        result = sqrt( d.value_ );

    POST( result >= 0.0 );

    return result;
}

inline
ostream& operator <<( ostream& o, const MexInt& d )
{
	if( d.isPlusInfinity() )
		o << "+infinity";
	else if( d.isMinusInfinity() )
		o << "-infinity";
	else
		o << d.value_;

	return o;
}

/* //////////////////////////////////////////////////////////////// */

/* End INT.IPP ******************************************************/
