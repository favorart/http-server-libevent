# Home work #1: HTTP server 

Technosphere.mail.ru 2015

using libevent

console arguments:
  + -h/--ip   [server ip adress]
  + -p/--port [listening port]
  + -d/--dir  [folder with files]

supports
requests:
  +  GET (html/jpeg files),
  +  HEAD,
  +  POST (just 200 answer)
  
responses:
  +  200
  +  404
  
