/*******************************************************************************/
/** Inet_socket Classes. Copyright (c) 2002 by Pawel Wilk <siefca@kernel.pl>  **/
/**                                                                           **/
/** Please contact author to ask for license terms.			      **/
/** Redistribution and/or using in any form without permission is prohibited. **/
/*******************************************************************************/


#include "inet.hh"

const string inet_socket::string_in_any("0.0.0.0");

/*************************************************************************/
/* class: inet_socket							 */
/*************************************************************************/

inet_socket::inet_socket ()
{
	setstructs();
	set_addr(string_in_any, s_addr_in);
	set_state(CLOSED);
}

/*************************************************************************/

inet_socket::inet_socket (const inet_socket &i)
{
	setstructs();
	set_addr(string_in_any, s_addr_in);
	set_state(CLOSED);
	*this = i;		//note: operator = is redefined later
}

/*************************************************************************/

inet_socket::inet_socket (const string &adr, int port)
{
	setstructs();
	set_addr(adr, s_addr_in);
	set_port(port, s_addr_in);
	set_state(CLOSED);
}

/*************************************************************************/

inet_socket::inet_socket (const string &adr, const string &port)
{
	setstructs();
	set_addr(adr, s_addr_in);
	set_port(port, s_addr_in);
	set_state(CLOSED);
}

/*************************************************************************/

inet_socket::inet_socket (const string &s)
{
	string host, port;
	setstructs();
	parse_hostport(s, host, port);
	set_addr(host, s_addr_in);
	set_port(port, s_addr_in);
	set_state(CLOSED);
}

/*************************************************************************/

inet_socket::~inet_socket ()
{
	if ((state()) != TEMP_ACC2) disconnect();
}

/*************************************************************************/

inet_socket &inet_socket::operator =(const inet_socket &i)
{
	setstructs();
	(void) memcpy (s_addr_in, i.s_addr_in, sizeof(struct sockaddr_in));
	(void) memcpy (r_addr_in, i.r_addr_in, sizeof(struct sockaddr_in));

	if ((state()) != CLOSED && (state()) != TEMP_ACC && (state()) != TEMP_ACC2)
		throw erro("inet_socket::operator =",
			   "socket is already open - cannot assign new value");
	
	if ((i.state()) != CLOSED   &&
	    (i.state()) != TEMP_ACC &&
	    (i.state()) != TEMP_ACC2   )
	{
		int new_socket = dup(i.get_socket());
		if (new_socket == -1) throw erro("inet_socket::inet_socket dup");
		set_socket(new_socket);
	} else
		set_socket(i.get_socket());

	if ((i.state()) == TEMP_ACC2)
		set_state(CONNECTED);
	else
		set_state(i.state());

	refresh_adr_local();
	refresh_adr_remote();
	ret_bad_lines = i.ret_bad_lines;
	remove_delimiter = i.remove_delimiter;
	read_timeout = i.read_timeout;
	
	return *this;
}

/*************************************************************************/

void inet_socket::disconnect ()
{
	if ((state()) != CLOSED)
	{
		if ((::close(socket)) == -1)
		{
			char buf[16];
			snprintf (buf, 15, "%d", state());
			throw erro("inet_socket::disconnect close",
				   "socket state was: " + string(buf));
		}
		set_state(CLOSED);
	}
}

/*************************************************************************/

void inet_socket::setstructs ()
{
	memset (&s_addr, 0, sizeof(struct sockaddr));
	memset (&r_addr, 0, sizeof(struct sockaddr));
	s_addr_in = (struct sockaddr_in *) &s_addr;
	r_addr_in = (struct sockaddr_in *) &r_addr;
	omit_delimiters();
	omit_badlines();
	timeout(DEFAULT_READ_TIMEOUT);
	c_notc = true;
	c_refus = false;
}

/*************************************************************************/

void inet_socket::parse_hostport (const string s, string &a, string &b) const
{
	char *buf;
	register const char *p = s.c_str(), *beg;
	
	//leading separators
	if (*p == ' ' || *p == '\t')
		while (*p && (*p == ' ' || *p == '\t')) p++;
	if (!*p) throw erro("inet_socket::parse_hostport syntax error",
		            string("host:port string=") + s);

	//first string
	beg = p;
	while (*p && *p != ' ' && *p != '\t' && *p != ':') p++;
	if (!*p || beg == p)
		throw erro("inet_socket::parse_hostport syntax error",
		            string("host:port string=") + s);

	buf = (char *) malloc (p-beg+1);
	if (!buf) throw erro("inet_socket::parse_hostport malloc 1");
	memcpy (buf, beg, (p-beg));
	*(buf+(p-beg)) = (char) 0;
	a = buf;
	free(buf);

	//delimiter
	beg = p;
	while (*p && (*p == ' ' || *p == '\t' || *p == ':')) p++;
	if (!*p || beg == p)
		throw erro("inet_socket::parse_hostport syntax error",
		            string("host:port string=") + s);

	//last string
	beg = p;
	while (*p && *p != ' ' && *p != '\t') p++;
	if (beg == p)
		throw erro("inet_socket::parse_hostport syntax error",
		            string("host:port string=") + s);

	buf = (char *) malloc (p-beg+1);
	if (!buf) throw erro("inet_socket::parse_hostport malloc 2");
	memcpy (buf, beg, (p-beg));
	*(buf+(p-beg)) = (char) 0;
	b = buf;
	free(buf);
	
	//additional hacks
	if (a == "*" || a == "0") a = string_in_any;
}

/*************************************************************************/

void inet_socket::set_addr (const string &s, struct sockaddr_in *soi)
{
	signed char c = 0;
	struct hostent *h;
	
	if (s.empty()) throw erro("inet_socket::set_addr", "empty hostname string");
	while ( !(h = gethostbyname (s.c_str())) && c < 2) c++;
	if (!h || !h->h_addr_list)
		throw erro("inet_socket::set_addr gethostbyname", s);
	(void) memcpy(&soi->sin_addr, h->h_addr_list[0], sizeof(struct in_addr));
}

/*************************************************************************/

string inet_socket::local_address ()
{
	refresh_adr_local();
	return string(inet_ntoa(s_addr_in->sin_addr));
}

/*************************************************************************/

string inet_socket::remote_address ()
{
	refresh_adr_remote();
	return string(inet_ntoa(r_addr_in->sin_addr));
}

/*************************************************************************/

struct in_addr inet_socket::local_inaddr ()
{
	refresh_adr_local();
	return s_addr_in->sin_addr;
}

/*************************************************************************/

struct in_addr inet_socket::remote_inaddr ()
{
	refresh_adr_remote();
	return r_addr_in->sin_addr;
}

/*************************************************************************/

void inet_socket::set_port (const string &portname, struct sockaddr_in *soi)
{
	if (portname.empty()) throw erro("inet_socket::set_port", "empty port name");
	register const char *p = portname.c_str();
	if (!p) throw erro("inet_socket::set_port", "null pointer in portname");
	while (*p)
	{
		if (!isdigit(*p))
		{
			struct servent *pr = getservbyname (portname.c_str(), "tcp");
			if (!pr) throw erro("inet_socket::set_port getservbyname", portname);
			set_port_nbo(pr->s_port, soi);
			endservent();
			return;
		}
		p++;
	}
	set_port(atoi(portname.c_str()), soi);
}

/*************************************************************************/

int inet_socket::local_port ()
{
	refresh_adr_local();
	return (ntohs(s_addr_in->sin_port));
}

/*************************************************************************/

int inet_socket::remote_port ()
{
	refresh_adr_remote();
	return (ntohs(r_addr_in->sin_port));
}

/*************************************************************************/

string inet_socket::local_address_port ()
{
	char buf[16];
	snprintf (buf, 15, "%d", local_port());
	string s = local_address();
	s += ":";
	s += buf;
	return s;
}

/*************************************************************************/

string inet_socket::remote_address_port ()
{
	char buf[16];
	snprintf (buf, 15, "%d", remote_port());
	string s = remote_address();
	s += ":";
	s += buf;
	return s;
}

/*************************************************************************/

void inet_socket::do_socket ()
{
	if (state() != CLOSED)
		throw erro("inet_socket::do_socket", "socket already created");
	socket = ::socket(PF_INET, SOCK_STREAM, 0);
	if (socket == -1) throw erro("inet_socket::do_socket socket");
	set_state(FREE);
}

/*************************************************************************/

void inet_socket::bind ()
{
	int r;
	
	if ((state()) == CLOSED) do_socket();
	else if ((state()) != FREE)
		throw erro("inet_socket::bind",
			   "trying to bind a non-free socket");
	
	if (!(local_port()))
		throw erro("inet_socket::bind",
			   "need at least port number to bind");
	
	r = ::bind(socket, &s_addr, sizeof(struct sockaddr));
	if (r == -1) throw erro("inet_socket::bind", local_address_port());
	set_state(BIND);
}

/*************************************************************************/

void inet_socket::bind(int port)
{
	set_port (port, s_addr_in);
	set_addr ("0.0.0.0", s_addr_in);
	bind();
}

/*************************************************************************/

void inet_socket::bind(const string &adr, int port)
{
	set_port (port, s_addr_in);
	set_addr (adr, s_addr_in);
	bind();
}

/*************************************************************************/

void inet_socket::bind(const string &adr, const string &port)
{
	set_port (port, s_addr_in);
	set_addr (adr, s_addr_in);
	bind();
}

/*************************************************************************/

void inet_socket::bind(const string &host_port)
{
	string host, port;
	parse_hostport (host_port, host, port);
	set_port (port, s_addr_in);
	set_addr (host, s_addr_in);
	bind();
}

/*************************************************************************/

void inet_socket::refresh_adr_local()
{
	if ((state()) == CLOSED || (state()) == FREE) return;
	register unsigned int l = sizeof(sockaddr);
	getsockname(socket, &s_addr, &l);
}

/*************************************************************************/

void inet_socket::refresh_adr_remote()
{
	if((state()) != CONNECTED) return;
	register unsigned int l = sizeof(sockaddr);
	getpeername(socket, &r_addr, &l);
}

/*************************************************************************/

bool inet_socket::reuse_addr (int val=-1)
{
	if (val < 0 || val == s_reuse_addr) return s_reuse_addr;
	setopt(SOL_SOCKET, SO_REUSEADDR, (char *) &val, sizeof(val));
	bool prev_reuse = s_reuse_addr;
	s_reuse_addr = val;
	return prev_reuse;
}

/*************************************************************************/

void inet_socket::setopt (int level, int optname, const void *optval,
				 socklen_t optlen)
{
	if ((state()) == CLOSED) do_socket();
	if ((setsockopt(socket, level, optname, optval, optlen)) == -1)
		throw erro("inet_socket::setopt");
}

/*************************************************************************/

int inet_socket::timeout (int seconds=-1)
{
	int prev_timeout = read_timeout;
	if (seconds >= 0) read_timeout = seconds;
	return prev_timeout;
}

/*************************************************************************/

string inet_socket::clear_buffer ()
{
	string s = s_buf;
	s_buf.erase();
	return s;
}

/*************************************************************************/

bool inet_socket::getbuf (unsigned int count, string &result,
			  const string &delim, unsigned int now=0)
{
	unexpected = false;
	if (s_buf.empty()) return false;
	if (delim.empty())
	{
		/* without delimiting string */
		if ((s_buf.length()) < count) return false;
		result = s_buf.substr(0, count);
		s_buf.erase(0, count);
		return true;
	}
	else
	{
		/* with delimiting string */
		unsigned int pos;
		int from = 0;
		unsigned int to = count; /* this is the number of characters NOT the position */
		if (!now)
		{
			/* there was no real read before */
			if (to > (s_buf.length())) to = s_buf.length();
			pos = s_buf.substr(0, to).find(delim, 0);
		}
		else
		{
			/* there was read and a buffer refresh before */
			from = (s_buf.length()) - now - (delim.length()) + 1;
			if (from < 0) from = 0;
			to -= from;
			if (to <= 0) pos = string::npos;
			else pos = s_buf.substr(from, to).find(delim, 0);
		}

		if (pos == string::npos)
		{
			/* delimiter not found */
			if ((s_buf.length()) < count) return false; /* there is a chance */
			unexpected = true;
			if (!ret_bad_lines)
			{
				/* return empty string */
				s_buf.erase(0, count);
				result.erase();
				return true;
			}
			/* return unmatched line wchich has reached max requested size */
			result  = s_buf;
			s_buf.erase(0, count);
			return true;
		}

		/* delimiter was found */
		pos += from; 				/* relative to global positioning */
		result = s_buf.substr(0, pos);

		/* should we remove delimiter from buffer? */
		if (remove_delimiter) pos += delim.length();
		s_buf.erase(0, pos);
		return true;
	}
}

/*************************************************************************/

string inet_socket::read (string &output,
			  unsigned int count, int rtimeout=-1,
			  const string &delim="")
{
	r_timed_out = false;
	if ((state()) != CONNECTED)
		throw erro("inet_socket::read", "invalid socket state");
	if (!count)
	{
		output = string("");
		return "";
	}
	
	/* first try to read the data from buffer */
	if (getbuf(count, output, delim)) return "";

	char *buf = (char *) calloc(count+1, sizeof(char));
	if (!buf) throw erro("inet_socket::read calloc");

	int x, r;
	fd_set rd;
	struct timeval tv;
	unsigned int r_count = count;

	if (rtimeout < 0) rtimeout = read_timeout;
	
	c_refus = false;
	c_notc = false;
	
	while (count)
	{
		FD_ZERO(&rd);
		FD_SET(socket, &rd);
		tv.tv_sec = rtimeout;
		tv.tv_usec = 0;
		if (!(x=select(socket+1, &rd, NULL, NULL, &tv)))
		{
			free(buf);
			errno = 0;
			r_timed_out = true;
			throw erro("inet_socket::read", "read timeout on socket");
		}
		if (x == -1) throw erro("inet_socket::read select");
		r = ::recv(socket, buf, count, MSG_NOSIGNAL|MSG_DONTWAIT);
		if (r == -1 && errno != EAGAIN && errno != EWOULDBLOCK && errno != EINTR)
		{
			r = 0;
			switch (errno)
			{
				case EPIPE:
				case ENOTCONN:
						if ((state()) == CONNECTED)
							set_state(FREE);
						c_notc = true;
						break;
				case ECONNREFUSED:
						if ((state()) == CONNECTED)
							set_state(FREE);
						c_refus = true;
						break;
				case ENOTSOCK:
						set_state(CLOSED);
						break;
				default:
						break;
			}
			
			free(buf);
			throw erro("inet_socket::read recv");
		} else if (!r) //connection was closed
		{
			free(buf);
			c_notc = true;
			set_state(CLOSED);
			throw erro("inet_socket::read", "endpoint has closed connection");
		}
		insbuf(buf, r);				//insert into internal buffer
		if (getbuf(r_count, output, delim, r))	//check if buffer contains sufficient amount of data
		{
			free(buf);
			return "";
		}
		count -= r;
	}

	free(buf);
	output = string("");
	return "";
}

/*************************************************************************/

string inet_socket::read (unsigned int count, int rtimeout=-1,
			  const string &delim="")
{
	string s;
	read (s, count, rtimeout, delim);
	return s;
}

/*************************************************************************/

void inet_socket::write (const string &s, unsigned int count, int rtimeout=-1)
{
	w_timed_out = false;
	if ((state()) != CONNECTED)
		throw erro("inet_socket::write", "invalid socket state");
	if (s.empty() || !count) return;
	if (count > (s.length())) count = s.length();
	if (rtimeout < 0) rtimeout = read_timeout;
	
	int x, r;
	fd_set wr;
	struct timeval tv;
	unsigned int from = 0;

	c_refus = false;
	c_notc = false;
	
	while (count)
	{
		FD_ZERO(&wr);
		FD_SET(socket, &wr);
		tv.tv_sec = rtimeout;
		tv.tv_usec = 0;
		if (!(x=select(socket+1, NULL, &wr, NULL, &tv)))
		{
			errno = 0;
			w_timed_out = true;
			throw erro("inet_socket::write", "write timeout on socket");
		}
		if (x == -1) throw erro("inet_socket::write select");
		r = ::send(socket, s.substr(from).c_str(),
			   count, MSG_DONTWAIT|MSG_NOSIGNAL);
		if (r == -1 && errno != EAGAIN && errno != EWOULDBLOCK && errno != EINTR)
		{
			r = 0;
			switch (errno)
			{
				case ENOTCONN:
						if ((state()) == CONNECTED)
							set_state(FREE);
						c_notc = true;
						break;
				case ECONNREFUSED:
						if ((state()) == CONNECTED)
							set_state(FREE);
						c_refus = true;
						break;
				case ENOTSOCK:
						set_state(CLOSED);
						break;
				default:
						break;
			}
			throw erro("inet_socket::write send");
		}
		count -= r;
		from += r;
	}
}

/*************************************************************************/

string inet_socket::readline (unsigned int count=128)
{
		string s = read(count, -1, "\n");
		unsigned int lchar = s.length() - 1;
		if (s.substr(lchar) == "\r") s.erase(lchar);
		return s;
}

/*************************************************************************/

string inet_socket::readlines (unsigned int lines, unsigned int count=128)
{
	if (!(count*lines)) return string("");
	string s;
	while (lines)
	{
		s += readline(count);
		s += "\n";
		lines--;
	}
	return s;
}



/*************************************************************************/
/* class: inet_client							 */
/*************************************************************************/

void inet_client::connect()
{
	if ((state()) == CLOSED) do_socket();
	if ((state()) != FREE) throw erro("inet_client::connect", "invalid socket state");
	set_state(CONNECTING);
	if ((::connect(get_socket(), &r_addr, sizeof(struct sockaddr))) == -1)
			throw erro("inet_client::connect");
	set_state(CONNECTED);
}

/*************************************************************************/

void inet_client::connect(const string &s, int p)
{
	set_addr(s, r_addr_in);
	set_port(p, r_addr_in);
	connect();
}
		
/*************************************************************************/

void inet_client::connect(const string &s, const string &p)
{
	set_addr(s, r_addr_in);
	set_port(p, r_addr_in);
	connect();
}
		
/*************************************************************************/

void inet_client::connect(const string &s)
{
	string host, port;
	parse_hostport(s, host, port);
	set_addr(host, r_addr_in);
	set_addr(port, r_addr_in);
	connect();
}


/*************************************************************************/
/* class: inet_server							 */
/*************************************************************************/

void inet_server::listen(int backlog=5)
{
	if ((state()) != BIND) bind();
	if ((::listen(get_socket(), backlog)) == -1)
		throw erro("inet_server::listen");
	set_state(LISTENING);
}

/*************************************************************************/

inet_socket inet_server::accept ()
{
	int new_socket = -1;
	if ((state()) > TEMP_ACC2)
		throw erro("inet_server::accept", "invalid socket state");
	if ((state()) != LISTENING) listen();
	
	(void) memset(&r_addr, 0, sizeof(struct sockaddr_in));
	
	errno = EINTR;
	while (new_socket == -1 && errno == EINTR)
		new_socket = ::accept(get_socket(), &r_addr, NULL);

	if (new_socket == -1)
	{
		if (errno == ECONNABORTED) c_notc = true;
		throw erro("inet_server::accept");
	}
	
	socket_state prev_state = state();
	set_state(TEMP_ACC);
	inet_socket i(*this); //nasty hack to get into the private area
	inet_server *x = (inet_server *) &i;
	set_state(prev_state);
	x->set_state(TEMP_ACC2);
	x->set_socket(new_socket);

	return i;
}

/*************************************************************************/

inet_socket inet_server::accept (const string &addr)
{
	inet_socket s = accept();
	while (s.remote_address() != addr)
	{
		if (!p_msg.empty())
			s.write(p_msg, p_msg.length(), 5);
		s.disconnect();
		s = accept();
	}

	return s;
}


/*************************************************************************/
/* test stuff								 */


/*
//to let it work, remember to #include <ostream>

int main (int argc, char *argv[])
{
	try {
		erro e;
		e.set_application_name("mdeva");
	} catch (erro e) { e.show_err(); exit(1); }


	
	try {
	Cgicc cgi;
	} catch (exception &e) {
	    exit(7);
	}
	
	inet_socket s; //incomming connections socket
	inet_server i; //server socket

	try{
		i.reuse_addr (true);			// reusable addresses
		i.bind ("*:fido");			// bind to INADDR_ANY port: fido
		i.timeout(8); 				// inherited by newly created socket objects
		
		cerr << "server socket ready"	<< endl
		     << "socket type: "		<< i.type()			<< endl
		     << "bound to: "		<< i.local_address_port()	<< endl
		     << endl;
		
		s = i.accept();
		
		cerr << "GOT CONNECT!"	<< endl
		     << "socket type: "	<< s.type()			<< endl
		     << "timeout: "	<< s.timeout()			<< endl
		     << "local: "	<< s.local_address_port()	<< endl
		     << "remote: "	<< s.remote_address_port()	<< endl
		     << endl;

		string result;
		while (result != "quit")			// repeat until remote end will send quit
		{
			s.write("type something: ");		// write prompt
			s.write(s.show_buffer());
			try
			{
				result = s.read(25, "\r\n");	// get up to 25 characters finished by \r\n sequence
			}
			catch (erro e)
			{
				if (!s.read_timed_out()) throw e;
				// and nothing more here, due to exception while catching other exception
				// which shouldn't happend anymore - write method can throw an exception
			}

			if (s.read_timed_out())
			{
				s << HTTPHTMLHeader() << HTTPHTMLHeader();//"\t\t\t\tfaster faster Helga!\n\r";
				// uncomment if you want to delete previous input: s.clear_buffer();
				continue;
			}
			if (s.unexpected_read())
			{
				cerr	<< "wicked :/ - no delimiter found in previous string"
					<< endl;
				s.write("\t\t\t\tHelga, relax...\n\r");
				continue;
			}
			
			cerr	<< "client typed: "
					<< result	
					<< endl;

		}
		s.close();
		i.close();
	}
	catch (erro e)
	{
		e.show_err();
		exit(2);
	}
	
	sleep (1);
	
	exit (0);
}

*/


