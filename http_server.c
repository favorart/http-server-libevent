#include "stdafx.h"
#include "http_error.h"
#include "http_server.h"
#include "http_request.h"

//-----------------------------------------
void  on_read   (evutil_socket_t fd, short ev, void *arg)
{
 struct client  *Client = (struct client*) arg;

 static char Buffer[1024];
 int  recv_size = recv (fd, Buffer, 1024, MSG_NOSIGNAL);
 if ( recv_size <= 0 )
 {
   shutdown (fd, SHUT_RDWR);
   close    (fd);

   event_del (&(Client->ev_read));
   free      (  Client);
 }

 send (fd, Buffer, recv_size, MSG_NOSIGNAL);
}
//-----------------------------------------
void  on_accept (evutil_socket_t fd, short ev, void *arg)
{
 int  SlaveSocket = accept (fd, 0, 0);
 if ( SlaveSocket == -1 )
 {
   fprintf (stderr, "%s\n", strerror (errno));
   return;
 }

 set_nonblock (SlaveSocket);

 struct client *Client = (struct client*) calloc (1U, sizeof (*Client));
 if ( !Client )
 {
   fprintf (stderr, "%s\n", strerror (errno));
   
   shutdown (SlaveSocket, SHUT_RDWR);
   close    (SlaveSocket);
   return;
 }

 event_set (&Client->ev_read, SlaveSocket, EV_READ | EV_PERSIST, on_read, Client);
 event_add (&Client->ev_read, NULL);
}
//-----------------------------------------
/*   Для того, чтобы начать работать с буферизированным вводом/выводом необходимо:
 *   —  инициализировать структуру bufferevent вызовом bufferevent_new (),
 *      указав функции для обратного вызова на события
 *      чтения (необязательно), записи (необязательно) и ошибки (обязательно);
 *   —  Привязать это событие к локальной событийной базе (event_base) вызовом bufferevent_base_set();
 *   —  Включать или выключать обработчики чтения/записи вызовами bufferevent_enable ()/bufferevent_disable ();
 *   —  Читать и писать вызовами bufferevent_read () и bufferevent_write () соответственно.
 *   Стоит отметить, что обработчик чтения будет вызван после того, как в буфере накопится весь пакет
 *   или закроется подключение, а обработчик записи — когда отправляемый буфер отправлен либо полностью,
 *   либо до указанной отметки (watermark).
 */

void  http_ac_err_cb (struct evconnlistener *listener, void *arg)
{
  struct event_base *base = evconnlistener_get_base (listener);
  int err = EVUTIL_SOCKET_ERROR ();
  fprintf (stderr, "Got an error %d (%s) on the listener. "
           "Shutting down.\n", err, evutil_socket_error_to_string (err));

  event_base_loopexit (base, NULL);
}
/* Connecting handling */
void  http_accept_cb (struct evconnlistener *listener, evutil_socket_t fd,
                      struct sockaddr *address, int socklen, void *arg)
{
  /* A new connection! Set up a bufferevent for it */
  struct event_base *base = evconnlistener_get_base (listener);
  // !!!

  /* Create new bufferized event, linked with client's socket */
  struct bufferevent *b_ev = bufferevent_socket_new (base, fd, BEV_OPT_CLOSE_ON_FREE);
  bufferevent_setcb  (b_ev,  http_read_cb, http_write_cb, http_error_cb, arg);
  /* Ready to get data */
  bufferevent_enable (b_ev,  EV_READ | EV_WRITE | EV_PERSIST);
}
//-----------------------------------------
void  http_read_cb  (struct bufferevent *b_ev, short events, void *arg)
{
  /* This callback is invoked when there is data to read on b_ev_read */
     struct evbuffer  * input_buf = bufferevent_get_input  (arg);
  // struct evbuffer  *output_buf = bufferevent_get_output (arg);
  if ( !input_buf )
  {
    fprintf (stderr, "%s\n", strerror (errno));
    return;
  }

  size_t len = (evbuffer_get_length (input_buf) + 1U);
  if ( !len )
    return;

  char *data = (char*) malloc (sizeof (char) * len);
  if( !data )
  {
    fprintf (stderr, "%s\n",strerror (errno));
    return;
  }

  evbuffer_copyout (input_buf, data, len);
  printf ("we got some data: %s\n", data);
  free (data);

  /* Copy all the data from the input buffer to the output buffer. */
  // evbuffer_add_buffer (output, input);
}
void  http_write_cb (struct bufferevent *b_ev, short events, void *arg)
{
  struct client *Client = (struct client*) arg;
  
  bool exist = 0;
  if ( exist )
  {
    const char *content_type = (CLient->efile == HTML) ?  "text/html; charset=utf-8" : 
                               (CLient->efile == JPEG) ? "image/jpeg" : "";
    const char *connection = "close";

    evbuffer_add_printf (bufferevent_get_output (b_ev), "%s\r\n"
                         "Content-Type: %s\r\n"
                         "Content-Length: %u\r\n"
                         "Connection: %s\r\n"
                         "\r\n%s",
                         HTTP_RESPONSE_200,
                         content_type,
                         Client->request->content_size,
                         connection,
                         Client->request->content );
  }
  else
  {
    evbuffer_add_printf (bufferevent_get_output (b_ev), "%s\r\n", HTTP_RESPONSE_404);
  }
}
void  http_error_cb (struct bufferevent *b_ev, short events, void *arg)
{
  if ( events & BEV_EVENT_ERROR )
    fprintf (stderr, "Error from bufferevent: '%s'\n", evutil_socket_error_to_string (EVUTIL_SOCKET_ERROR ()));
  if ( events & (BEV_EVENT_EOF | BEV_EVENT_ERROR) )
  { bufferevent_free (b_ev_error); }
}
//-----------------------------------------