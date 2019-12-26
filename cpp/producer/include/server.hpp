#ifndef SERVER_HPP_
#define SERVER_HPP_

#include <atomic>
#include <memory>
#include <assert.h>
#include <iostream>
#include <iterator> 
#include <algorithm>

#include "Poco/Net/TCPServer.h"
#include "Poco/Net/TCPServerConnection.h"
#include "Poco/Net/TCPServerConnectionFactory.h"
#include "Poco/Net/StreamSocket.h"
#include "Poco/Net/SocketStream.h"
#include "Poco/Net/ServerSocket.h"
#include "Poco/Exception.h"
#include "Poco/Util/ServerApplication.h"
#include "Poco/Util/Option.h"
#include "Poco/Util/OptionSet.h"
#include "Poco/Util/HelpFormatter.h"

#include "storage.hpp"
#include "generator.hpp"

using namespace Poco;
using namespace Poco::Net;
using namespace Poco::Util;

class ServerConnection: public TCPServerConnection
	/// This class handles all client connections.
{
public:
	ServerConnection(const StreamSocket& s, 
		             std::shared_ptr<Storage> storage) : 
		TCPServerConnection(s),
		storage(storage)
	{}

	void run()
	{
		Application& app = Application::instance();
		app.logger().information("Connection from " + this->socket().peerAddress().toString());
		Timespan timeout(1000000);
		while(true)
		{
			try
			{	
				app.logger().information("Waiting for request...");

				int64_t start_time, end_time;
				std::tie(start_time, end_time) = recv_time_period();

				app.logger().information("Requested data from: " + std::to_string(start_time) + " to " + std::to_string(end_time));
				std::vector<int> dt = storage->get_data(start_time, end_time);

				send_data(dt);

				app.logger().information("Sent " + std::to_string(dt.size()) + " items");
				std::ostream_iterator<int> out_it(std::cout, "\n");
				std::copy(dt.begin(), dt.end(), out_it);
			}
			catch (Poco::Exception& exc)
			{
				app.logger().log(exc);
			}
		}

		app.logger().information("Connection is closing");
	}

private:
	std::pair<int64_t, int64_t> recv_time_period()
	{
		int received_bytes = 0;
		unsigned char buffer[2 * sizeof(int64_t)];
		do
		{
			received_bytes += this->socket().receiveBytes(buffer + received_bytes, sizeof(buffer) - received_bytes);

		} while (received_bytes < 2 * sizeof(int64_t));

		int64_t start_time = *(int64_t*)buffer;
		int64_t end_time = *(int64_t*)(buffer + sizeof(int64_t));

		return std::make_pair(start_time, end_time);
	}

	void send_data(const std::vector<int>& dt)
	{
		size_t size = dt.size() * sizeof(int);
		this->socket().sendBytes(&size, sizeof(size));
		this->socket().sendBytes(dt.data(), size);
	}

private:
	std::shared_ptr<Storage> storage;
};

class ServerConnectionFactory: public TCPServerConnectionFactory
{
public:
	ServerConnectionFactory(std::shared_ptr<Storage> storage) : 
		storage(storage)
	{}

	TCPServerConnection* createConnection(const StreamSocket& socket) override
	{
		return new ServerConnection(socket, storage);
	}

private:
	std::shared_ptr<Storage> storage;
};

class Server: public ServerApplication
{
protected:

	int main(const std::vector<std::string>&)
	{
		std::shared_ptr<Storage> storage = std::make_shared<Storage>();
		Generator generator(storage);
		generator.start();

		// get parameters from configuration file
		unsigned short port = (unsigned short)config().getInt("Server.port", 9191);

		// set-up a server socket
		ServerSocket svs(port);
		//svs.setBlocking(false);

		// set-up a TCPServer instance
		TCPServer srv(new ServerConnectionFactory(storage), svs);
		// start the TCPServer
		srv.start();

		std::cout << "Server started on port: " << port << std::endl;

		// wait for CTRL-C or kill
		waitForTerminationRequest();

		generator.stop();

		// Stop the TCPServer
		srv.stop();
		
		return Application::EXIT_OK;
	}
};

#endif //SERVER_HPP_