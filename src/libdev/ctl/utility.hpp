/*
 * U T I L I T Y . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    Macros to make defining simple classes consisting
    of 2, 3 or 4 data members easy. These are like a
    friendlier version of the standard library "pair"
    class - friendlier because they allow the names of
    the data members to be specified.
*/

#ifndef _CTL_UTILITY_HPP
#define _CTL_UTILITY_HPP

//#include "stdlib/utility.hpp"
#include <algorithm>

//////////////////////////////////////////////////////////////////////

template < class FIRST, class SECOND >
std::pair< const FIRST, SECOND >
ctl_make_const_first_pair( const FIRST& first, const SECOND& second )
// In the absence of a conforming compiler, ctl_make_const_first_pair can be used
// in place of make_pair for inserting elements into associative containers.
{
	return std::pair< const FIRST, SECOND >( first, second );
}

//////////////////////////////////////////////////////////////////////

#define	CTL_PAIR( className, type1, member1, type2, member2 )		\
	class className : public std::pair< type1, type2 >					\
	{																\
	public:															\
		className()													\
		{}															\
		className( className const & copyMe )						\
		: std::pair< type1, type2 >( copyMe )							\
		{}															\
		className( type1 const & m1, type2 const & m2 )				\
		: std::pair< type1, type2 >( m1, m2 )							\
		{}															\
		type1 const & member1() const								\
		{															\
			return first;											\
		}															\
		void member1( type1 const & t1 )							\
		{															\
			first = t1;				    							\
		}															\
		type2 const & member2() const								\
		{															\
			return second;											\
		}															\
		void member2( type2 const & t2 )							\
		{															\
			second = t2;											\
		}															\
        bool operator ==( className const & c2 ) const              \
        {                                                           \
            return first == c2.first and second == c2.second;       \
        }                                                           \
        bool operator <( className const & c2 ) const                \
        {                                                           \
            return first < c2.first or( not( c2.first < first ) and \
              second < c2.second );                                 \
        }                                                           \
	private:														\
	};

#define	CTL_TRIPLE( className, type1, member1, type2, member2, type3, member3 )			\
	class className                             					\
	{																\
	public:															\
		className()													\
		{}															\
		className( className const & copyMe )						\
		: first( copyMe.first ),                                    \
          second( copyMe.second ),                                  \
          third( copyMe.third )                                     \
		{}															\
		className( type1 const & m1,                                 \
          type2 const & m2, type3 const & m3 )				        \
		: first( m1 ), second( m2 ), third( m3 )                    \
		{}															\
		type1 const & member1() const								\
		{															\
			return first;											\
		}															\
		void member1( type1 const & t1 )								\
		{															\
			first = t1;	    										\
		}															\
		type2 const & member2() const								\
		{															\
			return second;											\
		}															\
		void member2( type2 const & t2 )     					    \
		{															\
			second = t2;											\
		}															\
		type3 const & member3() const								\
		{															\
			return third;											\
		}															\
		void member3( type3 const & t3 )     	   					\
		{															\
			third = t3;	    										\
		}															\
        bool operator ==( className const & c2 ) const               \
        {                                                           \
            return first == c2.first and                            \
              second == c2.second and                               \
              third == c2.third;                                    \
        }                                                           \
        bool operator <( className const & c2 ) const                \
        {                                                           \
            if( first < c2.first )                                  \
                return true;                                        \
            if( c2.first < first )                                  \
                return false;                                       \
            if( second < c2.second )                                \
                return true;                                        \
            if( c2.second < second )                                \
                return false;                                       \
            if( third < c2.third )                                  \
                return true;                                        \
            return false;                                           \
        }                                                           \
	private:														\
        type1   first;                                              \
        type2   second;                                             \
        type3   third;                                              \
	};

#define	CTL_QUAD( className, type1, member1, type2, member2, type3, member3, type4, member4 )			\
	class className                             					\
	{																\
	public:															\
		className()													\
		{}															\
		className( className const & copyMe )						\
		: first( copyMe.first ),                                    \
          second( copyMe.second ),                                  \
          third( copyMe.third ),                                    \
          fourth( copyMe.fourth )                                   \
   		{}															\
		className( type1 const & m1,                                 \
          type2 const & m2,                                          \
          type3 const & m3,                                          \
          type4 const & m4 )	                            			\
		: first( m1 ), second( m2 ), third( m3 ), fourth( m4 )      \
		{}															\
		type1 const & member1() const								\
		{															\
			return first;											\
		}															\
		void member1( type1 const & t1 )								\
		{															\
			first = t1;	    										\
		}															\
		type2 const & member2() const								\
		{															\
			return second;											\
		}															\
		void member2( type2 const & t2 )     					    \
		{															\
			second = t2;											\
		}															\
		type3 const & member3() const								\
		{															\
			return third;											\
		}															\
		void member3( type3 const & t3 )     	   					\
		{															\
			third = t3;	    										\
		}															\
		type4 const & member4() const								\
		{															\
			return fourth;											\
		}															\
		void member4( type4 const & t4 )     	   					\
		{															\
			fourth = t4;	    									\
		}															\
        bool operator ==( className const & c2 ) const               \
        {                                                           \
            return first == c2.first and                            \
               second == c2.second and                              \
               third == c2.third and                                \
               fourth == c2.fourth;                                 \
        }                                                           \
        bool operator <( className const & c2 ) const                \
        {                                                           \
            if( first < c2.first )                                  \
                return true;                                        \
            if( c2.first < first )                                  \
                return false;                                       \
            if( second < c2.second )                                \
                return true;                                        \
            if( c2.second < second )                                \
                return false;                                       \
            if( third < c2.third )                                  \
                return true;                                        \
            if( c2.third < third )                                  \
                return false;                                       \
            if( fourth < c2.fourth )                                \
                return true;                                        \
            return false;                                           \
        }                                                           \
	private:														\
        type1   first;                                              \
        type2   second;                                             \
        type3   third;                                              \
        type4   fourth;                                             \
	};

#endif
