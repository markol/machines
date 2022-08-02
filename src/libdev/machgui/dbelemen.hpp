/*
 * D B E L E M E N . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachGuiDbElement

    A base class for the elements of the scenario database, such as system, planet
    and scenario, which share some common data.
*/

#ifndef _MACHGUI_DBELEMEN_HPP
#define _MACHGUI_DBELEMEN_HPP

#include "base/base.hpp"
#include "base/persist.hpp"
#include "stdlib/string.hpp"

//forward refs
class MachGuiDbIElement;
class MachGuiDbTextData;
template <class T> class ctl_vector;

class MachGuiDbElement
// Canonical form revoked
{
public:
    //menuStringId is the resource table id for the string to appear in menu list boxes.
    MachGuiDbElement( uint menuStringId );

    virtual ~MachGuiDbElement();

    //Set/get the name of the element as defined in the database definition file.
    //This name is used to generate eg scenario and planet file names.
    void name( const string& name );
    const string& name() const;

	//Get at the stringId used as a lookup into the string table resource. This string
	//is displayed in list boxes etc.
	uint menuStringId() const;
	string menuString() const;
	void menuString(const string& name);

    //Set/get the text data object source file relative path.
    //The source version path is relative to the data directory.
    //A persistent version gets created in pdata.
    void textDataFileName( const string& filename );
    const string& textDataFileName() const;

    //The text data object associated with this element
    const MachGuiDbTextData& textData() const;

    //Clear out the cached text data object
    void clearTextData();

    //Indicates that a player must complete pAntecedent before this element can be available
    void addDependency( MachGuiDbElement* pAntecedent );

	// Get all the other MachGuiDbElement that this one relies on to become available
	const ctl_vector< MachGuiDbElement* >& antecedents() const;

    //True if all this element's antecedents have been completed
    bool isAvailable() const;

    //set/get the complete status flag
    void isComplete( bool complete );
    bool isComplete() const;

    //Element is a user content
    void isCustom( bool custom );
    bool isCustom() const;

    void CLASS_INVARIANT;

    PER_MEMBER_PERSISTENT_VIRTUAL( MachGuiDbElement );
    PER_FRIEND_READ_WRITE( MachGuiDbElement );

private:
    friend ostream& operator <<( ostream& o, const MachGuiDbElement& t );

    MachGuiDbElement( const MachGuiDbElement& );
    MachGuiDbElement& operator =( const MachGuiDbElement& );

    //data members
    MachGuiDbIElement* pData_; //The data implementation object
};

PER_DECLARE_PERSISTENT( MachGuiDbElement );


#endif

/* End DBELEMEN.HPP *************************************************/
