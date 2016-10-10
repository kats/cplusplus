/*
   TCPAcceptor.h

   TCPAcceptor class interface. TCPAcceptor provides methods to passively
   establish TCP/IP connections with clients.

   ------------------------------------------

   Copyright � 2013 [Vic Hargrave - http://vichargrave.com]

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

#ifndef __tcpacceptor_h__
#define __tcpacceptor_h__

#include <string>
#include <memory>
#include <netinet/in.h>
#include "tcpstream.h"

class TCPAcceptor
{
	int		m_port;
	string	m_address;
	int		m_listning_socket = 0;
	bool	m_listening = false;

public:
	TCPAcceptor(int port, const std::string& address = "");
	TCPAcceptor(int port, const char* address);
	~TCPAcceptor();

	int						start();
	unique_ptr<TCPStream>	accept();
};

#endif