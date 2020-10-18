/*
 * G X E R R O R . C P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "gxin/GXError.hpp"

GXError::GXError() : val_(NO_ERROR_AGT)
{

    TEST_INVARIANT;
}

GXError::GXError(const GXError& copy) : val_(copy.val_)
{

    TEST_INVARIANT;
}

GXError& GXError::operator =(const GXErrorCode& copy)
{
   TEST_INVARIANT;
   val_=copy;
   TEST_INVARIANT;
   return *this;
}

GXError& GXError::operator =(const GXError& copy)
{
        TEST_INVARIANT;

        if (this!=&copy) {
                val_=copy.val_;
        }

    TEST_INVARIANT;
        return *this;
}

bool operator==(const GXError& err1, const GXError& err2) {
        bool result=true;
        if (err1.val_!=err2.val_) result=false;
        return result;
}

bool operator==(const GXError& err1, const GXErrorCode& err2) {
        bool result=true;
        if (err1.val_!=err2) result=false;

        return result;
}

bool operator==(const GXErrorCode& err1, const GXError& err2) {
        bool result=true;
        if (err1!=err2.val_) result=false;
        return result;
}

bool operator!=(const GXError& err1, const GXError& err2) {
        bool result=true;
        if (err1.val_==err2.val_) result=false;
        return result;
}

bool operator!=(const GXError& err1, const GXErrorCode& err2) {
        bool result=true;
        if (err1.val_==err2) result=false;

        return result;
}

bool operator!=(const GXErrorCode& err1, const GXError& err2) {
        bool result=true;
        if (err1==err2.val_) result=false;
        return result;
}

GXError::~GXError()
{
    TEST_INVARIANT;

}

void GXError::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const GXError& t )
{

    o << "GXError " << (void*)&t << " start" << endl;
    o << "GXError " << (void*)&t << " end" << endl;

    return o;
}

/* End GXERROR.CPP **************************************************/
