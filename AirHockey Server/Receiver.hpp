#ifndef __RECEIVER_INCLUDED__
#define __RECEIVER_INCLUDED__

#include "Global.hpp"

class Receiver
	: public boost::enable_shared_from_this<Receiver>
	, boost::noncopyable
{
public:
	typedef boost::shared_ptr <Receiver> SharedPtr;
	typedef boost::function<void(std::string const&)> AnswerHandler;
	typedef boost::function<void(boost::system::error_code const&)> ErrorHandler;

	static SharedPtr Create(boost::asio::io_service& service);
	void SetErrorHandler(ErrorHandler errorHandler);
	void SetAnswerHandler(AnswerHandler answerHandler);

	void SetSessionId(std::string sessionId);
	std::string GetSessionId() const;

	boost::asio::ip::tcp::socket& Sock();
	bool IsStarted();

	void Start();
	void Stop();

private:
	typedef Receiver SelfType;

	std::string sessionId_;
	boost::asio::ip::tcp::socket sock_;
	bool isStarted_;
	char readBuffer_[BUFFER_LENGTH];
	std::string message_;
	AnswerHandler answerHandler_;
	ErrorHandler errorHandler_;

	Receiver(boost::asio::io_service& service);

	void Read_();
	size_t IsReadingCompleted_(boost::system::error_code const& error, size_t bytes);
	void OnRead_(boost::system::error_code const& error, size_t bytes);
};

#endif // __RECEIVER_INCLUDED__
