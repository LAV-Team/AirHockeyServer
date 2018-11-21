#ifndef __CLIENT_INCLUDED__
#define __CLIENT_INCLUDED__

#include <iostream>
#include <string>
#include "../Classes/Transceiver.hpp"

class Network
	: public boost::enable_shared_from_this<Network>
	, boost::noncopyable
{
public:
	typedef boost::shared_ptr<Network> SharedPtr;
	typedef Transceiver::AnswerHandler AnswerHandler;
	typedef Transceiver::ErrorHandler ErrorHandler;

	static SharedPtr Create();
	void SetErrorHandler(ErrorHandler errorHandler);
	void SetAnswerHandler(AnswerHandler answerHandler);

	bool Connect(boost::asio::ip::tcp::endpoint const& ep);
	void Disconnect();

	void StartReading();

	void Send(std::string const& message);

private:
	typedef Network SelfType;

	bool isStarted_;
	boost::asio::io_service service_;
	std::thread serviceThread_;
	Transceiver::SharedPtr transceiver_;

	Network();
};

#endif // __CLIENT_INCLUDED__
