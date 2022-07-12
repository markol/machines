/*
 * G X E R R O R . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */
#ifndef _GXERROR_HPP
#define _GXERROR_HPP

#include"base/base.hpp"

typedef enum
{
  NO_ERROR_AGT,         // 0
  NO_TEXTURE_AGT,         // 0
  FILE_OPEN_AGT,
  HEADER_NAME_AGT,
  HEADER_VERSION_AGT,
  LABEL_AGT,
  MEMORY_AGT,           // 5
  LINE_AGT,
  COMMENT_AGT,
  TOKEN_AGT,
  FLOAT_AGT,
  COUNT_AGT,            // 10
  DUP_AGID_AGT,
  INV_AGID_AGT,
  LABEL_TOOLONG_AGT,
  MATRIX_ROW_AGT,
  STR_LENGTH_AGT,       // 15
  NT_NAME_AGT,          // not terminated name
  INV_NAME_AGT,
              // internal errors
  END_AGT,
  UV_ABSENT_AGT,
  TEXTURE_END_AGT,
  TRANSFORM_ABSENT_AGT,
  FAIL_AGT,             // 20
  UNKNOWN_AGT

} GXError;    // internal errors are not reported

// Error codes are inherited from those of Gamut-X

/*-------------------------------------------------------------------------------**
** class GXError {                                                               **
** public:                                                                       **
**  // Orthodox canonical form                                                   **
**         GXError();                                                      **
**         ~GXError();                                                     **
**         GXError( const GXError& copy);                                        **
**         GXError& operator=(const GXError& copy);                              **
**         GXError& operator=(const GXErrorCode& err);                           **
**         friend bool operator==(const GXError& err1, const GXError& err2);     **
**         friend bool operator==(const GXError& err1, const GXErrorCode& err2); **
**         friend bool operator==(const GXErrorCode& err1, const GXError& err2); **
**         friend bool operator!=(const GXError& err1, const GXError& err2);     **
**         friend bool operator!=(const GXError& err1, const GXErrorCode& err2); **
**         friend bool operator!=(const GXErrorCode& err1, const GXError& err2); **
**                                                                               **
** 		const GXErrorCode& operator() const {                                    **
** 		  return val_;                                                           **
** 		}                                                                        **
** 		GXErrorCode& operator() {                                                **
** 		  return val_;                                                           **
** 		}                                                                        **
**                                                                               **
**     void CLASS_INVARIANT;                                                     **
**                                                                               **
** private:                                                                      **
**                                                                               **
**   GXErrorCode val_;                                                           **
**                                                                               **
** };                                                                            **
**-------------------------------------------------------------------------------*/

#endif

