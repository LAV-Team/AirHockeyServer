#ifndef __RECEIVER_INCLUDED__
#define __RECEIVER_INCLUDED__

#include "Global.hpp"

class Receiver
	: public boost::enable_shared_from_this<Receiver>
	, boost::noncopyable
{
public:
	static ReceiverPtr Create(boost::asio::io_service& service);
	static ReceiverPtr Create(SocketPtr socket);
	void SetErrorHandler(OnErrorHandler onErrorHandler);
	void SetAnswerHandler(OnAnswerHandler onAnswerHandler);
	void SetCloseHandler(OnCloseHandler onCloseHandler);

	SocketPtr Sock();

	void StartReading();
	void StopReading();
	void Close();

private:
	typedef Receiver SelfType;
	friend class Transceiver;

	SocketPtr sock_;
	OnErrorHandler onErrorHandler_;
	OnAnswerHandler onAnswerHandler_;
	OnCloseHandler onCloseHandler_;
	boost::function<void(char)> transceiverClose_;

	bool isStarted_;
	char readBuffer_[BUFFER_LENGTH];
	std::string message_;

	explicit Receiver(boost::asio::io_service& service);
	explicit Receiver(SocketPtr socket);

	void Read_();
	size_t IsReadingCompleted_(boost::system::error_code const& error, size_t bytes);
	void OnRead_(boost::system::error_code const& error, size_t bytes);

	void CloseSocket_();
};

#endif // __RECEIVER_INCLUDED__
