#include "Receiver.hpp"

Receiver::SharedPtr Receiver::Create(boost::asio::io_service& service)
{
	SharedPtr receiver{ new Receiver{ service } };
	return receiver;
}

void Receiver::SetErrorHandler(ErrorHandler errorHandler)
{
	errorHandler_ = errorHandler;
}

void Receiver::SetAnswerHandler(AnswerHandler answerHandler)
{
	answerHandler_ = answerHandler;
}

Receiver::Receiver(boost::asio::io_service& service)
	: sock_{ service }
	, isStarted_{ false }
{}

boost::asio::ip::tcp::socket& Receiver::Sock()
{
	return sock_;
}

bool Receiver::IsStarted()
{
	return isStarted_;
}

void Receiver::Start()
{
	if (isStarted_) {
		return;
	}
	isStarted_ = true;

	message_.clear();
	Read_();
}

void Receiver::Stop()
{
	if (!isStarted_) {
		return;
	}
	isStarted_ = false;

	sock_.close();
}

void Receiver::Read_()
{
	if (!isStarted_) {
		return;
	}

	boost::asio::async_read(
		sock_,
		boost::asio::buffer(readBuffer_),
		BIND_WITH_2_ARGS(IsReadingCompleted_, _1, _2),
		BIND_WITH_2_ARGS(OnRead_, _1, _2)
	);
}

size_t Receiver::IsReadingCompleted_(boost::system::error_code const& error, size_t bytes)
{
	return error || (bytes > 0 && (
		readBuffer_[bytes - 1] == COMMAND_END || readBuffer_[bytes - 1] == MESSAGE_END
		)) ? 0 : 1;
}

void Receiver::OnRead_(boost::system::error_code const& error, size_t bytes)
{
	if (error) {
		Stop();
		if (errorHandler_) errorHandler_(error);
		return;
	}

	message_ += std::string{ readBuffer_, bytes };

	if (bool isEnd{ message_.back() == MESSAGE_END }; message_.back() == COMMAND_END || isEnd) {
		message_.pop_back();
		if (!message_.empty()) {
			if (answerHandler_) answerHandler_(message_);
		}
		if (isEnd) {
			Stop();
			return;
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
