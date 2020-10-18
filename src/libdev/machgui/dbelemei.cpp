/*
 * D B E L E M E I . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machgui/internal/dbelemei.hpp"
#include "machgui/dbelemen.hpp"

PER_DEFINE_PERSISTENT( MachGuiDbIElement );

MachGuiDbIElement::MachGuiDbIElement()
:   menuStringId_( 0 ),
    pTextData_( NULL )
{
    //Use sensible collection sizes
    antecedents_.reserve( 4 );

    TEST_INVARIANT;
}

MachGuiDbIElement::~MachGuiDbIElement()
{
    TEST_INVARIANT;

}

void MachGuiDbIElement::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachGuiDbIElement& t )
{

    o << "MachGuiDbIElement " << (void*)&t << " start" << std::endl;
    o << "MachGuiDbIElement " << (void*)&t << " end" << std::endl;

    return o;
}

void perWrite( PerOstream& ostr, const MachGuiDbIElement& ob )
{
    ostr << ob.antecedents_;
    ostr << ob.menuStringId_;
    ostr << ob.name_;
    ostr << ob.textDataFileName_;
}

void perRead( PerIstream& istr, MachGuiDbIElement& ob )
{
    istr >> ob.antecedents_;
    istr >> ob.menuStringId_;
    istr >> ob.name_;
    istr >> ob.textDataFileName_;

    ob.isComplete_ = false;
    ob.isCustom_ = false;
    ob.pTextData_ = NULL;
}

/* End DBELEMEI.CPP *************************************************/
