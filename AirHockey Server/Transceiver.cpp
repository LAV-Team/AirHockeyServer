#include "Transceiver.hpp"

Transceiver::SharedPtr Transceiver::Create(boost::asio::io_service& service)
{
	SharedPtr transceiver{ new Transceiver{ service } };
	return transceiver;
}

void Transceiver::SetErrorHandler(ErrorHandler errorHandler)
{
	errorHandler_ = errorHandler;
}

void Transceiver::SetAnswerHandler(AnswerHandler answerHandler)
{
	answerHandler_ = answerHandler;
}

void Transceiver::SetSessionId(std::string sessionId)
{
	sessionId_ = sessionId;
}

std::string Transceiver::GetSessionId() const
{
	return sessionId_;
}

Transceiver::Transceiver(boost::asio::io_service& service)
	: closed_{ false }
	, sock_{ service }
	, transfersCount_{ 0U }
	, isReadingStarted_{ false }
{}

boost::asio::ip::tcp::socket& Transceiver::Sock()
{
	return sock_;
}

void Transceiver::Connect(boost::asio::ip::tcp::endpoint const& ep)
{
	sock_.async_connect(ep, BIND_WITH_1_ARG(OnConnect_, _1));
}

void Transceiver::Cancel()
{
	closed_ = true;
	boost::thread{ BIND(Cancel_) };
}

void Transceiver::Close()
{
	sock_.close();
}

void Transceiver::StartReading()
{
	if (isReadingStarted_ || closed_) {
		return;
	}
	isReadingStarted_ = true;

	message_.clear();
	Read_();
}

void Transceiver::StopReading()
{
	if (!isReadingStarted_ || closed_) {
		return;
	}
	isReadingStarted_ = false;

	sock_.close();
}

void Transceiver::Send(std::string const& message)
{
	if (!closed_) {
		Write_(message);
	}
}

void Transceiver::OnConnect_(const boost::system::error_code& error)
{
	if (error) {
		Close();
		if (errorHandler_) errorHandler_(error);
		return;
	}
}

void Transceiver::Cancel_()
{
	while (transfersCount_ > 0);
	Write_(std::string{ MESSAGE_END });
}

void Transceiver::Write_(std::string const& message)
{
	bool isEnd{ message.back() == MESSAGE_END };

	std::string copy{ message };
	if (!isEnd) {
		copy += COMMAND_END;
	}

	++transfersCount_;
	sock_.async_write_some(
		boost::asio::buffer(copy.c_str(), copy.size()),
		BIND_WITH_3_ARGS(OnWrite_, _1, _2, isEnd)
	);
}

void Transceiver::OnWrite_(boost::system::error_code const& error, size_t bytes, bool isEnd)
{
	--transfersCount_;
	if (error) {
		Close();
		if (errorHandler_) errorHandler_(error);
		return;
	}
	if (isEnd) {
		Close();
	}
}

void Transceiver::Read_()
{
	if (!isReadingStarted_) {
		return;
	}

	boost::asio::async_read(
		sock_,
		boost::asio::buffer(readBuffer_),
		BIND_WITH_2_ARGS(IsReadingCompleted_, _1, _2),
		BIND_WITH_2_ARGS(OnRead_, _1, _2)
	);
}

size_t Transceiver::IsReadingCompleted_(boost::system::error_code const& error, size_t bytes)
{
	return error || (bytes > 0 && (
		readBuffer_[bytes - 1] == COMMAND_END || readBuffer_[bytes - 1] == MESSAGE_END
		)) ? 0 : 1;
}

void Transceiver::OnRead_(boost::system::error_code const& error, size_t bytes)
{
	if (error) {
		StopReading();
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
			StopReading();
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
