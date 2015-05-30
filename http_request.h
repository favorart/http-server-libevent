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

//#define HTTP_VERSION_LENGTH  5
typedef struct http_request http_req;
struct  http_request
{ http_type         type;          // v
  char              uri[FILENAME_MAX];
  //------------------
  struct evbuffer  *content;       // v
  file_ext          content_type;  // v
  //------------------
  // char           http_version[HTTP_VERSION_LENGTH];
  bool              file_exist;    // v
  //------------------
};

#define  HTTP_REQUEST_GET_SIMPLE   "GET /data/test.html HTTP/1.0\r\n\r\n"
#define  HTTP_RESPONSE_200         "HTTP/1.0 200 OK"        
#define  HTTP_RESPONSE_404         "HTTP/1.0 404 Not Found"
//-----------------------------------------
bool  http_request_init  (struct http_request *req);
bool  http_request_parse (struct http_request *req, struct evbuffer  *in_buf);
bool  http_reqhead_parse (struct http_request *req, struct evbuffer  *in_buf);
bool  http_response_make (struct http_request *req, struct evbuffer *out_buf);
void  http_request_clear (struct http_request *req);
void  http_request_free  (struct http_request *req);
//-----------------------------------------
#endif // _HTTP_REQ_H_