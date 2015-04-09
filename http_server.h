#include "stdafx.h"
#include "http_request.h"

#ifndef _HTTP_SERVER_H_
#define _HTTP_SERVER_H_
/*
 *  v   Создать HTTP-сервер.
 *  v   Сборка через make.
 *  v   Запуск:  ./wwwd -d <dir> -h <ip> -p <port>
 *  v   Реализация HEAD/GET/POST.
 *  v   Статусы 200 и 404
 *  v   В каталоге <dir> - html и jpeg файлы
 *
 *  8 апреля (Коллоквиум)
 */
//-----------------------------------------
#define  HTTP_SERVER_NAME    "http-server-pract#3"
#define  HTTP_IP_STR_LENGTH  16

typedef struct http_server_config  srv_conf;
struct  http_server_config
{
  uint16_t  port;
  char      ip[HTTP_IP_STR_LENGTH];
  //-----------------------
  struct evbuffer *folder_path;
  struct evbuffer *server_path;
  //-----------------------
  // struct evbuffer *buffer_temp;
  char *server_name;
  //-----------------------
 };
struct http_server_config  server_conf = { 0 };
//-----------------------------------------
int   http_server_config_init  (srv_conf *conf, char *path, char *port, char *ip);
void  http_server_config_print (srv_conf *conf, FILE *stream);
void  http_server_config_free  (srv_conf *conf);
//-----------------------------------------
int   parse_console_parameters (int argc, char **argv, srv_conf *conf);
//-----------------------------------------
int   set_nonblock (evutil_socket_t fd);
//-----------------------------------------
struct client
{
  /* bufferevent already has separate
   * two buffers for input and output.
   */
  struct bufferevent     *b_ev;
  struct event_base      *base;
  //---------------------------
  struct evbuffer      *buffer;
  //---------------------------
  struct http_request  request;
  struct http_request  response;
};
//-----------------------------------------
// void  http_ac_err_cb (struct evconnlistener *listener, void *arg);
// void  http_accept_cb (struct evconnlistener *listener, evutil_socket_t fd,
//                       struct sockaddr *address, int socklen, void *arg);
void  http_accept_cb (evutil_socket_t fd, short ev, void *arg);
void  http_ac_err_cb (evutil_socket_t fd, short ev, void *arg);
//-----------------------------------------
void  http_read_cb   (struct bufferevent *b_ev, void *arg);
void  http_write_cb  (struct bufferevent *b_ev, void *arg);
void  http_error_cb  (struct bufferevent *b_ev, short events, void *arg);
//-----------------------------------------
#endif // _HTTP_SERVER_H_
