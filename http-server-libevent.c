#include "stdafx.h"
/*
* file: http_server.cpp
* description: Simple HTTP server
*
*/
// #include <errno.h>
// #include <event.h>
// #include <evhttp.h>
// #include <sys/socket.h>
// #include <sys/types.h>
// #include <signal.h>
// 
// #include <netinet/in.h>
// #include <arpa/inet.h>
// 
// #include <iostream>
// 
// const short  SERVER_BACKLOG = 128;
// const short  BUF_LEN = 26;
// const char   RESPONCE[]  = "<H1>Hello there</H1><BR/>";
// const char * SERVER_NAME = "Simple HTTP Server";
// 
// void on_request (struct evhttp_request *, void *);
// 
// int main (int argc, char **argv)
// {
//  if ( argc < 3 )
//  {
//   std::cout << "Start as:" << std::endl
//    << argv[0] << " host_address port" << std::endl;
//   return 1;
//  }
// 
//  int server_sock = socket (AF_INET, SOCK_STREAM, 0);
//  if ( server_sock == -1 )
//  {
//   std::cout << "Error socket(): " << strerror (errno) << std::endl;
//   return 1;
//  }
// 
//  u_short      port = atol (argv[2]);
//  const char * host = argv[1];
//  sockaddr_in  sa;
//  int          on = 1;
//  sa.sin_family = AF_INET;
//  sa.sin_port = htons (port);
//  sa.sin_addr.s_addr = inet_addr (host);
// 
//  if ( setsockopt (server_sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof (on)) == -1 )
//  {
//   std::cout << "Error setsockopt(): " << strerror (errno) << std::endl;
//   return 1;
//  }
// 
//  // Bind server socket to ip:port
//  if ( bind (server_sock, (const sockaddr*) &sa, sizeof (sa)) == -1 )
//  {
//   std::cout << "Error bind(): " << strerror (errno) << " on: " << host << ":" << port << std::endl;
//   return 1;
//  }
//  // Make server to listen
//  if ( listen (server_sock, SERVER_BACKLOG) == -1 )
//  {
//   std::cout << "Error listen(): " << strerror (errno) << std::endl;
//   return 1;
//  }
//  // Init events
//  event_base * serv_base = (event_base *) event_init ();
//  evhttp     * http_server = evhttp_new (serv_base);
// 
//  // Ignore SIGPIPE
//  signal (SIGPIPE, SIG_IGN);
// 
//  if ( evhttp_accept_socket (http_server, server_sock) == -1 )
//  {
//   std::cout << "Error evhttp_accept_socket(): " << strerror (errno) << std::endl;
//   return 1;
//  }
//  // Set HTTP request callback                               
//  evhttp_set_gencb (http_server, on_request, NULL);
//  // Dispatch events
//  event_base_dispatch (serv_base);
// 
//  return 0;
// }
// 
// void on_request (struct evhttp_request * req, void * arg)
// {
//  // Create responce buffer
//  struct evbuffer *evb = evbuffer_new ();
//  if ( !evb ) { return; }
// 
//  // Add heading text
//  evbuffer_add_printf (evb, "<HTML><HEAD><TITLE>%s Page</TITLE></HEAD><BODY>\n", SERVER_NAME);
//  // Add buffer
//  evbuffer_add (evb, RESPONCE, BUF_LEN);
//  // Add formatted text
//  evbuffer_add_printf (evb, "Your request is <B>%s</B> from <B>%s</B>.<BR/>Your user agent is '%s'\n",
//                       req->uri, req->remote_host, evhttp_find_header (req->input_headers, "User-Agent"));
//  // Add footer
//  evbuffer_add_printf (evb, "</BODY></HTML>");
// 
//  // Set HTTP headers
//  evhttp_add_header (req->output_headers, "Server", SERVER_NAME);
//  evhttp_add_header (req->output_headers, "Connection", "close");
// 
//  // Send reply
//  evhttp_send_reply (req, HTTP_OK, "OK", evb);
// 
//  // Free memory
//  evbuffer_free (evb);
// }
