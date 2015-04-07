#include "stdafx.h"

#ifndef _HTTP_REQ_H_
#define _HTTP_REQ_H_
//-----------------------------------------
typedef enum { NONE, HTML, JPEG      } e_file_ex;
typedef enum { NONE, HEAD, GET, POST } http_type;
typedef struct http_request http_req;
struct  http_request
{
 e_file_ex  efile;
 http_type  type;
 char      *path;
 //------------------
 char      *content;
 uint32_t   content_size;
 //------------------
 bool       exist;
};

#define HTTP_RESPONSE_200  "HTTP/1.0 200 OK\r\n"                         \
                           "Server: http-server-pract3\r\n"                         
#define HTTP_RESPONSE_404  "HTTP/1.0 404 Not Found\r\n"                  \
                           "Server: http-server-pract3\r\n"              \
                           "Connection: close"

/* CMND URI HTTP/1.0\r\n
 * Header1: something\r\n
 * Header2: something\r\n
 * ...
 * HeaderN: something\r\n
 * \r\n
 * BODY html/image
 *
 */

// const char *typical_requst = "GET /path/to/file/index.html HTTP/1.0\r\n\r\n";
//-----------------------------------------
int   http_request_init  (http_req *req, ...);
int   http_request_parse (http_req *req, char *buffer);
void  http_request_free  (http_req *req);
//-----------------------------------------
#endif // _HTTP_REQ_H_