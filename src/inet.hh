/*****************************************************************************/
/** Inet_socket Classes. Copyright (c) 2002 by Pawel Wilk <siefca@kernel.pl>**/
/**                                                                         **/
/** Please contact author to ask for license terms.			    **/
/** Redistribution and/or using in any form without permission is prohibited**/
/*****************************************************************************/

#ifndef _INET_HH
#define _INET_HH

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifndef HAVE_SNPRINTF_H
#include "snprintf.h"
#endif

#include "erro.hh"

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <string>

#ifndef	DEFAULT_READ_TIMEOUT
#define	DEFAULT_READ_TIMEOUT	60
#endif

enum socket_state { CLOSED=0, FREE=1, BIND=2, LISTENING=3,
		    TEMP_ACC=4, TEMP_ACC2=5, CONNECTING=6, CONNECTED=7 };

enum socket_type { REGULAR=0, CLIENT, SERVER };

/*************************************************************************/

class inet_socket {
public:
  inet_socket();
  inet_socket(const inet_socket &);
  inet_socket(const string &, int);
  inet_socket(const string &, const string &);
  inet_socket(const string &);
  ~inet_socket();
  
  inet_socket &operator =(const inet_socket &);
  
  void bind();
  void bind(int);
  void bind(const string &, int);
  void bind(const string &, const string &);
  void bind(const string &);
  void disconnect();
  inline void close() { disconnect (); }
  
  bool reuse_addr(int);
  void setopt (int, int, const void *, socklen_t);
  
  inline void omit_delimiters() { remove_delimiter = true; }
  inline void keep_delimiters() { remove_delimiter = false; }
  inline void omit_badlines() { ret_bad_lines = false; }
  inline void keep_badlines() { ret_bad_lines = true; }
  int timeout(int);
  
  string clear_buffer();
  inline string show_buffer() { return s_buf; }
  
  string read (string &, unsigned int, int rtimeout=-1,
	       const string &delim="");
  string read (unsigned int, int, const string &delim="");
  inline string read (unsigned int count, const string &delim)
  { return read(count, -1, delim); }
  inline string read (const string &delim)
  { return read(128, -1, delim); }
  string readline(unsigned int count=128);
  string readlines(unsigned int, unsigned int);
  
  void write(const string &s, unsigned int count, int rtimeout=-1);
  inline void write(const string &s)
  { write(s, s.length()); }
  
  inline bool read_timed_out() const { return r_timed_out; }
  inline bool write_timed_out() const { return w_timed_out; }
  inline bool unexpected_read() const { return unexpected; }
  inline bool connection_refused() const { return c_refus; }
  inline bool connection_closed() const { return c_notc; }
  
  inline socket_state state() const { return s_state; }
  inline int get_socket() const { return socket; }
  string local_address();
  string remote_address();
  struct in_addr local_inaddr();
  struct in_addr remote_inaddr();
  int local_port();
  int remote_port();
  string local_address_port();
  string remote_address_port();
  
  static const string string_in_any;
  inline socket_type type() const { return REGULAR; }
protected:
  void do_socket();
  inline void set_socket(int s) { socket = s; }
  inline void set_state(socket_state s) { s_state = s; }
  
  void set_addr(const string &, struct sockaddr_in *);
  void set_port(const string &, struct sockaddr_in *);
  
  inline void set_port(int port, struct sockaddr_in *soi)
  { soi->sin_port = htons(port); }
  inline void set_port_nbo(int port, struct sockaddr_in *soi)
  { soi->sin_port = port; }
  
  void parse_hostport (const string, string &, string &) const;
  
  void refresh_adr_remote();
  void refresh_adr_local();
  
  bool getbuf (unsigned int, string &, const string &, unsigned int);
  inline void insbuf (const char *s, unsigned int count)
  { s_buf.append(s, count); }
  
  struct sockaddr s_addr;	//local side
  struct sockaddr r_addr;	//remote side
  
  struct sockaddr_in *s_addr_in;
  struct sockaddr_in *r_addr_in;
  
  bool r_timed_out,
    w_timed_out,
    c_notc,
    c_refus;
  
private:
  void setstructs ();
  bool ret_bad_lines,
    remove_delimiter,
    unexpected,
    s_reuse_addr;
  int socket,
    read_timeout;
  socket_state s_state;
  string s_buf;
};

/*************************************************************************/

class inet_client : public inet_socket {
public:
  inet_client() : inet_socket() {}
  inet_client(const string &adr, int port): inet_socket(adr, port) {}
  inet_client(const string &adr, const string &port): inet_socket(adr, port) {}
  inet_client(const string &adr_port): inet_socket(adr_port) {}

  void connect(const string &, int);
  void connect(const string &, const string &);
  void connect(const string &);
  
  inline socket_type type() const { return CLIENT; }

protected:
  inline void connect();			
};

/*************************************************************************/

class inet_server : public inet_socket {
public:
  inet_server(): inet_socket() {}
  inet_server(int port): inet_socket("0.0.0.0", port) {}
  inet_server(const string &adr, int port): inet_socket(adr, port) {}
  inet_server(const string &adr, const string &port): inet_socket(adr, port) {}
  inet_server(const string &host_port): inet_socket(host_port) {}
  
  void listen(int backlog=5);
  inet_socket accept();
  inet_socket accept(const string &);
  
  inline void set_prohibition_msg(const string &s) { p_msg = s; }
  inline string prohibition_msg() const { return p_msg; }
  
  inline socket_type type() const { return SERVER; }

private:
  string p_msg;
};

/*************************************************************************/

#endif

