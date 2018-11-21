#include "Transmitter.hpp"

Transmitter::SharedPtr Transmitter::Create(boost::asio::io_service& service)
{
	SharedPtr transmitter{ new Transmitter{ service } };
	return transmitter;
}

Transmitter::Transmitter(boost::asio::io_service& service)
	: closed_{ false }
	, sock_{ service }
	, errorHandler_{}
	, transfersCount_{ 0U }
{}

void Transmitter::SetErrorHandler(ErrorHandler errorHandler)
{
	errorHandler_ = errorHandler;
}

boost::asio::ip::tcp::socket& Transmitter::Sock()
{
	return sock_;
}

void Transmitter::Connect(boost::asio::ip::tcp::endpoint const& ep)
{
	sock_.connect(ep);
}

void Transmitter::AsyncConnect(boost::asio::ip::tcp::endpoint const& ep)
{
	sock_.async_connect(ep, BIND_WITH_1_ARG(OnConnect_, _1));
}

void Transmitter::Send(std::string const& message)
{
	if (!closed_) {
		Write_(message);
	}
}

void Transmitter::Close()
{
	closed_ = true;
	while (transfersCount_ > 0);
	Write_(std::string{ MESSAGE_END });
	while (sock_.is_open());
}

void Transmitter::OnConnect_(const boost::system::error_code& error)
{
	if (error) {
		sock_.close();
		if (errorHandler_) errorHandler_(error);
		return;
	}
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
		sock_.close();
		if (errorHandler_) errorHandler_(error);
		return;
	}
	if (isEnd) {
		sock_.close();
	}
}
