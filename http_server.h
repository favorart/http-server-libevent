#include "stdafx.h"

#ifndef _HTTP_SERVER_H_
#define _HTTP_SERVER_H_
/*
 *  +   Создать HTTP-сервер.
 *  v   Сборка через make.
 *  v   Запуск:  ./wwwd -d <dir> -h <ip> -p <port>
 *  -   Реализация HEAD/GET/POST.
 *  v   Статусы 200 и 404.
 *  +   В каталоге <dir> - html и jpeg файлы.
 *
 *  8 апреля (Коллоквиум)
 */
//-----------------------------------------
#define STR_IP_LEN 16
typedef struct http_server_config srv_conf;
struct  http_server_config
{
  uint16_t  port;
  char      ip[STR_IP_LEN];
  char     *path;
 };
//-----------------------------------------
int   http_server_config_init  (srv_conf *conf, char *path, char *port, char *ip);
void  http_server_config_print (srv_conf *conf, FILE *stream);
void  http_server_config_free  (srv_conf *conf);
//-----------------------------------------
int   parse_console_parameters (int argc, char **argv, srv_conf *conf);
//-----------------------------------------
int   set_nonblock (int fd);
//-----------------------------------------
struct client
{
  // static const uint32_t MAX_LIMIT = 1025;
  struct event ev_read;

  struct evbuffer *buffer;
  /* bufferevent alreadt has separate
   * two buffers for input and output
   */
  struct bufferevent *b_ev;

  http_req request;
};

void  on_accept (evutil_socket_t fd, short ev, void *arg);
void  on_read   (evutil_socket_t fd, short ev, void *arg);
void  on_write  (evutil_socket_t fd, short ev, void *arg);
//-----------------------------------------
void  http_ac_err_cb (struct evconnlistener *listener, void *arg);
void  http_accept_cb (struct evconnlistener *listener, evutil_socket_t fd,
                      struct sockaddr *address, int socklen, void *arg);
//-----------------------------------------
void  http_read_cb   (struct bufferevent *b_ev, short events, void *arg);
void  http_write_cb  (struct bufferevent *b_ev, short events, void *arg);
void  http_error_cb  (struct bufferevent *b_ev, short events, void *arg);
//-----------------------------------------
#endif // _HTTP_SERVER_H_
