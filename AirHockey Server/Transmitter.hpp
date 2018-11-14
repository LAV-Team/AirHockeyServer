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

	static SharedPtr Create(boost::asio::io_service& service, boost::asio::ip::tcp::endpoint const& ep);
	void SetErrorHandler(ErrorHandler errorHandler);
	
	void SetSessionId(std::string sessionId);
	std::string GetSessionId() const;

	boost::asio::ip::tcp::socket& Sock();

	void Send(std::string const& message);
	void Cancel();
	void Close();

private:
	typedef Transmitter SelfType;

	std::string sessionId_;
	boost::asio::ip::tcp::socket sock_;
	ErrorHandler errorHandler_;
	size_t transfersCount_;
	bool closed_;

	Transmitter(boost::asio::io_service& service);

	void Connect_(boost::asio::ip::tcp::endpoint const& ep);
	void OnConnect_(const boost::system::error_code& error);
	void Cancel_();

	void Write_(std::string const& message);
	void OnWrite_(boost::system::error_code const& error, size_t bytes, bool isEnd = false);
};

#endif // __TRANSMITTER_INCLUDED__
