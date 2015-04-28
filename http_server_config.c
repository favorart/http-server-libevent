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
int   http_server_config_init  (srv_conf *conf, char *path, char *port, char *ip, char* workers)
{
  //-----------------------
  const uint16_t  max_port = 65535U;
  if ( !port || (conf->port = atoi (port)) > max_port )
  { my_errno = HTTP_ERR_INPUT;
    fprintf (stderr, "%s\n", strmyerror ());    
    conf->port = 8080;
  }
  //-----------------------
  struct stat sb;
  if ( path && stat (path, &sb) == 0 && S_ISDIR (sb.st_mode) )
  {
    my_errno = HTTP_ERR_INPUT;
    fprintf (stderr, "%s\n", strmyerror ());
    strcpy (conf->folder_path, path);
  }
  else
  {
    strcpy (conf->folder_path, "data/");
  }
  //-----------------------
  if ( ip && strlen (ip) < HTTP_IPSTRLENGTH )
  {
    strcpy (conf->ip, ip);
  }
  else
  {
    strcpy (conf->ip, "0.0.0.0");
  }
  //-----------------------
  const uint16_t  max_workers = HTTP_MAX_WORKERS;
  if ( !workers || (conf->workers = atoi (workers)) > max_workers )
  { my_errno = HTTP_ERR_INPUT;
    fprintf (stderr, "%s\n", strmyerror ());
    conf->workers = 4;
  }
  //-----------------------
  /* exec working directory */
  // getcwd (conf->server_path, sizeof (conf->server_path))

  int  bytes = 0U;
  if ( 0 >= (bytes = readlink ("/proc/self/exe", conf->server_path, FILENAME_MAX - 1U)) )
  {
    fprintf (stderr, "%s\n", strerror (errno));
    strcpy  (conf->server_path, "");
  }
  else conf->server_path[bytes] = '\0';
  //-----------------------
  conf->ptr_server_name = HTTP_SERVER_NAME;
  //-----------------------
  return 0;
}
void  http_server_config_print (srv_conf *conf, FILE *stream)
{
  fprintf (stream, ">>> http server config:\n\n\tname: '%s'   server path: '%s'\n"
                   "\tpath: '%s'\n\tport: %u   ip: '%s'\n\tworkers count: %u\n\n",
                   conf->ptr_server_name, conf->server_path, conf->folder_path,
                   conf->port, conf->ip, conf->workers);
}
void  http_server_config_free  (srv_conf *conf)
{
  //-----------------------
  memset (conf, 0, sizeof (*conf));
}
//-----------------------------------------
#include <getopt.h>   /* for getopt_long finction */
int   parse_console_parameters (int argc, char **argv, srv_conf *conf)
{
  char  *dir_opt = NULL, *port_opt = NULL, *ip_opt = NULL, *work_opt = NULL;
  static struct option long_options[] =
  {
    {     "dir", required_argument, 0, 'd' },
    {      "ip", required_argument, 0, 'h' },
    {    "port", required_argument, 0, 'p' },
    { "workers", required_argument, 0, 'w' },
    { 0, 0, 0, 0 }
  };
  //-----------------------
  int c, option_index = 0;
  while ( -1 != (c = getopt_long (argc, argv, "d:h:p:",
    long_options, &option_index)) )
  {
    switch ( c )
    {
      case 0:
        printf ("option %s", long_options[option_index].name);
        if ( optarg )
          printf (" with arg %s", optarg);
        printf ("\n");
        //-----------------------
        switch ( option_index )
        {
          case 0:  dir_opt = optarg; break;
          case 1:   ip_opt = optarg; break;
          case 2: port_opt = optarg; break;
          case 3: work_opt = optarg; break;
          case 4:
            printf ("using:\n./wwwd -d|--dir <dir> -h|--ip <ip> -p|--port <port> -w|--workers <num>\n\n");
            break;
        }
        break;

      case 'd':  dir_opt = optarg; break;
      case 'h':   ip_opt = optarg; break;
      case 'p': port_opt = optarg; break;
      case 'w': work_opt = optarg; break;

      case '?':
        /* getopt_long already printed an error message. */
        printf ("using:\n./wwwd -d|--dir <dir> -h|--ip <ip> -p|--port <port> -w|--workers <num>\n\n");
        break;

      default:
        printf ("?? getopt returned character code 0%o ??\n", c); break;
    }
  }
  //-----------------------
  if ( optind < argc )
  {
    printf ("non-option ARGV-elements: ");
    while ( optind < argc )
      printf ("%s ", argv[optind++]);
    printf ("\n");
  }
  //-----------------------
  return  http_server_config_init (conf, dir_opt, port_opt, ip_opt, work_opt);
}
//-----------------------------------------
