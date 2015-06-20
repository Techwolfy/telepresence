/**
 * @file  super_sock.h
 *
 * $Id: $
 *
 * @author  Team 691
 * @date    March 03, 2010
 *
 * A simple, efficient socket class that hides all binding and initialization
 *
 */
#ifndef  SUPER_SOCK_H
#define  SUPER_SOCK_H


/*
 * Includes
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h> 
#include <strings.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>

#include "nano_time.h"


class SuperSock 
{

 public:

  /** @{ @name Constructor/destructor/init */
  SuperSock();
  ~SuperSock();

  bool init(int port, unsigned long addr);
  bool init(int port, char * addr);
  bool init(char * port, char * addr);
  bool init();

  bool close_sock();

  /** @} */
 
  /** @{ @name I/O */

  int read(unsigned char * data, unsigned int len);
  int read_timeout(unsigned char * data, unsigned int len, nano_time timeout);
  int read_until(unsigned char * data, unsigned int len, nano_time endtime);

  int write(unsigned char * data, unsigned int len);
  int write_timeout(unsigned char * data, unsigned int len, nano_time timeout);
  int write_until(unsigned char * data, unsigned int len, nano_time endtime);
 
  /** @} */
 
 private:

  bool try_connect();

  int sock_port;
  unsigned long sock_addr;

  bool listening_sock;
  bool bind_complete;
  bool connect_complete;

  struct sockaddr_in sock_server;
  struct sockaddr_in sock_client;

  int sockfd;
  int linkfd;
  int fd;

  NanoTime nano_t;

};

#endif // SUPER_SOCK_H
