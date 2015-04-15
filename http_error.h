#include "stdafx.h"

#ifndef _HTTP_ERR_H_
#define _HTTP_ERR_H_

#define _DEBUG
//-----------------------------------------
typedef enum { HTTP_ERR_NONE,  HTTP_ERR_NPORT, HTTP_ERR_PARAM,
               HTTP_ERR_INPUT, HTTP_ERR_LIBEV, HTTP_ERR_RCMMN } myerr;
myerr my_errno;
const char*  strmyerror ();
//-----------------------------------------
#endif // _HTTP_ERR_H_
