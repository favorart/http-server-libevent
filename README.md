> **Moved:** this server now lives in [favorart/network-servers](https://github.com/favorart/network-servers/tree/main/http-server), with its full history, tests and releases. This repository is archived.

# HTTP server 

Technosphere.mail.ru 2015

using libevent

console arguments:
  + -h/--ip   [server ip adress]
  + -p/--port [listening port]
  + -d/--dir  [folder with files]

supports

 1. requests:
    +  GET (html/jpeg files),
    +  HEAD,
    +  POST (just 200 answer)
 2. responses:
    +  200
    +  404
  
