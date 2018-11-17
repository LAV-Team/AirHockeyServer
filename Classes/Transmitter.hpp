#ifndef __TRANSMITTER_INCLUDED__
#define __TRANSMITTER_INCLUDED__

#include "Global.hpp"

class Transmitter
	: public boost::enable_shared_from_this<Transmitter>
	, boost::noncopyable
{
public:
	typedef boost::shared_ptr<Transmitter> SharedPtr;
	typedef boost::function<void(boost::system::error_code const&)> ErrorHandler;

	static SharedPtr Create(boost::asio::io_service& service);
	void SetErrorHandler(ErrorHandler errorHandler);
	
	boost::asio::ip::tcp::socket& Sock();

	void Connect(boost::asio::ip::tcp::endpoint const& ep);
	void AsyncConnect(boost::asio::ip::tcp::endpoint const& ep);
	void Send(std::string const& message);
	void Close();

private:
	typedef Transmitter SelfType;

	bool closed_;
	boost::asio::ip::tcp::socket sock_;
	ErrorHandler errorHandler_;
	size_t transfersCount_;

	Transmitter(boost::asio::io_service& service);

	void OnConnect_(const boost::system::error_code& error);

	void Write_(std::string const& message);
	void OnWrite_(boost::system::error_code const& error, size_t bytes, bool isEnd = false);
};

#endif // __TRANSMITTER_INCLUDED__
