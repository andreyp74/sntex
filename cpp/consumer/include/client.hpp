#ifndef CLIENT_HPP_
#define CLIENT_HPP_

#include <string>
#include <iostream>
#include <thread>
#include <atomic>
#include <chrono>

#include "Poco/Net/SocketAddress.h"
#include "Poco/Net/StreamSocket.h"

class Client
{
public:
    Client(const std::string& host, int port):
        socket_addr(host, port), 
        host_(host), 
        port_(port),
        done(false)
    {
    }

    ~Client()
    {
        stop();
    }

    void start()
    {
        if (! connect())
            return;
        client_thread = std::thread(&Client::run, this);
    }

    void stop()
    {
        done = true;
        if (client_thread.joinable())
            client_thread.join();
    }

    void run()
    {
        while(!done)
        {
            try
            {
                exec_round_trip();
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
            catch(Poco::Exception err)
            {
                std::cerr << "Error occurred: " << err.what() << std::endl;
            }
        }
    }

    bool connect()
    {
        try {
            socket.connect(socket_addr); 
            std::cout << "Connected to " << host_ << ":" << port_ << std::endl;
            return true;
        }
        catch(Poco::Exception err) {
            std::cerr << "Connection failed: " << err.what() << std::endl;
        }
        return false;
    }

private:
    Client(Client&) = delete;
    Client& operator=(Client&) = delete;

    void exec_round_trip()
    {
        std::cout << "Requesting ... " << std::endl;
        std::vector<std::int64_t> message;
        message.push_back(3);
        message.push_back(0);
        message.push_back(0);
        //socket.sendBytes(message.data(), (int)message.size()); 
        socket.sendBytes(message.data(), (int)message.size()); 

        std::vector<int> resp;
		char buffer[1024];
		int received_bytes = 0;

        received_bytes = socket.receiveBytes(buffer, sizeof(buffer));
        decltype(resp.size()) size;
        //read size

		while (! received_bytes)
		{
			received_bytes = socket.receiveBytes(buffer, sizeof(buffer));
			if (received_bytes)
				resp.insert(resp.end(), std::make_move_iterator(buffer), std::make_move_iterator(buffer + received_bytes));
		}

        std::cout << "Receive values: " << std::endl;
        for (auto v: resp)
        {
            std::cout << v << std::endl;
        }
    }

private:
    // IP endpoint/socket address (consists of host addr and port #)
	Poco::Net::SocketAddress socket_addr;

	// Interface to a TCP stream socket
	Poco::Net::StreamSocket socket;

    std::thread client_thread;

    std::string host_;
    int port_;

    std::atomic<bool> done;
};

#endif //CLIENT_HPP_