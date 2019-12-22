#ifndef SERVER_HPP_
#define SERVER_HPP_

#include <atomic>
#include <memory>
#include <assert.h>

#include "Poco/Net/TCPServer.h"
#include "Poco/Net/TCPServerConnection.h"
#include "Poco/Net/TCPServerConnectionFactory.h"
#include "Poco/Net/StreamSocket.h"
#include "Poco/Net/ServerSocket.h"
#include "Poco/Exception.h"
#include "Poco/Util/ServerApplication.h"
#include "Poco/Util/Option.h"
#include "Poco/Util/OptionSet.h"
#include "Poco/Util/HelpFormatter.h"

#include "storage.hpp"
#include "generator.hpp"
#include "proto.hpp"


class ServerConnection: public Poco::Net::TCPServerConnection
	/// This class handles all client connections.
{
public:
	ServerConnection(const Poco::Net::StreamSocket& s, 
		             std::shared_ptr<Storage> storage) : 

		Poco::Net::TCPServerConnection(s),
		storage(storage)
	{
	}

	void run()
	{
		Poco::Util::Application& app = Poco::Util::Application::instance();
		app.logger().information("Request from " + this->socket().peerAddress().toString());

		while(true)
		{
			try
			{	
				auto client_req = receive_request();

				if (client_req.done())
				{
					app.logger().information("Stopping application");
					return;
				}
				else
				{
					app.logger().information("Requested data from: " + std::to_string(client_req.start_time()) + " to " + std::to_string(client_req.end_time()));
					std::vector<int> dt = storage->get_data(client_req.start_time(), client_req.end_time());
					send_response(std::move(dt));
				}
			}
			catch (Poco::Exception& exc)
			{
				app.logger().log(exc);
			}
		}

		app.logger().information("Connection is closing");
	}

private:

	proto::client::Req receive_request()
	{
		std::string request;
		char buffer[1024];
		int received_bytes = 0;
		while (! received_bytes)
		{
			received_bytes = this->socket().receiveBytes(buffer, sizeof(buffer));
			if (received_bytes)
				request.append(buffer, received_bytes);
		}

		assert(received_bytes == 0);

		return proto::client::deserialize(request);
	}

	void send_response(std::vector<int>&& dt)
	{
		proto::server::Resp response(std::move(dt));
		auto data = proto::server::serialize(response);
		this->socket().sendBytes(data.data(), (int) data.size());
	}

private:
	std::shared_ptr<Storage> storage;
};

class ServerConnectionFactory: public Poco::Net::TCPServerConnectionFactory
{
public:
	ServerConnectionFactory(std::shared_ptr<Storage> storage) : 
		storage(storage)
	{
	}

	Poco::Net::TCPServerConnection* createConnection(const Poco::Net::StreamSocket& socket)
	{
		return new ServerConnection(socket, storage);
	}

private:
	std::shared_ptr<Storage> storage;
};

class Server: public Poco::Util::ServerApplication
{
protected:
	void initialize(Poco::Util::Application& self)
	{
		//loadConfiguration(); // load default configuration files, if present
		Poco::Util::ServerApplication::initialize(self);
	}

	void uninitialize()
	{
		Poco::Util::ServerApplication::uninitialize();
	}

	void defineOptions(Poco::Util::OptionSet& options)
	{
		Poco::Util::ServerApplication::defineOptions(options);

		options.addOption(Poco::Util::Option("help", "h", "display help information on command line arguments").required(false).repeatable(false));
		//options.addOption(Poco::Util::Option("period", "p", "generation period of data").required(true).argumentRequired());
	}

	void handleOption(const std::string& name, const std::string& value)
	{
		Poco::Util::ServerApplication::handleOption(name, value);

		if (name == "help")
			helpRequested = true;
	}

	void displayHelp()
	{
		Poco::Util::HelpFormatter helpFormatter(options());
		helpFormatter.setCommand(commandName());
		helpFormatter.setUsage("OPTIONS");
		helpFormatter.setHeader("Producer server application");
		helpFormatter.format(std::cout);
	}

	int main(const std::vector<std::string>& args)
	{
		if (helpRequested)
		{
			displayHelp();
		}
		else
		{
			std::shared_ptr<Storage> storage = std::make_shared<Storage>();
			Generator generator(storage);
			generator.start();

			// get parameters from configuration file
			unsigned short port = (unsigned short) config().getInt("Server.port", 9191);

			// set-up a server socket
			Poco::Net::ServerSocket svs(port);
			// set-up a TCPServer instance
			Poco::Net::TCPServer srv(new ServerConnectionFactory(storage), svs);
			// start the TCPServer
			srv.start();
			// wait for CTRL-C or kill
			waitForTerminationRequest();

			generator.stop();

			// Stop the TCPServer
			srv.stop();
		}
		return Poco::Util::Application::EXIT_OK;
	}
private:
	bool helpRequested;
};

#endif //SERVER_HPP_