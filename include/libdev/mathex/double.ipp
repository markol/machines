/*
 * D O U B L E . I P P
 * (c) Charybdis Limited, 1997. All Rights Reserved.
 */

inline
MexDouble::MexDouble( double d )
: rep_( d, FINITE )
{
	// Intentionally Empty
}

inline
MexDouble::MexDouble( Cardinality c )
: rep_( 0.0, c )
{
	// Intentionally Empty
}

inline
MexDouble::operator double() const
{
	PRE( not isInfinite() );

	return rep_.value_;
}

inline
bool MexDouble::isPlusInfinity() const
{
	return rep_.cardinality_ == PLUS_INFINITY;
}

inline
bool MexDouble::isMinusInfinity() const
{
	return rep_.cardinality_ == MINUS_INFINITY;
}

inline
bool MexDouble::isInfinite() const
{
	return rep_.cardinality_ != FINITE;
}

inline
const MexDouble&
MexDouble::plusInfinity()
{
	static MexDouble result_( PLUS_INFINITY );
	return result_;
}

inline
const MexDouble& MexDouble::minusInfinity()
{
	static MexDouble result_( MINUS_INFINITY );
	return result_;
}

inline
MexDouble::Classification
MexDouble::classification( const MexDouble& a, const MexDouble& b )
{
	return Classification( ( a.rep_.cardinality_ << CARD_BITS ) | b.rep_.cardinality_ );
}

inline
MexDouble MexDouble::operator -() const
{
	MexDouble result = -rep_.value_;
	switch( rep_.cardinality_ )
	{
		case MINUS_INFINITY	:
			result = MexDouble( PLUS_INFINITY );
		break;
		case FINITE	:
			result = MexDouble( -rep_.value_ );
		break;
		case PLUS_INFINITY	:
			result = MexDouble( MINUS_INFINITY );
		break;
	}

	return result;
}

inline
const MexDouble&
MexDouble::operator +=( const MexDouble& rhs )
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
			rep_.cardinality_ = MINUS_INFINITY;
			break;
		case A_FINITE_B_FINITE			:
			rep_.value_ += rhs.rep_.value_;
			break;
		case A_FINITE_B_PLUS_INF		:
			rep_.cardinality_ = PLUS_INFINITY;
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
const MexDouble&
MexDouble::operator -=( const MexDouble& rhs )
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
			rep_.cardinality_ = PLUS_INFINITY;
			break;
		case A_FINITE_B_FINITE			:
			rep_.value_ -= rhs.rep_.value_;
			break;
		case A_FINITE_B_PLUS_INF		:
			rep_.cardinality_ = MINUS_INFINITY;
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
const MexDouble&
MexDouble::operator *=( const MexDouble& rhs )
{
	switch( classification( *this, rhs ) )
	{
		case A_MINUS_INF_B_MINUS_INF	:
			rep_.cardinality_ = PLUS_INFINITY;
			break;
		case A_MINUS_INF_B_FINITE		:
			if( rhs.rep_.value_ < 0 )
				rep_.cardinality_ = PLUS_INFINITY;
			break;
		case A_MINUS_INF_B_PLUS_INF		:
			break;
		case A_FINITE_B_MINUS_INF		:
			rep_.cardinality_ = rep_.value_ < 0 ? PLUS_INFINITY : MINUS_INFINITY;
			break;
		case A_FINITE_B_FINITE			:
			rep_.value_ *= rhs.rep_.value_;
			break;
		case A_FINITE_B_PLUS_INF		:
			rep_.cardinality_ = rep_.value_ < 0 ? MINUS_INFINITY : PLUS_INFINITY;
			break;
		case A_PLUS_INF_B_MINUS_INF		:
			rep_.cardinality_ = MINUS_INFINITY;
			break;
		case A_PLUS_INF_B_FINITE		:
			if( rhs.rep_.value_ < 0 )
				rep_.cardinality_ = MINUS_INFINITY;
			break;
		case A_PLUS_INF_B_PLUS_INF		:
			break;
		DEFAULT_ASSERT_BAD_CASE( "bad case" );
	}

	return *this;
}

inline
const MexDouble&
MexDouble::operator /=( const MexDouble& rhs )
{
	PRE( implies( isInfinite(), not rhs.isInfinite() ) );
    PRE( implies( not rhs.isInfinite(), rhs.rep_.value_ != 0.0 ) );

	switch( classification( *this, rhs ) )
	{
		case A_MINUS_INF_B_MINUS_INF	:
			ASSERT_FAIL( "Cannot divide -infinity by -infinity" );
			break;
		case A_MINUS_INF_B_FINITE		:
			if( rhs.rep_.value_ < 0 )
				rep_.cardinality_ = PLUS_INFINITY;
			break;
		case A_MINUS_INF_B_PLUS_INF		:
			ASSERT_FAIL( "Cannot divide -infinity by +infinity" );
			break;
		case A_FINITE_B_MINUS_INF		:
			rep_.cardinality_ = FINITE;
			rep_.value_ = 0;
			break;
		case A_FINITE_B_FINITE			:
			ASSERT( rhs.rep_.value_ != 0.0, "" );
			rep_.value_ /= rhs.rep_.value_;
			break;
		case A_FINITE_B_PLUS_INF		:
			rep_.cardinality_ = FINITE;
			rep_.value_ = 0;
			break;
		case A_PLUS_INF_B_MINUS_INF		:
			ASSERT_FAIL( "Cannot divide +infinity by -infinity" );
			break;
		case A_PLUS_INF_B_FINITE		:
			if( rhs.rep_.value_ < 0 )
				rep_.cardinality_ = MINUS_INFINITY;
			break;
		case A_PLUS_INF_B_PLUS_INF		:
			ASSERT_FAIL( "Cannot divide +infinity by +infinity" );
			break;
		DEFAULT_ASSERT_BAD_CASE( "bad case" );
	}

	return *this;
}

inline
const MexDouble&
MexDouble::operator %=( const MexDouble& rhs )
{
	PRE( implies( not rhs.isInfinite(), rhs != MexDouble(0.0) ) );

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
			ASSERT_FAIL( "Undefined operation" );
//            rep_.value_ %= rhs.rep_.value_;
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
bool operator  <( const MexDouble& a, const MexDouble& b )
{
	bool result = false;
	switch( MexDouble::classification( a, b ) )
	{
		case MexDouble::A_MINUS_INF_B_FINITE		:
		case MexDouble::A_MINUS_INF_B_PLUS_INF		:
		case MexDouble::A_FINITE_B_PLUS_INF		:
			result = true;
			break;
		case MexDouble::A_FINITE_B_MINUS_INF		:
		case MexDouble::A_PLUS_INF_B_MINUS_INF		:
		case MexDouble::A_PLUS_INF_B_FINITE		:
		case MexDouble::A_PLUS_INF_B_PLUS_INF		:
		case MexDouble::A_MINUS_INF_B_MINUS_INF	:
			result = false;
			break;
		case MexDouble::A_FINITE_B_FINITE			:
			result = a.rep_.value_ < b.rep_.value_;
			break;
		DEFAULT_ASSERT_BAD_CASE( "bad case" );
	}

	return result;
}

inline
bool operator  >( const MexDouble& a, const MexDouble& b )
{
    return (b < a);
}

inline
bool operator ==( const MexDouble& a, const MexDouble& b )
{
	bool result = false;
	switch( MexDouble::classification( a, b ) )
	{
		case MexDouble::A_MINUS_INF_B_MINUS_INF	:
		case MexDouble::A_PLUS_INF_B_PLUS_INF		:
			result = true;
			break;
		case MexDouble::A_MINUS_INF_B_FINITE		:
		case MexDouble::A_MINUS_INF_B_PLUS_INF		:
		case MexDouble::A_FINITE_B_MINUS_INF		:
		case MexDouble::A_FINITE_B_PLUS_INF		:
		case MexDouble::A_PLUS_INF_B_MINUS_INF		:
			result = false;
			break;
		case MexDouble::A_FINITE_B_FINITE			:
			result = a.rep_.value_ == b.rep_.value_;
			break;
		DEFAULT_ASSERT_BAD_CASE( "bad case" );
	}

	return result;
}

inline
bool operator !=( const MexDouble& a, const MexDouble& b )
{
    return !(a == b);
}

inline
bool operator  >=( const MexDouble& a, const MexDouble& b )
{
    return !(a < b);
}

inline
bool operator  <=( const MexDouble& a, const MexDouble& b )
{
    return !(b < a);
}

inline
MexDouble operator +( const MexDouble& a, const MexDouble& b )
{
	MexDouble result = a;
	return MexDouble( result += b );
}

inline
MexDouble operator -( const MexDouble& a, const MexDouble& b )
{
	MexDouble result = a;
	return MexDouble( result -= b );
}

inline
MexDouble operator  *( const MexDouble& a, const MexDouble& b )
{
	MexDouble result = a;
	return MexDouble( result *= b );
}

inline
MexDouble operator /( const MexDouble& a, const MexDouble& b )
{
	MexDouble result = a;
	return MexDouble( result /= b );
}

inline
MexDouble operator %( const MexDouble& a, const MexDouble& b )
{
	PRE( b != MexDouble(0.0) );
	MexDouble result = a;
	return MexDouble( result %= b );
}

inline
MexDouble sqrt( const MexDouble& d )
{
    PRE( not d.isMinusInfinity() );
    PRE( implies( not d.isInfinite(), d >= MexDouble(0.0) ) );

    MexDouble   result = d.rep_.value_;

    if( d.isPlusInfinity() )
        result = d;
    else
        result = sqrt( d.rep_.value_ );

    POST( result >= MexDouble(0.0) );

    return result;
}

inline
ostream& operator <<( ostream& o, const MexDouble& d )
{
	if( d.isPlusInfinity() )
		o << "+infinity";
	else if( d.isMinusInfinity() )
		o << "-infinity";
	else
		o << d.rep_.value_;

	return o;
}

/* //////////////////////////////////////////////////////////////// */

/* End DOUBLE.IPP *************************************************/
