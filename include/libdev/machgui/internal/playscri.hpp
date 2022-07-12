/*
 * P L A Y S C R I . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachGuiPlayerScore

    A brief description of the class should go in here
*/

#ifndef _PLAYSCRI_HPP
#define _PLAYSCRI_HPP

#include "base/base.hpp"
#include "machgui/statistics.hpp"
#include "machgui/internal/playeri.hpp"
#include "machgui/statdisp.hpp"

class MachGuiPlayerScore
:	public MachGuiStatistics,
	public MachGuiPlayerNameList
// Canonical form revoked
{
public:
    MachGuiPlayerScore( GuiDisplayable *pParent, const Gui::Coord& topLeft );
    ~MachGuiPlayerScore();

    void CLASS_INVARIANT;

	virtual	void update();

	virtual void setStatistics( int, int, int, int );

private:
    friend ostream& operator <<( ostream& o, const MachGuiPlayerScore& t );

    MachGuiPlayerScore( const MachGuiPlayerScore& );
    MachGuiPlayerScore& operator =( const MachGuiPlayerScore& );

	MachGuiStatisticsDisplay statDisplay_;
};


#endif

/* End PLAYSCRI.HPP *************************************************/
