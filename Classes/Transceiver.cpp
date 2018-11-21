#include "Transceiver.hpp"

TransceiverPtr Transceiver::Create(boost::asio::io_service& service)
{
	TransceiverPtr transceiver{ new Transceiver{ service } };
	return transceiver;
}

Transceiver::Transceiver(boost::asio::io_service& service)
	: receiver_{ new Receiver{ service } }
	, transmitter_{ new Transmitter{ receiver_->Sock() } }
{
	receiver_->transceiverClose_ = boost::bind(&Transmitter::SafeClose_, transmitter_, _1);
}

void Transceiver::SetErrorHandler(OnErrorHandler onErrorHandler)
{
	receiver_->SetErrorHandler(onErrorHandler);
	transmitter_->SetErrorHandler(onErrorHandler);
}

void Transceiver::SetAnswerHandler(OnAnswerHandler onAnswerHandler)
{
	receiver_->SetAnswerHandler(onAnswerHandler);
}

void Transceiver::SetCloseHandler(OnCloseHandler onCloseHandler)
{
	receiver_->SetCloseHandler(onCloseHandler);
	transmitter_->SetCloseHandler(onCloseHandler);
}

SocketPtr Transceiver::Sock()
{
	return transmitter_->Sock();
}

void Transceiver::Connect(boost::asio::ip::tcp::endpoint const& ep)
{
	transmitter_->Connect(ep);
}

void Transceiver::AsyncConnect(boost::asio::ip::tcp::endpoint const& ep)
{
	transmitter_->AsyncConnect(ep);
}

void Transceiver::StartReading()
{
	receiver_->StartReading();
}

void Transceiver::Send(std::string const& message)
{
	transmitter_->Send(message);
}

void Transceiver::Close()
{
	transmitter_->SafeClose_(TRANSCEIVER_END);
	while (transmitter_->sock_->is_open());
}
