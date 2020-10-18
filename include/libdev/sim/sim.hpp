/*
 * S I M . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
	General include for the Sim library, with forward declarations.
*/

#ifndef _SIM_HPP
#define _SIM_HPP

//Forward declarations
typedef uint SimPriority;
class SimManager;
class SimProcess;
class SimActor;
class SimEventDiary;
class SimDiscreteEvent;

//Collection of SimActor*'s
template< class T > class ctl_vector;
typedef ctl_vector<SimActor*> SimActors;

#endif //ndefined _SIM_HPP
/* End SIM.HPP **************************************************/
