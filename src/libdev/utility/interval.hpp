
#ifndef _UTL_INTERVAL_HPP
#define _UTL_INTERVAL_HPP

// template class UtlOpenInterval< NUMERIC > represents
// the open interval [ lower(), upper() ], i.e. the range of
// consecutive values of NUMERIC from lower() to upper() inclusive.

template < class NUMERIC >
class UtlOpenInterval
// cannonical according as NUMERIC
{
public:

	typedef NUMERIC NumericType;

 	UtlOpenInterval();
	// POST( lower() == 0 );
	// POST( upper() == lower() );

	UtlOpenInterval( NUMERIC fl, NUMERIC cl );
	// PRE( isOpenInterval( fl, cl ) );
	// POST( lower() == fl );
	// POST( upper() == cl );

	///////////////////////////////

	bool isBefore( const UtlOpenInterval& other ) const;
	// POST( iff( result, upper() < other.lower() );
	// POST( iff( result, other.isAfter( *this ) ) );

	bool isAfter( const UtlOpenInterval& ) const;
	// POST( iff( result, lower() > other.upper() );
	// POST( iff( result, other.isBefore( *this ) ) );

	bool meets( const UtlOpenInterval& ) const;
	// POST( iff( result, upper() == other.lower() );

	bool overlaps( const UtlOpenInterval& ) const;
	// POST( iff( result, contains( other.lower() )
	//						or contains( other.upper() ) );

	bool contains( NUMERIC x ) const;
	// POST( iff( result, lower() <= x <= upper() ) );

	bool contains( const UtlOpenInterval& other ) const;
	// POST( iff( result, contains( other.lower() )
	//						and contains( other.upper() ) );

	///////////////////////////////

	NUMERIC lower() const;
	NUMERIC upper() const;

	NUMERIC length() const;
	// POST( result == upper() - lower() );

	///////////////////////////////

	void lower( NUMERIC newlower );
	// PRE( isOpenInterval( newlower, upper() );
	// POST( lower() == newlower );

	void upper( NUMERIC newupper );
	// PRE( isOpenInterval( lower(), newupper );
	// POST( upper() == newupper );

	///////////////////////////////

	static bool isOpenInterval( NUMERIC lower, NUMERIC upper );
	// POST( iff( result, lower <= upper ) );

private:

	void CLASS_INVARIANT
	{
		INVARIANT( isOpenInterval( lower(), upper() ) );
	}

	NUMERIC		lower_;
	NUMERIC		upper_;

	friend ostream& operator <<( ostream& o, const UtlOpenInterval< NUMERIC >& i );

	friend bool operator <( const UtlOpenInterval& a, const UtlOpenInterval& b );
	// POST( iff( result, a.isBefore( b ) ) );

	friend bool operator ==( const UtlOpenInterval& a, const UtlOpenInterval& b );
};

/* //////////////////////////////////////////////////////////////// */

template < class NUMERIC >
UtlOpenInterval< NUMERIC >::UtlOpenInterval()
: lower_( 0 ),
  upper_( 0 )
{
	POST( lower() == 0 );
	POST( upper() == lower() );

	TEST_INVARIANT;
}

template < class NUMERIC >
UtlOpenInterval< NUMERIC >::UtlOpenInterval( NUMERIC fl, NUMERIC cl )
: lower_( fl ),
  upper_( cl )
{
	PRE( isOpenInterval( fl, cl ) );

	POST( lower() == fl );
	POST( upper() == cl );

	TEST_INVARIANT;
}

template < class NUMERIC >
bool operator  <( const UtlOpenInterval< NUMERIC >& a,
								const UtlOpenInterval< NUMERIC >& b )
{
	return a.isBefore( b );
}

template < class NUMERIC >
bool operator  ==( const UtlOpenInterval< NUMERIC >& a,
								const UtlOpenInterval< NUMERIC >& b )
{
	return a.lower() == b.lower() and a.upper() == b.upper();
}

// static
template < class NUMERIC >
bool
UtlOpenInterval< NUMERIC >::isOpenInterval( NUMERIC lower, NUMERIC upper )
{
	return lower <= upper;
}

template < class NUMERIC >
bool UtlOpenInterval< NUMERIC >::contains( NUMERIC x ) const
{
	bool result = lower() <= x and x <= upper();
	POST( iff( result, lower() <= x and x <= upper() ) );
	return result;
}

template < class NUMERIC >
bool UtlOpenInterval< NUMERIC >::isBefore( const UtlOpenInterval& other ) const
{
	bool result = upper() < other.lower();
	POST( iff( result, other.isAfter( *this ) ) );
	return result;
}

template < class NUMERIC >
bool UtlOpenInterval< NUMERIC >::isAfter( const UtlOpenInterval& other ) const
{
	bool result = lower() > other.upper();
	POST( iff( result, other.isBefore( *this ) ) );
	return result;
}

template < class NUMERIC >
bool UtlOpenInterval< NUMERIC >::meets( const UtlOpenInterval& other ) const
{
	return upper() == other.lower();
}

template < class NUMERIC >
bool UtlOpenInterval< NUMERIC >::overlaps( const UtlOpenInterval& other ) const
{
	return contains( other.lower() ) or contains( other.upper() );
}

template < class NUMERIC >
bool UtlOpenInterval< NUMERIC >::contains( const UtlOpenInterval& other ) const
{
	return contains( other.lower() ) and contains( other.upper() );
}

template < class NUMERIC >
NUMERIC UtlOpenInterval< NUMERIC >::lower() const
{
	return lower_;
}

template < class NUMERIC >
void UtlOpenInterval< NUMERIC >::lower( NUMERIC newlower )
{
	PRE( isOpenInterval( newlower, upper() ) );
	lower_ = newlower;
	POST( lower() == newlower );
}

template < class NUMERIC >
NUMERIC UtlOpenInterval< NUMERIC >::upper() const
{
	return upper_;
}

template < class NUMERIC >
void UtlOpenInterval< NUMERIC >::upper( NUMERIC newupper )
{
	PRE( isOpenInterval( lower(), newupper ) );
	upper_ = newupper;
	POST( upper() == newupper );
}

template < class NUMERIC >
NUMERIC UtlOpenInterval< NUMERIC >::length() const
{
	return upper() - lower();
}

//////////////////////////////////////////////////////////////////////

#endif	// #ifndef _UTL_INTERVAL_HPP

/* End _UTL_INTERVAL_HPP ********************************************/
