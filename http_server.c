#include "stdafx.h"
#include "http_error.h"
#include "http_server.h"
#include "http_request.h"

//-----------------------------------------
/*   Для того, чтобы начать работать с буферизированным вводом/выводом необходимо:
 *   —  инициализировать структуру bufferevent вызовом bufferevent_new (),
 *      указав функции для обратного вызова на события
 *      чтения (необязательно), записи (необязательно) и ошибки (обязательно);
 *   —  Привязать это событие к локальной событийной базе (event_base) вызовом bufferevent_base_set ();
 *   —  Включать или выключать обработчики чтения/записи вызовами bufferevent_enable ()/bufferevent_disable ();
 *   —  Читать и писать вызовами bufferevent_read () и bufferevent_write () соответственно.
 *   Стоит отметить, что обработчик чтения будет вызван после того, как в буфере накопится весь пакет
 *   или закроется подключение, а обработчик записи — когда отправляемый буфер отправлен либо полностью,
 *   либо до указанной отметки (watermark).
 */

void  http_ac_err_cb (evutil_socket_t fd, short ev, void *arg)
{
  struct event_base *base = (struct event_base*) arg;

  int err = EVUTIL_SOCKET_ERROR ();
  fprintf (stderr, "Got an error %d (%s) on the listener. "
           "Shutting down.\n", err, evutil_socket_error_to_string (err));

  event_base_loopexit (base, NULL);
}
/* Connecting handling */
void  http_accept_cb (evutil_socket_t fd, short ev, void *arg)
{
  /* A new connection! Set up a bufferevent for it */
  struct event_base  *base =  (struct event_base*) arg;

  int  SlaveSocket = accept (fd, 0, 0);
  if ( SlaveSocket == -1 )
  { fprintf (stderr, "%s\n", strerror (errno));
    return;
  }
  
  set_nonblock (SlaveSocket);

  /* Making the new client */
  struct client *Client = (struct client*) calloc (1U, sizeof (*Client));
  if ( !Client )
  { fprintf (stderr, "%s\n", strerror (errno));
    return;
  }
  Client->base = base;

  /* Create new bufferized event, linked with client's socket */
  Client->b_ev = bufferevent_socket_new (base, SlaveSocket, BEV_OPT_CLOSE_ON_FREE);
  bufferevent_setcb  (Client->b_ev, http_read_cb, NULL /* http_write_cb */ , http_error_cb, Client);
  /* Ready to get data */
  bufferevent_enable (Client->b_ev, EV_READ | EV_WRITE | EV_PERSIST);

#ifdef _DEBUG
  printf ("connection established\n");
#endif
}
//-----------------------------------------
void  http_read_cb  (struct bufferevent *b_ev, void *arg)
{
  struct client  *Client = (struct client*) arg;
  /* This callback is invoked when there is data to read on b_ev_read */
  http_request_parse (&Client->request, bufferevent_get_input (b_ev));

#ifdef _DEBUG
  printf ("request reseived\n");
#endif

  /* Copy all the data from the input buffer to the output buffer. */
  http_response_make (&Client->request, bufferevent_get_output (b_ev));
  bufferevent_setwatermark (b_ev, EV_WRITE, 0, 0);

#ifdef _DEBUG
  printf ("response ready\n");
#endif
}
void  http_write_cb (struct bufferevent *b_ev, void *arg)
{
  struct client *Client = (struct client*) arg;
  
  // bufferevent_write
  // bufferevent_write_buffer ()
#ifdef _DEBUG
  printf ("write event\n");
#endif

  // http_response_make (&Client->request, bufferevent_get_output (b_ev));
  // bufferevent_write_buffer (b_ev, ev_buf);
}
void  http_error_cb (struct bufferevent *b_ev, short events, void *arg)
{
  struct client *Client = (struct client*) arg;
#ifdef _DEBUG
  if ( events & BEV_EVENT_EOF )
  {
    printf ("Got a close. Len = %u\n", evbuffer_get_length (bufferevent_get_output (b_ev)));
  }
#endif
  if ( events & BEV_EVENT_ERROR )
  {
    fprintf (stderr, "Error from bufferevent: '%s'\n",
             evutil_socket_error_to_string (EVUTIL_SOCKET_ERROR ()));
  }
  if ( events & (BEV_EVENT_EOF | BEV_EVENT_ERROR) )
  { 
    http_request_free (&Client->request);
    bufferevent_free  ( Client->b_ev);
  
    free (Client);
  }
}
//-----------------------------------------
// void  on_read (evutil_socket_t fd, short ev, void *arg)
// {
//   struct client  *Client = (struct client*) arg;
// 
//   static char Buffer[1024];
//   int  recv_size = recv (fd, Buffer, 1024, MSG_NOSIGNAL);
//   if ( recv_size <= 0 )
//   {
//     shutdown (fd, SHUT_RDWR);
//     close (fd);
// 
//     event_del (&(Client->ev_read));
//     free (Client);
//   }
// 
//   send (fd, Buffer, recv_size, MSG_NOSIGNAL);
// }
// //-----------------------------------------
// void  on_accept (evutil_socket_t fd, short ev, void *arg)
// {
//   int  SlaveSocket = accept (fd, 0, 0);
//   if ( SlaveSocket == -1 )
//   {
//     fprintf (stderr, "%s\n", strerror (errno));
//     return;
//   }
// 
//   set_nonblock (SlaveSocket);
// 
//   struct client *Client = (struct client*) calloc (1U, sizeof (*Client));
//   if ( !Client )
//   {
//     fprintf (stderr, "%s\n", strerror (errno));
// 
//     shutdown (SlaveSocket, SHUT_RDWR);
//     close (SlaveSocket);
//     return;
//   }
// 
//   event_set (&Client->ev_read, SlaveSocket, EV_READ | EV_PERSIST, on_read, Client);
//   event_add (&Client->ev_read, NULL);
// }
//-----------------------------------------
