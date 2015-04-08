#include "stdafx.h"
#include "http_error.h"
#include "http_request.h"

//-----------------------------------------
const char*  strmyerror ()
{
  const char* strerr;

  switch ( my_errno )
  {
    default: strerr = NULL;                             break;
    case HTTP_ERR_PORT: strerr = "Incorrect port.";                break;
    case HTTP_ERR_PARAM: strerr = "Invalid function's parameter.";  break;
    case HTTP_ERR_INPUT: strerr = "Input incorrect user data.";     break;
    case HTTP_ERR_LIBEV: strerr = "Problem with libevent entity.";  break;
    case HTTP_ERR_RCMMN: strerr = "Incorrect request command.";     break;
  }

  my_errno = HTTP_ERR_NONE;
  return strerr;
}
//-----------------------------------------
int  http_request_init  (http_req *req, ...)
{
  memset (req, 0, sizeof (*req));
  return 0;
}
void http_request_free  (http_req *req)
{
 free (req->content);
 free (req->path);
 memset (req, 0, sizeof (*req));
}
//-----------------------------------------
int  http_request_parse (http_req *req, char *buffer, size_t buffer_length)
{
  int   result = 0;
  char *first_space, *second_space, *first_enter, *second_enter;
  //-----------------------------------------------------------------
  if ( !buffer )
  {
    my_errno = HTTP_ERR_PARAM;
    fprintf (stderr, "%s\n", strmyerror ());
    return 1;
  }
  //-----------------------------------------------------------------
  first_space = strchr (buffer, ' ');
  if ( !first_space || (first_space - buffer) > 4 )
  {
    my_errno = HTTP_ERR_RCMMN;
    fprintf (stderr, "%s\n", strmyerror ());
    return 1;
  }

  if ( !(second_space = strchr (first_space + 1, ' ')) )
  { my_errno = HTTP_ERR_INPUT;
    fprintf (stderr, "%s\n", strmyerror ());
    return 1;
  }

  if ( !( first_enter = strstr (second_space, "\r\n")) )
  { my_errno = HTTP_ERR_INPUT;
    fprintf (stderr, "%s\n", strmyerror ());
    return 1;
  }
  // TODO: check for exact string "HTTP/1.0"
  //-----------------------------------------------------------------
  char command[5] = { 0 };
  strncpy (command, buffer, first_space - buffer);

       if ( !strcmp (command, "HEAD") ) req->type = HTTP_REQ_HEAD;
  else if ( !strcmp (command, "GET" ) ) req->type = HTTP_REQ_GET;
  else if ( !strcmp (command, "POST") ) req->type = HTTP_REQ_POST;
  else
  { my_errno = HTTP_ERR_RCMMN;
    fprintf (stderr, "%s\n", strmyerror ());
    return 1;
  }
  //-----------------------------------------------------------------
  /* reseive the exact path to required file */
  req->path_size = (second_space - first_space);
  if ( !(req->path = (char*) calloc (req->path_size, sizeof (char))) )
  { fprintf (stderr, "%s\n", strerror (errno));
    result = 1;
    goto REQ_FREE;
  }
  strncpy (req->path, first_space + 1U, req->path_size - 1U);

  char *char_dot = NULL;
  if ( !(char_dot = strrchr (req->path_size, '.')) )
  { my_errno = HTTP_ERR_RCMMN;
    fprintf (stderr, "%s\n", strmyerror ());
    result = 1;
    goto REQ_FREE;
  }
  ++char_dot;

       if ( !strcmp (char_dot, "html")
         || !strcmp (char_dot, "htm")) req->file_type = HTTP_EXT_HTML;
  else if ( !strcmp (char_dot, "jpeg")
         || !strcmp (char_dot, "jpg")) req->file_type = HTTP_EXT_JPEG;
  else
  { my_errno = HTTP_ERR_INPUT;
    fprintf (stderr, "%s\n", strmyerror ());
    result = 1;
    goto REQ_FREE;
  }
  //-----------------------------------------------------------------
  // TODO: Read the Headers: Content-length, file-type
  //-----------------------------------------------------------------
  if ( !(second_enter  = strstr (first_space, "\r\n\r\n")) )
  { my_errno = HTTP_ERR_INPUT;
    fprintf (stderr, "%s\n", strmyerror ());
    result = 1;
    goto REQ_FREE;
  }
  else   second_enter += 4;

  req->content_size = ((buffer + buffer_length) - second_enter + 1U);
  /* read out the full content from the user request */
  if (   req->content_size && 
       !(req->content = (char *) calloc (req->content_size, sizeof (char))) )
  { fprintf (stderr, "%s\n", strerror (errno));
    result = 1;
    goto REQ_FREE;
  }
  strncpy (req->content, second_enter, req->content_size - 1U);

  /* Check for existence */
  req->file_exist = (access (req->path, 0) != -1);
  //-----------------------------------------------------------------
REQ_FREE:;
  if ( result )
    http_request_free (req);
  //-----------------------------------------------------------------
  return 0;
}
int  http_response_make (http_req *req, struct evbuffer *out_buf)
{
  if ( req->file_exist )
  {
    const char *content_type = (req->file_type == HTTP_EXT_HTML) ? "text/html; charset=utf-8" :
                               (req->file_type == HTTP_EXT_JPEG) ? "image/jpeg" : "";

    req->type = HTTP_RESPONSE;
    free (req->content);
    //----------------------------------------------------------------
    FILE *fd = fopen (req->path, "r");
    if ( !fd )
    { fprintf (stderr, "%s\n", strerror (errno));
      return 1;
    }
    fseek (fd, 0L, SEEK_SET);
    req->content_size = ftell (fd);
    //----------------------------------------------------------------
    if ( !(req->content = (char*) calloc (req->content_size, sizeof (char))) )
    { fprintf (stderr, "%s\n", strerror (errno));
      req->content_size = 0;
      fclose (fd);
      return 1;    
    }
    fread (req->content, sizeof (char), req->content_size, fd);
    //----------------------------------------------------------------
    fclose (fd);

    evbuffer_add_printf (out_buf, "%s\r\n"
                         "Content-Type: %s\r\n"
                         "Content-Length: %u\r\n"
                         "\r\n%s",
                         HTTP_RESPONSE_200,
                         content_type,
                         req->content_size,
                         req->content);
  }
  else
  {
    evbuffer_add_printf (out_buf, "%s\r\n", HTTP_RESPONSE_404);
  }

  return 0;
}
// #define MAXPATH	  255
// #define MAXBUF  	10000
// 
// int  http_request_parse (http_req *req, char *buffer)
// {
//   // char buff[MAXBUF];
//   if ( !buffer )
//   {
//     my_errno = HTTP_ERR_PARAM;
//     fprintf (stderr, "%s\n", strmyerror ());
//     return 1;
//   }
// 
//   /* Получить тип запроса */
//   char Req[MAXPATH];
//   if( 0 >= sscanf (buffer, "%s /", Req) )
//   {
//     my_errno = HTTP_ERR_INPUT;
//     // fprintf (stderr, "Incorrect request\n");
//     return 1;
//   }
//  
//        if ( !strcmp (Req, "HEAD") ) req->type = HEAD;
//   else if ( !strcmp (Req, "GET" ) ) req->type = GET;
//   else if ( !strcmp (Req, "POST") ) req->type = POST;
//   else
//   {
//     my_errno = HTTP_ERR_PARAM;
//     // fprintf (stderr, "Incorrect request\n");
//     return 1;
//   }
//  
//   if ( req->type == HEAD )
//   {
//   }
//   /* Если тип запроса - POST */
//   if ( req->type == POST )
//   {
//     /* Обработка пути */
//     char fname[MAXPATH];
//     char dirpath[MAXPATH];
//   
//     sscanf (buffer, "POST %s HTTP", fname);
//     getcwd (dirpath, MAXPATH);
//     /* Формирование пути к скрипту */
//     // const char *cmd = "sh " + dirpath + fname;
//     if ( strcmp (fname, "/script") == 0 )
//     {
//       /* Запуск скрипта */
//       // int rv = system ( cmd.c_str() );
//       /* Считать результат работы скрипта и вернуть клиенту */
//       // if (rv == 0)
//       {
//         // std::string strr;
//         // ifstream ifs("result.txt");
//         // if (ifs.is_open())
//         // {
//         //   while (!ifs.eof())
//         //   {
//         //     string line;
//         //     getline(ifs, line);
//         //     strr.append("<br>" + line + "\n");
//         //   }
//         //   ifs.close();
//         //   fwrite (strr.c_str(), strlen(strr.c_str()), 1, ClientFP);
//         // }
//       }
//     }
//      
//     // Считать содержимое страницы и вернуть клиенту
//     // std::string str;
//     // ifstream ifs("test.htm");
//     // if (ifs.is_open())
//     // {
//     //   while (!ifs.eof())
//     //   {
//     //     string line;
//     //     getline(ifs, line);
//     //     str.append(line + "\n");
//     //   }
//     //   ifs.close();
//     //   fwrite(str.c_str(), strlen(str.c_str()), 1, ClientFP);
//     // } 
//   }
//   /* Если тип запроса - GET */
//   else if ( req->type == GET )
//   {
//     fprintf (stdout /* ClientFP */,
//              "HTTP/1.1 200 OK\r\n"
//              "Server: webinfo\r\n"
//              "Content-Length: 0"
//              "Content-Type: text/html; charset=utf-8\r\n"
//              "Connection: close"
//              "\r\n"
//              "\r\n"
//              "<html>\n"
//              "<head>\n"
//              "<meta http-equiv='Content-Language' content='ru'>\n"
//              "<meta http-equiv='Content-Type' content='text/html; charset=UTF-8'>\n"
//              "<title>Web - Info</title>\n"
//              "</head>\n"
//              "<body>\n"
//              "<div align='center'>\n"
//              "<p><H1>Список процессов:</H1></p>\n" );
//     // char  buf[255];
//   
//     // FILE *fp = popen ("pstree -p","r");
//     // std::string rv, s;
//     // if ( !fp ) return NULL;
//     // for ( int i = 0; (i = fread (buf, 1, sizeof (buf), fp)); )
//     //   rv.append(buf, i);
//     // pclose (fp);
//     // s = "<textarea name='cmdwrite' rows='30' style='width:60%;'>" + rv + "</textarea>\n";
//     // fprintf (stdout /* ClientFP */, s.c_str());
//     fprintf (stdout /* ClientFP */,
//              "<form action='test.htm' name='file' method='post'>\n"
//              "<input type='submit' value='Открыть файл test.htm' title='' style='width:30%%;'>\n"
//              "</form>\n");
//     fprintf (stdout /* ClientFP */,
//              "<form action='script' name='scr' method='post'>\n"
//              "<input type='submit' value='Открыть результат работы скрипта' title='' style='width:30%%;'>\n"
//              "</form>\n");
//     fprintf (stdout /* ClientFP */,
//              "<form action='http://www.google.ru/search' method='get'  target='_blank'>\n"
//              "<input type='hidden' value='протокол http' name='q'>"
//              "<input type='submit' value='Отправить Google запрос на поиск' title='' style='width:30%%;'>\n"
//              "</form>\n");
//     fprintf (stdout /* ClientFP */,
//              "</div>\n"
//              "<body>\n"
//              "<html>\n");
//   }
//   
//   return 0;
// }
//-----------------------------------------
