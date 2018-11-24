#include "Transceiver.hpp"

HockeyNet::TransceiverPtr HockeyNet::Transceiver::Create(boost::asio::io_service& service)
{
	TransceiverPtr transceiver{ new Transceiver{ service } };
	return transceiver;
}

HockeyNet::Transceiver::Transceiver(boost::asio::io_service& service)
	: receiver_{ new Receiver{ service } }
	, transmitter_{ new Transmitter{ receiver_->Sock() } }
{
	receiver_->transceiverClose_ = boost::bind(&Transmitter::SafeClose_, transmitter_, _1);
}

void HockeyNet::Transceiver::SetErrorHandler(OnErrorHandler onErrorHandler)
{
	receiver_->SetErrorHandler(onErrorHandler);
	transmitter_->SetErrorHandler(onErrorHandler);
}

void HockeyNet::Transceiver::SetAnswerHandler(OnAnswerHandler onAnswerHandler)
{
	receiver_->SetAnswerHandler(onAnswerHandler);
}

void HockeyNet::Transceiver::SetCloseHandler(OnCloseHandler onCloseHandler)
{
	receiver_->SetCloseHandler(onCloseHandler);
	transmitter_->SetCloseHandler(onCloseHandler);
}

HockeyNet::SocketPtr HockeyNet::Transceiver::Sock()
{
	return transmitter_->Sock();
}

void HockeyNet::Transceiver::Connect(boost::asio::ip::tcp::endpoint const& ep)
{
	transmitter_->Connect(ep);
}

void HockeyNet::Transceiver::AsyncConnect(boost::asio::ip::tcp::endpoint const& ep)
{
	transmitter_->AsyncConnect(ep);
}

void HockeyNet::Transceiver::StartReading()
{
	receiver_->StartReading();
}

void HockeyNet::Transceiver::Send(std::string const& message)
{
	transmitter_->Send(message);
}

void HockeyNet::Transceiver::Close()
{
	transmitter_->SafeClose_(TRANSCEIVER_END);
	while (transmitter_->sock_->is_open());
}
