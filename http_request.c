#include "stdafx.h"
#include "http_error.h"
#include "http_request.h"

//-----------------------------------------
int  http_request_init (http_req *req, ...)
{
 memset (req, 0, sizeof (*req))
 return 0;
}
void http_request_free (http_req *req)
{
 free (req->host);
 req->type = NONE;
 req->host = NULL;
 req->filename = NULL;
}
//-----------------------------------------
int http_request_parse (http_req *req, char *buffer)
{
  int firstSpace, secondSpace, firstEnter, secondEnter, end;

  if ( !buffer )
  {
    my_errno = HTTP_ERR_PARAM;
    fprintf (stderr, "%s\n", strmyerror ());
    return 1;
  }

  firstSpace = strstr (buffer, " ");
  if ( firstSpace == 0 || firstSpace > 4 )
  {
    my_errno = HTTP_ERR_INPUT;
    fprintf (stderr, "%s\n", strmyerror ());
    return 1;
  }

  secondSpace = strstr (buffer + firstSpace, " ");
  if ( secondSpace == 0 )
  {
    my_errno = HTTP_ERR_INPUT;
    fprintf (stderr, "%s\n", strmyerror ());
    return 1;
  }

  firstEnter = strstr (buffer + secondSpace, "\r\n");
  if ( secondSpace == 0 )
  {
    my_errno = HTTP_ERR_INPUT;
    fprintf (stderr, "%s\n", strmyerror ());
    return 1;
  }

  char requastion[5];
  strncpy (requastrion, buffer, firstSpace + 1);
  malloc (requastrion, 0, sizeof (char));
  if ( !strcmp (Req, "HEAD") )
  {
    req->type = HEAD;
  }
  else if ( !strcmp (Req, "GET") )
  {
    req->type = GET;
  }
  else if ( !strcmp (Req, "POST") )
  {
    req->type = POST;
  }
  else
  {
    my_errno = HTTP_ERR_PARAM;
    // fprintf (stderr, "Incorrect request\n");
    return 1;
  }

  req->path = (char *) calloc (firstEnter - secondSpace, sizeof (char));
  strncpy (req->path, buffer + firstEnter + 1, firstSpace - secondSpace - 1);

  if ( req->type == "GET" )
  {
    return 0;
  }

  secondSpace = strstr (buffer + secondSpace, "\r\n\r\n");
  if ( secondEneter == 0 )
  {
    my_errno = HTTP_ERR_INPUT;
    fprintf (stderr, "%s\n", strmyerror ());
    return 1;
  }

  secondEnter += 4;
  end = secondEnter;
  while ( buffer[end] )
  {
    ++end;
  }
  req->content = (char *) calloc (end - secondEnter + 1, sizeof (char));
  strncpy (req->path, buffer + secondEnter, end - secondEnter);
}
[01:04 : 35] Роман Холин : typedef enum { NONE, HEAD, GET, POST } http_type;
typedef struct http_request http_req;

struct  http_request
{
  http_type type;
  size_t pathSize;
  char      *path;
  size_t contentSize;
  char      *content;
};

int http_request_parse (http_req *req, char *buffer)
{
  int firstSpace, secondSpace, firstEnter, secondEnter, end;

  if ( !buffer )
  {
    my_errno = HTTP_ERR_PARAM;
    fprintf (stderr, "%s\n", strmyerror ());
    return 1;
  }

  firstSpace = strstr (buffer, " ");
  if ( firstSpace == 0 || firstSpace > 4 )
  {
    my_errno = HTTP_ERR_INPUT;
    fprintf (stderr, "%s\n", strmyerror ());
    return 1;
  }

  secondSpace = strstr (buffer + firstSpace, " ");
  if ( secondSpace == 0 )
  {
    my_errno = HTTP_ERR_INPUT;
    fprintf (stderr, "%s\n", strmyerror ());
    return 1;
  }

  firstEnter = strstr (buffer + secondSpace, "\r\n");
  if ( secondSpace == 0 )
  {
    my_errno = HTTP_ERR_INPUT;
    fprintf (stderr, "%s\n", strmyerror ());
    return 1;
  }

  char requastion[5];
  strncpy (requastrion, buffer, firstSpace + 1);
  malloc (requastrion, 0, sizeof (char));
  if ( !strcmp (Req, "HEAD") )
  {
    req->type = HEAD;
  }
  else if ( !strcmp (Req, "GET") )
  {
    req->type = GET;
  }
  else if ( !strcmp (Req, "POST") )
  {
    req->type = POST;
  }
  else
  {
    my_errno = HTTP_ERR_PARAM;
    // fprintf (stderr, "Incorrect request\n");
    return 1;
  }

  req->path = (char *) calloc (firstEnter - secondSpace, sizeof (char));
  strncpy (req->path, buffer + firstEnter + 1, firstSpace - secondSpace - 1);
  req->pathSize = firstSpace - secondSpace - 1;

  if ( req->type == "GET" )
  {
    req->content = 0;
    req->contentSize = 0;
    return 0;
  }

  secondSpace = strstr (buffer + secondSpace, "\r\n\r\n");
  if ( secondEneter == 0 )
  {
    my_errno = HTTP_ERR_INPUT;
    fprintf (stderr, "%s\n", strmyerror ());
    return 1;
  }

  secondEnter += 4;
  end = secondEnter;
  while ( buffer[end] )
  {
    ++end;
  }
  req->content = (char *) calloc (end - secondEnter + 1, sizeof (char));
  strncpy (req->path, buffer + secondEnter, end - secondEnter);
  req->contentSize = end - secondEnter;
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
