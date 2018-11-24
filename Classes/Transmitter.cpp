#include "Transmitter.hpp"

HockeyNet::TransmitterPtr HockeyNet::Transmitter::Create(boost::asio::io_service& service)
{
	TransmitterPtr transmitter{ new Transmitter{ service } };
	return transmitter;
}

HockeyNet::TransmitterPtr HockeyNet::Transmitter::Create(SocketPtr socket)
{
	TransmitterPtr transmitter{ new Transmitter{ socket } };
	return transmitter;
}

HockeyNet::Transmitter::Transmitter(boost::asio::io_service& service)
	: closed_{ false }
	, sock_{ SocketPtr { new boost::asio::ip::tcp::socket{ service } } }
	, transfersCount_{ 0U }
	, onErrorHandler_{}
	, onCloseHandler_{}
{}

HockeyNet::Transmitter::Transmitter(SocketPtr socket)
	: closed_{ false }
	, sock_{ socket }
	, transfersCount_{ 0U }
	, onErrorHandler_{}
	, onCloseHandler_{}
{}

void HockeyNet::Transmitter::SetErrorHandler(OnErrorHandler onErrorHandler)
{
	onErrorHandler_ = onErrorHandler;
}

void HockeyNet::Transmitter::SetCloseHandler(OnCloseHandler onCloseHandler)
{
	onCloseHandler_ = onCloseHandler;
}

HockeyNet::SocketPtr HockeyNet::Transmitter::Sock()
{
	return sock_;
}

void HockeyNet::Transmitter::Connect(boost::asio::ip::tcp::endpoint const& ep)
{
	if (!sock_->is_open()) {
		sock_->connect(ep);
	}
}

void HockeyNet::Transmitter::AsyncConnect(boost::asio::ip::tcp::endpoint const& ep)
{
	if (!sock_->is_open()) {
		sock_->async_connect(ep, BIND(OnConnect_, _1));
	}
}

void HockeyNet::Transmitter::Send(std::string const& message)
{
	if (!closed_ && sock_->is_open()) {
		Write_(message);
	}
}

void HockeyNet::Transmitter::Close()
{
	if (!closed_ && sock_->is_open()) {
		SafeClose_(TRANSMITTER_END);
		while (sock_->is_open());
	}
}

void HockeyNet::Transmitter::OnConnect_(const boost::system::error_code& error)
{
	if (error) {
		sock_->close();
		if (onErrorHandler_) { 
			onErrorHandler_(error); 
		}
		return;
	}
}

void HockeyNet::Transmitter::Write_(std::string const& message)
{
	if (!sock_->is_open()) {
		return;
	}

	std::string copy{ message };
	if (!IS_STREAM_END(copy.back())) {
		copy += COMMAND_END;
	}

	++transfersCount_;
	sock_->async_write_some(
		boost::asio::buffer(copy.c_str(), copy.size()),
		BIND(OnWrite_, _1, _2, copy.back())
	);
}

void HockeyNet::Transmitter::OnWrite_(boost::system::error_code const& error, size_t bytes, char lastChar)
{
	--transfersCount_;
	if (error) {
		CloseSocket_();
		if (onErrorHandler_) { 
			onErrorHandler_(error); 
		}
		return;
	}
	if (lastChar == TRANSMITTER_END) {
		CloseSocket_();
	}
}

void HockeyNet::Transmitter::SafeClose_(char endChar)
{
	if (closed_) {
		return;
	}
	closed_ = true;

	while (transfersCount_ > 0);
	Write_(std::string{ endChar });
}

void HockeyNet::Transmitter::CloseSocket_()
{
	if (!sock_->is_open()) {
		return;
	}

	sock_->close();
	if (onCloseHandler_) {
		onCloseHandler_();
	}
}
