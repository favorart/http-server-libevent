#include "stdafx.h"
#include "http_error.h"
#include "http_server.h"

//-----------------------------------------
int   set_nonblock (evutil_socket_t fd)
{
  int flags;
#ifdef O_NONBLOCK
  if ( -1 == (flags = fcntl (fd, F_GETFL, 0)) )
    flags = 0;
  return fcntl (fd, F_SETFL, flags | O_NONBLOCK);
#else
  flags = 1;
  return ioctl (fd, FIOBIO, &flags);
#endif
}
//-----------------------------------------
int   http_server_config_init  (srv_conf *conf, char *path, char *port, char *ip)
{
  const uint16_t max_port = 65535;

  if ( !path ) path = "/";
  if ( !ip   )   ip = "0.0.0.0";

  if ( !port || (conf->port = atoi (port)) <= 0 || conf->port > max_port )
  {
    my_errno = HTTP_ERR_PORT;
    fprintf (stderr, "%s\n", strmyerror ());    
    conf->port = 8080;
  }

  struct stat sb;
  if ( stat (path, &sb) == 0 && S_ISDIR (sb.st_mode) )
  {
    printf ("YES\n");
  }
  else
  {
    printf ("NO\n");
  }

  if ( !(conf->folder_path = (char*) malloc (sizeof (char) * (strlen (path) + 1U))) )
    return 1;

  strcpy (conf->ip,     ip);
  strcpy (conf->path, path);
  
  return 0;
}
void  http_server_config_print (srv_conf *conf, FILE *stream)
{
  fprintf (stream, ">>> http server config:\n\n\tpath: '%s'\n\tport: %d   ip: %s\n\n",
           conf->path, conf->port, conf->ip);
}
void  http_server_config_free  (srv_conf *conf)
{
  free   (conf->path);
  memset (conf, 0, sizeof (*conf));
}
//-----------------------------------------
#include <getopt.h>   /* for getopt_long finction */
int  parse_console_parameters (int argc, char **argv, srv_conf *conf)
{
 char  *dir_opt = NULL, *port_opt = NULL, *ip_opt = NULL;

 // ./wwwd -d|--dir <dir> -h|--ip <ip> -p|--port <port>
 static struct option long_options[] =
 {
  {  "dir", required_argument, 0, 'd' },
  {   "ip", required_argument, 0, 'h' },
  { "port", required_argument, 0, 'p' },
  { 0, 0, 0, 0 }
 };

 int c, option_index = 0;
 while ( -1 != (c = getopt_long (argc, argv,
                                 "d:h:p:",
                                 long_options,
                                 &option_index)) )
 {
  // int this_option_optind = optind ? optind : 1;

  switch (c)
  {
   case 0:
    printf ("option %s", long_options[option_index].name);
    if ( optarg )
     printf (" with arg %s", optarg);
    printf ("\n");

    switch ( option_index )
    {
     case 0:  dir_opt = optarg; break;
     case 1:   ip_opt = optarg; break;
     case 2: port_opt = optarg; break;
     case 3:
      printf ("using:\n./wwwd -d|--dir <dir> -h|--ip <ip> -p|--port <port>\n\n");
      break;
    }
    break;

   case 'd':  dir_opt = optarg; break;
   case 'h':   ip_opt = optarg; break;
   case 'p': port_opt = optarg; break;

   case '?':
    /* getopt_long already printed an error message. */
    printf ("\nusing:\t./wwwd -d|--dir <dir> -h|--ip <ip> -p|--port <port>\n");
    break;

   default:
     printf ("?? getopt returned character code 0%o ??\n", c); break;
  }
 }

 if ( optind < argc )
 {
   printf ("non-option ARGV-elements: ");
   while ( optind < argc )
     printf ("%s ", argv[optind++]);
   printf ("\n");
 }
 
 return  http_server_config_init (conf, dir_opt, port_opt, ip_opt);
}
//-----------------------------------------
