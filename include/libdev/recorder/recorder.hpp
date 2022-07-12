/*
 * R E C O R D E R . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    RecRecorder

    A brief description of the class should go in here
*/

#ifndef _RECORDER_RECORDER_HPP
#define _RECORDER_RECORDER_HPP

#include "base/base.hpp"
#include "utility/DependencyProvider.hpp"

class RecRecorder
// Canonical form revoked
{
public:
    //  Singleton class
    static RecRecorder& instance();
    ~RecRecorder();

    enum State { INACTIVE, RECORDING, PLAYING };

    State state() const;

    //  Write out details of what the recorder is currently doing
    void writeDetails( ostream& ) const;
    
    //  Set this to false around parts of the program where no recording should
    //  be taking place - e.g. the Windows message loop. If any recording is
    //  attempted at these points the program will assert out.
    void recordingAllowed( bool );
    
	double percentageComplete() const;
	//	PRE( state() == PLAYING );
	//	POST( result >= 0.0 );
	//	POST( result <= 100.0 );

    void CLASS_INVARIANT;

private:
    friend ostream& operator <<( ostream& o, const RecRecorder& t );

    RecRecorder( const RecRecorder& );
    RecRecorder& operator =( const RecRecorder& );

    RecRecorder();
};

/* *******************************************************
 * SINGLETON DEPENDENCY PROVIDER
 */
template<>
inline RecRecorder& DependencyProvider<RecRecorder>::getProvided()
{
    return RecRecorder::instance();
}


#endif

/* End RECORDER.HPP *************************************************/
