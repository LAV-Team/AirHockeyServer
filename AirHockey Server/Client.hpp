#ifndef __CLIENT_INCLUDED__
#define __CLIENT_INCLUDED__

#include <iostream>
#include <string>
#include "Transceiver.hpp"

class Client
	: public boost::enable_shared_from_this<Client>
	, boost::noncopyable
{
public:
	typedef boost::shared_ptr<Client> SharedPtr;

	static SharedPtr Create(boost::asio::ip::tcp::endpoint ep);

	void Start();
	void Stop();

private:
	typedef Client SelfType;

	bool isStarted_;
	boost::asio::io_service service_;
	Transceiver::SharedPtr transceiver_;
	std::thread sendingThread_;

	Client();

	void StartSending_();
	void StartAsyncSending_();

	void ErrorHandler_(boost::system::error_code const& error);
	void AnswerHandler_(std::string const& answer);
};

#endif // __CLIENT_INCLUDED__
