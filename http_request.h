#include "stdafx.h"

#ifndef _HTTP_REQ_H_
#define _HTTP_REQ_H_
//-----------------------------------------
/*  CMND URI HTTP/1.0\r\n
 *  Header1: something\r\n
 *  Header2: something\r\n
 *  ...
 *  HeaderN: something\r\n
 *  \r\n
 *  BODY html/jpeg
 */

typedef enum { HTTP_REQ_NONE, HTTP_REQ_HEAD, HTTP_REQ_GET,
               HTTP_REQ_POST, HTTP_RESPONSE  /* , ... */   } http_type;
typedef enum { HTTP_EXT_NONE, HTTP_EXT_HTML, HTTP_EXT_JPEG } file_ext;

typedef struct http_request http_req;
struct  http_request
{
 http_type  type;
 char      *path;
 //------------------
 char      *content;
 uint32_t   content_size;
 //------------------
 bool       file_exist;
 file_ext   file_type;
};

#define HTTP_REQUEST_GET_SIMPLE  "GET /data/test.html HTTP/1.0\r\n\r\n"

#define HTTP_RESPONSE_200        "HTTP/1.0 200 OK\r\n"            \
                                 "Server: http-server-pract3"            
#define HTTP_RESPONSE_404        "HTTP/1.0 404 Not Found\r\n"     \
                                 "Server: http-server-pract3\r\n" \
                                 "Connection: close"
//-----------------------------------------
int   http_request_init  (http_req *req, ...);
int   http_request_parse (http_req *req, char *buffer, size_t buffer_length);
void  http_request_free  (http_req *req);
//-----------------------------------------
int   http_response_make (http_req *req, struct evbuffer *out_buf);
//-----------------------------------------
#endif // _HTTP_REQ_H_