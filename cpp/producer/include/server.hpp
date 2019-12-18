#ifndef SERVER_HPP_
#define SERVER_HPP_

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


class ServerConnection: public Poco::Net::TCPServerConnection
	/// This class handles all client connections.
{
public:
	ServerConnection(const Poco::Net::StreamSocket& s/*args*/):
		Poco::Net::TCPServerConnection(s)
	{
	}

	void run()
	{
		Poco::Util::Application& app = Poco::Util::Application::instance();
		app.logger().information("Request from " + this->socket().peerAddress().toString());
		try
		{
			std::string dt;
			socket().sendBytes(dt.data(), (int) dt.length());
		}
		catch (Poco::Exception& exc)
		{
			app.logger().log(exc);
		}
	}

private:

};

class ServerConnectionFactory: public Poco::Net::TCPServerConnectionFactory
{
public:
	ServerConnectionFactory(/*args*/)
	{
	}

	Poco::Net::TCPServerConnection* createConnection(const Poco::Net::StreamSocket& socket)
	{
		return new ServerConnection(socket/*, args*/);
	}

private:
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

		options.addOption(
			Poco::Util::Option("help", "h", "display help information on command line arguments")
				.required(false)
				.repeatable(false));
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
		helpFormatter.setHeader("A server application that serves the current date and time.");
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
			// get parameters from configuration file
			unsigned short port = (unsigned short) config().getInt("Server.port", 9119);

			// set-up a server socket
			Poco::Net::ServerSocket svs(port);
			// set-up a TCPServer instance
			Poco::Net::TCPServer srv(new ServerConnectionFactory(), svs);
			// start the TCPServer
			srv.start();
			// wait for CTRL-C or kill
			waitForTerminationRequest();
			// Stop the TCPServer
			srv.stop();
		}
		return Poco::Util::Application::EXIT_OK;
	}
private:
	bool helpRequested;
};

#endif //SERVER_HPP_