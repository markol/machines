/*
 * D B E L E M E I . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachGuiDbIElement

    A data implementation object for MachGuiDbElement.
*/

#ifndef _MACHGUI_DBELEMEI_HPP
#define _MACHGUI_DBELEMEI_HPP

#include "base/base.hpp"
#include "base/persist.hpp"
#include "stdlib/string.hpp"
#include "ctl/vector.hpp"

//forward refs
class MachGuiDbElement;
class MachGuiDbTextData;

class MachGuiDbIElement
// Canonical form revoked
{
public:

    void CLASS_INVARIANT;

    PER_MEMBER_PERSISTENT_DEFAULT( MachGuiDbIElement );
    PER_FRIEND_READ_WRITE( MachGuiDbIElement );

private:
    friend class MachGuiDbElement;

    friend ostream& operator <<( ostream& o, const MachGuiDbIElement& t );

    MachGuiDbIElement();
    ~MachGuiDbIElement();
    MachGuiDbIElement( const MachGuiDbIElement& );
    MachGuiDbIElement& operator =( const MachGuiDbIElement& );

    //data members
    bool isComplete_; //True if the current player has completed this element
    bool isCustom_; //True if this element is user planet and not stored in binary database
    ctl_vector< MachGuiDbElement* > antecedents_; //Collection of elements that must be complete for this element
                                                  //to be undertaken
    uint menuStringId_; //Id of name string to be used in the menu selection list boxes
    string name_; //The name used in the definition file. Also used to generate eg scenario filename.
    string textDataFileName_; //relative path to the MachGuiDbTextData source file for this object
    string menuName_; // alternative for menuStringId_
    MachGuiDbTextData* pTextData_; //The text data object derived from above source file. Loaded when needed
};

PER_DECLARE_PERSISTENT( MachGuiDbIElement );


#endif

/* End DBELEMEI.HPP *************************************************/
