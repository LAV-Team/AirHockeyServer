#include "Transmitter.hpp"

Transmitter::SharedPtr Transmitter::Create(boost::asio::io_service& service, 
	boost::asio::ip::tcp::endpoint const& ep)
{
	SharedPtr transmitter{ new Transmitter{ service } };
	transmitter->Connect_(ep);
	return transmitter;
}

Transmitter::Transmitter(boost::asio::io_service& service)
	: sock_{ service }
	, transfersCount_{ 0U }
	, closed_{ false }
{}

void Transmitter::SetErrorHandler(ErrorHandler errorHandler)
{
	errorHandler_ = errorHandler;
}

void Transmitter::SetSessionId(std::string sessionId)
{
	sessionId_ = sessionId;
}

std::string Transmitter::GetSessionId() const
{
	return sessionId_;
}

boost::asio::ip::tcp::socket& Transmitter::Sock()
{
	return sock_;
}

void Transmitter::Send(std::string const& message)
{
	if (!closed_) {
		Write_(message);
	}
}

void Transmitter::Cancel()
{
	closed_ = true;
	boost::thread{ BIND(Cancel_) };
}

void Transmitter::Close()
{
	sock_.close();
}

void Transmitter::Connect_(boost::asio::ip::tcp::endpoint const& ep)
{
	sock_.async_connect(ep, BIND_WITH_1_ARG(OnConnect_, _1));
}

void Transmitter::OnConnect_(const boost::system::error_code& error)
{
	if (error) {
		Close();
		if (errorHandler_) errorHandler_(error);
		return;
	}
}

void Transmitter::Cancel_()
{
	while (transfersCount_ > 0);
	Write_(std::string{ MESSAGE_END });
}

void Transmitter::Write_(std::string const& message)
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

void Transmitter::OnWrite_(boost::system::error_code const& error, size_t bytes, bool isEnd)
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
