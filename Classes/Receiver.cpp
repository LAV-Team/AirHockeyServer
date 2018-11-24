#include "Receiver.hpp"

HockeyNet::ReceiverPtr HockeyNet::Receiver::Create(boost::asio::io_service& service)
{
	ReceiverPtr receiver{ new Receiver{ service } };
	return receiver;
}

HockeyNet::ReceiverPtr HockeyNet::Receiver::Create(SocketPtr socket)
{
	ReceiverPtr receiver{ new Receiver{ socket } };
	return receiver;
}

HockeyNet::Receiver::Receiver(boost::asio::io_service& service)
	: sock_{ SocketPtr { new boost::asio::ip::tcp::socket{ service } } }
	, onErrorHandler_{}
	, onAnswerHandler_{}
	, onCloseHandler_{}
	, transceiverClose_{}
	, isStarted_{ false }
	, message_{}
{}

HockeyNet::Receiver::Receiver(SocketPtr socket)
	: sock_{ socket }
	, onErrorHandler_{}
	, onAnswerHandler_{}
	, onCloseHandler_{}
	, transceiverClose_{}
	, isStarted_{ false }
	, message_{}
{}

void HockeyNet::Receiver::SetErrorHandler(OnErrorHandler onErrorHandler)
{
	onErrorHandler_ = onErrorHandler;
}

void HockeyNet::Receiver::SetAnswerHandler(OnAnswerHandler onAnswerHandler)
{
	onAnswerHandler_ = onAnswerHandler;
}

void HockeyNet::Receiver::SetCloseHandler(OnCloseHandler onCloseHandler)
{
	onCloseHandler_ = onCloseHandler;
}

HockeyNet::SocketPtr HockeyNet::Receiver::Sock()
{
	return sock_;
}

void HockeyNet::Receiver::StartReading()
{
	if (isStarted_ || !sock_->is_open()) {
		return;
	}
	isStarted_ = true;

	message_.clear();
	Read_();
}

void HockeyNet::Receiver::StopReading()
{
	if (!isStarted_) {
		return;
	}
	isStarted_ = false;

	sock_->cancel();
}

void HockeyNet::Receiver::Close()
{
	if (!sock_->is_open()) {
		return;
	}

	sock_->close();
	if (onCloseHandler_) {
		onCloseHandler_();
	}
}

void HockeyNet::Receiver::Read_()
{
	if (!sock_->is_open()) {
		return;
	}

	boost::asio::async_read(
		*sock_,
		boost::asio::buffer(readBuffer_),
		BIND(IsReadingCompleted_, _1, _2),
		BIND(OnRead_, _1, _2)
	);
}

size_t HockeyNet::Receiver::IsReadingCompleted_(boost::system::error_code const& error, size_t bytes)
{
	return error || (bytes > 0 && IS_END(readBuffer_[bytes - 1])) ? 0 : 1;
}

void HockeyNet::Receiver::OnRead_(boost::system::error_code const& error, size_t bytes)
{
	if (error) {
		if (onErrorHandler_) {
			onErrorHandler_(error);
		}
		Close();
		return;
	}

	message_ += std::string{ readBuffer_, bytes };

	char lastChar{ message_.back() };
	if (IS_END(lastChar)) {
		message_.pop_back();
		if (!message_.empty()) {
			if (onAnswerHandler_) {
				onAnswerHandler_(message_);
			}
		}
		if (IS_STREAM_END(lastChar)) {
			if (transceiverClose_ && lastChar == TRANSCEIVER_END) {
				transceiverClose_(TRANSMITTER_END);
			}
			else {
				Close();
			}
		}
		else {
			message_.clear();
			Read_();
		}
	}
	else {
		Read_();
	}
}
