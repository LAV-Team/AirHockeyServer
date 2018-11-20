#ifndef __SERVER_INCLUDED__
#define __SERVER_INCLUDED__

#include <iostream>
#include <list>
#include <map>
#include "Client.hpp"

class Server
	: public boost::enable_shared_from_this<Server>
	, boost::noncopyable
{
public:
	typedef boost::shared_ptr<Server> ServerPtr;
	static ServerPtr Create(unsigned short port);

	void Start();
	void Stop();

private:
	typedef Server SelfType;

	bool isStarted_;
	boost::asio::io_service service_;
	std::thread serviceThread_;
	boost::asio::ip::tcp::acceptor acceptor_;

	std::list<Client::ClientPtr> clients_;
	std::string lastSessionId_;
	std::map<std::string, std::pair<Client::ClientPtr, Client::ClientPtr>> sessions_;

	Server(unsigned short port);

	std::string GenerateSessionId_();
	void CreateTransceiver_();
	
	void AcceptHandler_(Client::ClientPtr client, boost::system::error_code const& error);
	void ErrorHandler_(Client::ClientPtr client, boost::system::error_code const& error);
	void AnswerHandler_(Client::ClientPtr client, std::string const& answer);
	void CloseHandler_(Client::ClientPtr client);
};

#endif // __SERVER_INCLUDED__
