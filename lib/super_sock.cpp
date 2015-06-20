/** 
 * @file  super_sock.cc
 *
 * $Id: $
 *
 * @author  Team 691
 * @date    March 15, 2010
 *
 * A simple, efficient socket class that hides all binding and initialization
 *
 */ 

#include "super_sock.h"


using namespace std;

SuperSock::SuperSock()
{
  sockfd = -1;
  linkfd = -1;
  fd = -1;
  sock_port = -1;
  sock_addr = -1;
  listening_sock = false;
  bind_complete = false;
  connect_complete = false;
  nano_t.init();
}


SuperSock::~SuperSock()
{
  close_sock();
}

bool SuperSock::close_sock()
{
  if(linkfd >= 0)
    {
      shutdown(linkfd, SHUT_RDWR);
      close(linkfd);
    }

  if(sockfd >= 0)
    {
      shutdown(sockfd, SHUT_RDWR);
      close(sockfd);
    }
  sockfd = -1;
  linkfd = -1;
  fd = -1;
  listening_sock = false;
  bind_complete = false;
  connect_complete = false;

}


bool SuperSock::init(int port, unsigned long addr)
{
  struct in_addr in;

  /*
   * Save the socket and port address for later use
   */
  sock_port = port;
  sock_addr = addr;

  in.s_addr = addr;
  cout << "Port = " << port << ", Address = " << inet_ntoa(in) << endl;

  if(sockfd < 0)
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

  if(sockfd < 0)
    { 
      cout << "ERROR opening socket" << endl;
      return false;
    }
  

  /*
   * Make sure the socket bind does not fail because we used it a few minutes 
   * ago.
   */
  int on = 1;
  if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0)
    {
      cout << "ERROR: Socket option failed with error - " << errno << endl;
    }


  /*
   * Fill in the server address
   */
  bzero((char *) &sock_server, sizeof(sock_server));
  sock_server.sin_family = AF_INET;
  sock_server.sin_addr.s_addr = addr;
  sock_server.sin_port = htons(port);

  /*
   * Make sure the socket is non-blocking
   */
  fcntl(sockfd, F_SETFL, O_NONBLOCK);


  return try_connect();
}


bool SuperSock::init(int port, char * addr)
{
  /*
   * See if this address is by inet dot notation
   */
  struct in_addr inp;
  if(inet_aton(addr, &inp))
    {
      return init(port, inp.s_addr);
    }


  /*
   * See if we can get the host by name
   */
  struct hostent * server = gethostbyname(addr);
  if (server == NULL) 
    {
      cout << "Host " << addr << " not found by name" << endl;
    }
  else
    {
      cout << "Host " << addr << " found as " << *(unsigned long *)server->h_addr << endl;
      return init(port, *(unsigned long *)server->h_addr);
      
    }

  return false;
}

bool SuperSock::init(char * port, char * addr)
{

  int  portno = atoi(port);
  
  return init(portno, addr);
}

bool SuperSock::init()
{

  if(sock_port == -1 || sock_addr == -1)
    {
      // If nothing is set already, reset to default values
      return init(20000, (char *)"localhost");
    }
  else
    {
      return init(sock_port, sock_addr);
    }
}

int SuperSock::read(unsigned char * data, unsigned int len)
{
  /*
   * Read until the end of time
   */
  return read_until(data, len, MAX_64_BIT);
}



int SuperSock::read_timeout(unsigned char * data, unsigned int len, nano_time timeout)
{
  /*
   * Calculate a timeout
   */
  return read_until(data, len, nano_t.get_time_nsec() + timeout);
}



int SuperSock::read_until(unsigned char * data, unsigned int len, nano_time endtime)
{
  /*
   * We will wait until we connect before trying to read
   */
  while(!connect_complete)
    {
      try_connect();
      if(endtime < nano_t.get_time_nsec())
        return -1;
    }
  int bytes_in = 0;
  int rc = 0;

  while(bytes_in < len)
    {
      rc = ::read(fd, data + bytes_in, len - bytes_in);
      if(rc < 0)
        {
          if(errno != EWOULDBLOCK)
            {
              cout << "Read failed" << endl;
              return -1;
            }
        }
      else
        {
          bytes_in += rc;
        }
      if(endtime < nano_t.get_time_nsec())
        return -1;
    }
  return bytes_in;
}



int SuperSock::write(unsigned char * data, unsigned int len)
{
  /*
   * Write until the end of time
   */
  return write_until(data, len, MAX_64_BIT);
}



int SuperSock::write_timeout(unsigned char * data, unsigned int len, nano_time timeout)
{
  /*
   * Calculate a timeout
   */
  return write_until(data, len, nano_t.get_time_nsec() + timeout);
}



int SuperSock::write_until(unsigned char * data, unsigned int len, nano_time endtime)
{
  /*
   * We will wait until we connect before trying to write
   */
  while(!connect_complete)
    {
      try_connect();
      if(endtime < nano_t.get_time_nsec())
        return -1;
    }
  int bytes_out = 0;
  int rc = 0;

  while(bytes_out < len)
    {
      rc = ::write(fd, data + bytes_out, len - bytes_out);
      if(rc < 0)
        {
          if(errno != EWOULDBLOCK)
            {
              cout << "Write failed, restarting connection" << endl;
	      
	      close_sock();
	      nano_t.nano_sleep(100000000);
	      init();
              return -1;
            }
        }
      else
        {
          bytes_out += rc;
        }
      if(endtime < nano_t.get_time_nsec())
        return -1;
    }
  return bytes_out;
}



 
bool SuperSock::try_connect()
{
  if(sock_addr == INADDR_ANY)
    {
      /*
       * We are a listening socket
       */
      listening_sock = true;
//       cout << "Listening socket" << endl;

      if(!bind_complete)
        {
          if (bind(sockfd, (sockaddr *) &sock_server, sizeof(sock_server)) < 0) 
            {
              cout << "ERROR on binding" << endl;
              return false;
            }
          else
            {
              bind_complete = true;
            }
        }
      listen(sockfd,5);
      socklen_t len = sizeof(sock_client);
      linkfd = accept(sockfd, (sockaddr *) &sock_client, &len);
      
      if(linkfd < 0)
        {
//           cout << "Timed out on accept" << endl;
          nano_t.nano_sleep(10000);  /* Give back some CPU */
        }
      else
        {
          /*
           * Make sure the socket is non-blocking
           */
          fcntl(linkfd, F_SETFL, O_NONBLOCK);
          connect_complete = true;
          fd = linkfd;
        }
    }
  else
    {
      /*
       * We are a connecting socket
       */
//       cout << "Connecting socket" << endl;
      if(connect(sockfd, (sockaddr *)&sock_server,sizeof(sock_server)) < 0) 
        {
//           cout << "Failed to connect" << endl;
          nano_t.nano_sleep(10000);  /* Give back some CPU */
          
        }
      else
        {
          connect_complete = true;
          fd = sockfd;
        }


    }

  return true;
}
