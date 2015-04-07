#include "stdafx.h"

#ifndef _HTTP_ERR_H_
#define _HTTP_ERR_H_
//-----------------------------------------
typedef enum { HTTP_ERR_NONE,  HTTP_ERR_PORT,  HTTP_ERR_PARAM,
               HTTP_ERR_INPUT, HTTP_ERR_LIBEV, HTTP_ERR_REQ  /* ,  ...  */   } myerr;
myerr my_errno;

static const char*  strmyerror ()
{
  const char* strerr;
  
  switch ( my_errno )
  {
   default             : strerr = NULL;                             break;
   case HTTP_ERR_PORT  : strerr = "Incorrect port.";                break;
   case HTTP_ERR_PARAM : strerr = "Invalid function's parameter.";  break;
   case HTTP_ERR_INPUT : strerr = "Input incorrect user data.";     break;
   case HTTP_ERR_LIBEV : strerr = "Problem with libevent entity.";  break;
   case HTTP_ERR_LIBEV: strerr = ""; break;
  }
  
  my_errno = HTTP_ERR_NONE;
  return strerr;
}
//-----------------------------------------
#endif // _HTTP_ERR_H_
