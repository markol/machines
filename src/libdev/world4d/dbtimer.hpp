/*
 * D B T I M E R . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    W4dDebugTimer

    A brief description of the class should go in here
*/

#ifndef _DBTIMER_HPP
#define _DBTIMER_HPP

#include "base/base.hpp"

#include "utility/time.hpp"
#include "ctl/vector.hpp"
#include <string.hpp>

class W4dDebugTimer
// Canonical form revoked
{
public:
    // Singleton class
    ~W4dDebugTimer() {
       for( int i =0; i< textSnaps_.size(); ++i )
	   {
	     LIONEL_STREAM( textSnaps_[i] << timeSnaps_[i] << endl);
	   }
    }

    static W4dDebugTimer& instance()
	{
    	static W4dDebugTimer instance_;
    	return instance_;
	}
	UtlDebugTimer& timer() { return timer_;	}

	bool output() const { return output_; }
	void output( bool doOutput ) { output_ = doOutput; }
	bool outputL2() const { return outputL2_; }
	void outputL2( bool doOutput ) { outputL2_ = doOutput; }

	void snapshot( const string& id )
	{
		
		textSnaps_.push_back( id + " at time "  );
		timeSnaps_.push_back( timer_.time().asDouble() );
	}

    void CLASS_INVARIANT;

private:
    friend ostream& operator <<( ostream& o, const W4dDebugTimer& t );

    W4dDebugTimer()  { 
		timer_.calibrate();
    	timer_.pause();
		output_=false;
		outputL2_=false;
		textSnaps_.reserve(20000);
		timeSnaps_.reserve(20000);
    }
    W4dDebugTimer( const W4dDebugTimer& );
    W4dDebugTimer& operator =( const W4dDebugTimer& );

	UtlDebugTimer timer_;
	bool output_;
	bool outputL2_;
	ctl_vector< string > textSnaps_;
	ctl_vector< double > timeSnaps_;
};


#endif

/* End DBTIMER.HPP **************************************************/
