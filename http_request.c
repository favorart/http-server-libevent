#include "stdafx.h"
#include "http_error.h"
#include "http_server.h"
#include "http_request.h"

//-----------------------------------------
const char*  strmyerror ()
{
  const char* strerr;

  switch ( my_errno )
  { default: strerr = NULL;                                      break;

    case HTTP_ERR_PARAM : strerr = "Invalid function argument."; break;
    case HTTP_ERR_INPUT : strerr = "Input incorrect user data."; break;
    case HTTP_ERR_LIBEV : strerr = "Incorrect libevent entity."; break;
    case HTTP_ERR_RCMMN : strerr = "Incorrect request command."; break;
    case HTTP_ERR_RFREE : strerr = "Dispatched a free request."; break;
    case HTTP_ERR_FDTRS : strerr = "Failure in transmiting fd."; break;
  }

  my_errno = HTTP_ERR_NONE;
  return strerr;
}
//-----------------------------------------
bool  http_request_init  (struct http_request *req)
{
  memset (req, 0, sizeof (*req));
  //-----------------------------------
  return ((req->content = evbuffer_new ()) == NULL);
}
void  http_request_free  (struct http_request *req)
{
  if( req->content )
    evbuffer_free (req->content);
  memset (req, 0, sizeof (*req));
}
//-----------------------------------------
void  http_request_clear (struct http_request *req)
{
  evbuffer_drain (req->content, -1);
  //-----------------------------------
  req->type = HTTP_REQ_NONE;
  req->file_exist = false;
  req->content_type = HTTP_EXT_NONE;
  memset (req->uri, 0, FILENAME_MAX);
}
bool  http_request_parse (struct http_request *req, struct evbuffer  *in_buf)
{
  const char*  HTTP_HEAD_TERM = "\r\n\r\n";
  const size_t HTTP_HEAD_TERM_LENGTH = sizeof (HTTP_HEAD_TERM);
  //-----------------------------------------------------------------  
  size_t  buffer_size = evbuffer_get_length (in_buf);
  if ( !buffer_size )
  { my_errno = HTTP_ERR_PARAM;
    fprintf (stderr, "%s\n", strmyerror ());
    return -1;
  }
  //-----------------------------------------------------------------  
  struct evbuffer_ptr  head_closer;
  head_closer = evbuffer_search (in_buf, HTTP_HEAD_TERM, HTTP_HEAD_TERM_LENGTH, NULL);
  if ( head_closer.pos == -1 )
  { my_errno = HTTP_ERR_INPUT;
    fprintf (stderr, "%s\n", strmyerror ());
    evbuffer_drain (in_buf, -1);
    return -1;
  }
  else if ( !head_closer.pos )
  { evbuffer_drain (in_buf, HTTP_HEAD_TERM_LENGTH);
    return 0;
  }
  // evbuffer_ptr_set (buffer, &head_closer, HTTP_HEAD_CLOSER_LENGTH, EVBUFFER_PTR_ADD);

  struct evbuffer *header = evbuffer_new ();
  evbuffer_remove_buffer (in_buf, header, head_closer.pos);
  evbuffer_drain         (in_buf, HTTP_HEAD_TERM_LENGTH);
  buffer_size -= (head_closer.pos + HTTP_HEAD_TERM_LENGTH);

  bool  fail = http_reqhead_parse (req, header);
  evbuffer_free (header);

  // if ( req->content ) evbuffer_free (req->content);
  // req->content = buffer;
  // evbuffer_remove_buffer (in_buf, req->content, buffer_size);
  //-----------------------------------------------------------------
  return fail;
}
bool  http_reqhead_parse (struct http_request *req, struct evbuffer  *in_buf)
{
  bool fail = false;
  //-----------------------------------------------------------------
#ifdef _DEBUG
  int index = 0;
#endif // _DEBUG

  const char   *http_command_strs[] = { "HEAD", "GET", "POST" };
  http_type     http_command_enum[] = { HTTP_REQ_HEAD, HTTP_REQ_GET, HTTP_REQ_POST };
  const size_t  http_command_length = sizeof (http_command_enum) / sizeof (*http_command_enum);

  const char   *http_version        = "HTTP/1.1";
  const size_t  http_version_length = sizeof (http_version) / sizeof (*http_version);
  //-----------------------------------------------------------------
  struct evbuffer_ptr  req_type, req_version;
  for ( int i = 0; i < http_command_length; ++i )
  {
    req_type = evbuffer_search (in_buf, http_command_strs[i],
                                strlen (http_command_strs[i]), NULL);
    if ( req_type.pos != -1 )
    {
#ifdef _DEBUG
      index = i;
#endif // _DEBUG
      req->type = http_command_enum[i];
      evbuffer_drain (in_buf, strlen (http_command_strs[i]) + req_type.pos);
      break;
    }
  }
  if ( req_type.pos == -1 )
  {
    my_errno = HTTP_ERR_RCMMN;
    fprintf (stderr, "%s\n", strmyerror ());
    fail = true;
    goto REQ_FREE;
  }

  // evbuffer_readln (in_buf)
  req_version = evbuffer_search (in_buf, http_version, http_version_length, NULL);
  if ( req_version.pos == -1 )
  {
    my_errno = HTTP_ERR_RCMMN;
    fprintf (stderr, "%s\n", strmyerror ());
    fail = true;
    goto REQ_FREE;
  }

  char uri[FILENAME_MAX] = { 0 };
  evbuffer_copyout (in_buf, uri, req_version.pos - req_type.pos);
  //-----------------------------------------------------------------
  /* reseive the exact path to required file */
  strcpy (req->uri, server_conf.folder_path);

  char *uri_bgn = uri;
  char *uri_end = uri + strlen (uri) - 1;

  while ( isspace (*uri_end) )
  {
     *uri_end = '\0';
    --uri_end;
  }

  while ( isspace (*uri_bgn) )
    ++uri_bgn;
  if ( *uri_bgn == '/' )
    ++uri_bgn;
  
  if ( req->uri[strlen (req->uri) - 1] != '/' )
    req->uri[strlen (req->uri)] = '/';
  
  strcat (req->uri, uri_bgn);
  //-----------------------------------------------------------------
  evbuffer_drain (in_buf, req_version.pos + http_version_length);
  //-----------------------------------------------------------------
  if ( req->type == HTTP_REQ_POST )
  {
    req->file_exist = false;
  }
  else
  {
    char *char_dot = NULL;
    if ( !(char_dot = strrchr (req->uri, '.')) )
    {
      my_errno = HTTP_ERR_RCMMN;
      fprintf (stderr, "%s\n", strmyerror ());
      fail = true;
      goto REQ_FREE;
    }
    ++char_dot;

         if ( strstr (req->uri, ".html")
           || strstr (req->uri, ".htm") ) req->content_type = HTTP_EXT_HTML;
    else if ( strstr (req->uri, ".jpeg")
           || strstr (req->uri, ".jpg") ) req->content_type = HTTP_EXT_JPEG;
    else
    {
      my_errno = HTTP_ERR_INPUT;
      fprintf (stderr, "%s\n", strmyerror ());
      fail = true;
      goto REQ_FREE;
    }

    /* Check for existence */
    struct stat sb;
    if ( stat (req->uri, &sb) == 0 )
    {
      req->file_exist = true;
    }
  }
  //-----------------------------------------------------------------
  // TODO: Read the Headers: Content-length, file-type  
  //-----------------------------------------------------------------
#ifdef _DEBUG
  printf ("\nREQUEST: %s\nFilepath: %s\n\n", http_command_strs[index], req->uri);
#endif // _DEBUG

REQ_FREE:;
  if ( fail )
    http_request_free (req);
  //-----------------------------------------------------------------
  return fail;
}
bool  http_response_make (struct http_request *req, struct evbuffer *out_buf)
{
  if ( req->type == HTTP_REQ_NONE || req->type == HTTP_RESPONSE )
    return false;
  //----------------------------------------------------------------
  const char connection[] = "close";
  char response_time[30];

  time_t timev;
  time (&timev);
  struct tm  *timeinfo = localtime (&timev);
  strftime (response_time, 30, "%d-%m-%Y %I:%M:%S", timeinfo);
  //----------------------------------------------------------------
  if (  req->type == HTTP_REQ_POST )
  {
#ifdef _DEBUG
    printf ("\nRESPONSE: %s\n"
            "Server: %s\n"
            "Time: %s\n",
            HTTP_RESPONSE_200,
            server_conf.ptr_server_name,
            response_time);
#endif // _DEBUG
    //----------------------------------------------------------------
    evbuffer_add_printf (out_buf, "%s\r\n"
                         "Server: %s\r\n"
                         "Time: %s\r\n"
                         "Connection: %s\r\n"
                         "\r\n",
                         HTTP_RESPONSE_200,
                         server_conf.ptr_server_name,
                         response_time,
                         connection);
  
  }
  else if ( (req->type == HTTP_REQ_HEAD || req->type == HTTP_REQ_GET) && req->file_exist )
  {
    //----------------------------------------------------------------
    const char *content_type = (req->content_type == HTTP_EXT_HTML) ? "text/html; charset=utf-8" :
                               (req->content_type == HTTP_EXT_JPEG) ? "image/jpg" : "";
    //----------------------------------------------------------------
    int fd = open (req->uri, O_RDONLY);
    if ( fd < 0 )
    {
      fprintf (stderr, "%s\n", strerror (errno));
      // error (1, errno, "open");
      return 1;
    }    

    struct stat sb;
    fstat (fd, &sb);

    size_t content_len = sb.st_size;
    //----------------------------------------------------------------
    evbuffer_add_printf (out_buf, "%s\r\n"
                         "Server: %s\r\n"
                         "Time: %s\r\n"
                         "Content-Type: %s\r\n"
                         "Content-Length: %u\r\n"
                         "Connection: %s\r\n"
                         "\r\n",
                         HTTP_RESPONSE_200,
                         server_conf.ptr_server_name,
                         response_time,
                         content_type,
                         content_len,
                         connection);

    if ( req->type == HTTP_REQ_GET )
    {
      evbuffer_add_file (out_buf, fd, 0, content_len);
      /* The file descriptor will be closed after the data
       * is flushed from the evbuffer, or when the evbuffer
       * is freed. 
       */
      // close (fd);
    }
    else
    {
      close (fd);
    }
    //----------------------------------------------------------------
    // assert (!evbuffer_add_file (out_buf, fd, 0, stat_.st_size));
 
#ifdef _DEBUG
    printf ("\nRESPONSE: %s\n"
            "Server: %s\n"
            "Time: %s\n"
            "Content-Type: %s\n"
            "Content-Length: %u\n",
            HTTP_RESPONSE_200,
            server_conf.ptr_server_name,
            response_time,
            content_type,
            content_len);
#endif // _DEBUG
  }
  else
  {
    evbuffer_add_printf (out_buf, "%s\r\n"
                         "Server: %s\r\n"
                         "Time: %s\r\n"
                         "Connection: %s\r\n"
                         "\r\n",
                         HTTP_RESPONSE_404,
                         server_conf.ptr_server_name,
                         response_time,
                         connection);
  }
  //----------------------------------------------------------------
  http_request_clear (req);
  return false;
}
//-----------------------------------------
