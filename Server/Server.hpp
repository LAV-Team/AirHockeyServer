#ifndef __SERVER_INCLUDED__
#define __SERVER_INCLUDED__

#include <iostream>
#include <string>
#include <map>
#include "../Classes/Transceiver.hpp"

class Server
	: public boost::enable_shared_from_this<Server>
	, boost::noncopyable
{
public:
	typedef boost::shared_ptr<Server> SharedPtr;

	static SharedPtr Create(unsigned short port);

	void Start();
	void Stop();

private:
	typedef Server SelfType;

	bool isStarted_;
	boost::asio::io_service service_;
	boost::asio::ip::tcp::acceptor acceptor_;

	std::string lastSessionId_;
	std::map<std::string, std::pair<Transceiver::SharedPtr, Transceiver::SharedPtr>> clients_;

	Server(unsigned short port);

	std::string GenerateSessionId_();
	void CreateTransceiver_();
	
	void AcceptHandler_(Transceiver::SharedPtr client, boost::system::error_code const& error);
	void ErrorHandler_(Transceiver::SharedPtr client, boost::system::error_code const& error);
	void AnswerHandler_(Transceiver::SharedPtr client, std::string const& answer);
};

#endif // __SERVER_INCLUDED__
