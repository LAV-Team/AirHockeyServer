#include "Network.hpp"

Network::NetworkPtr Network::Create()
{
	NetworkPtr network{ new Network{} };
	return network;
}

Network::Network()
	: isStarted_{ false }
	, service_{}
	, serviceThread_{}
	, transceiver_{ Transceiver::Create(service_) }
{}

void Network::SetErrorHandler(OnErrorHandler onErrorHandler)
{
	transceiver_->SetErrorHandler(onErrorHandler);
}

void Network::SetAnswerHandler(OnAnswerHandler onAnswerHandler)
{
	transceiver_->SetAnswerHandler(onAnswerHandler);
}

void Network::SetCloseHandler(OnCloseHandler onCloseHandler)
{
	transceiver_->SetCloseHandler(onCloseHandler);
}

bool Network::IsConnected()
{
	return transceiver_->Sock()->is_open();
}

bool Network::Connect(boost::asio::ip::tcp::endpoint const& ep)
{
	try {
		transceiver_->Connect(ep);
		return true;
	}
	catch(boost::exception&) {
		return false;
	}
}

void Network::Disconnect()
{
	transceiver_->Close();
	service_.stop();
	serviceThread_.join();
}

void Network::StartReading()
{
	if (isStarted_) {
		return;
	}
	isStarted_ = true;

	transceiver_->StartReading();
	serviceThread_ = std::thread{ boost::bind(&boost::asio::io_service::run, &service_) };
}

void Network::Send(std::string const& message)
{
	transceiver_->Send(message);
}
