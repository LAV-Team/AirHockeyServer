#ifndef __TRANSCEIVER_INCLUDED__
#define __TRANSCEIVER_INCLUDED__

#include "Global.hpp"

class Transceiver
	: public boost::enable_shared_from_this<Transceiver>
	, boost::noncopyable
{
public:
	typedef boost::shared_ptr<Transceiver> SharedPtr;
	typedef boost::function<void(std::string const&)> AnswerHandler;
	typedef boost::function<void(boost::system::error_code const&)> ErrorHandler;

	static SharedPtr Create(boost::asio::io_service& service);
	void SetErrorHandler(ErrorHandler errorHandler);
	void SetAnswerHandler(AnswerHandler answerHandler);

	boost::asio::ip::tcp::socket& Sock();
	
	void Connect(boost::asio::ip::tcp::endpoint const& ep);
	void Cancel();
	void Close();

	void StartReading();
	void StopReading();

	void Send(std::string const& message);

private:
	typedef Transceiver SelfType;

	bool closed_;
	boost::asio::ip::tcp::socket sock_;
	ErrorHandler errorHandler_;

	size_t transfersCount_;

	bool isReadingStarted_;
	char readBuffer_[BUFFER_LENGTH];
	std::string message_;
	AnswerHandler answerHandler_;

	Transceiver(boost::asio::io_service& service);

	void OnConnect_(const boost::system::error_code& error);
	void Cancel_();

	void Write_(std::string const& message);
	void OnWrite_(boost::system::error_code const& error, size_t bytes, bool isEnd = false);

	void Read_();
	size_t IsReadingCompleted_(boost::system::error_code const& error, size_t bytes);
	void OnRead_(boost::system::error_code const& error, size_t bytes);
};

#endif // __TRANSCEIVER_INCLUDED__
