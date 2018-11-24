#include "Network.hpp"

HockeyNet::NetworkPtr HockeyNet::Network::Create()
{
	NetworkPtr network{ new Network{} };
	return network;
}

HockeyNet::Network::Network()
	: isStarted_{ false }
	, service_{}
	, serviceThread_{}
	, transceiver_{ Transceiver::Create(service_) }
{}

void HockeyNet::Network::SetErrorHandler(OnErrorHandler onErrorHandler)
{
	transceiver_->SetErrorHandler(onErrorHandler);
}

void HockeyNet::Network::SetAnswerHandler(OnAnswerHandler onAnswerHandler)
{
	transceiver_->SetAnswerHandler(onAnswerHandler);
}

void HockeyNet::Network::SetCloseHandler(OnCloseHandler onCloseHandler)
{
	transceiver_->SetCloseHandler(onCloseHandler);
}

bool HockeyNet::Network::IsConnected()
{
	return transceiver_->Sock()->is_open();
}

bool HockeyNet::Network::Connect(boost::asio::ip::tcp::endpoint const& ep)
{
	try {
		transceiver_->Connect(ep);
		return true;
	}
	catch(boost::exception&) {
		return false;
	}
}

void HockeyNet::Network::Disconnect()
{
	transceiver_->Close();
	service_.stop();
	serviceThread_.join();
}

void HockeyNet::Network::StartReading()
{
	if (isStarted_) {
		return;
	}
	isStarted_ = true;

	transceiver_->StartReading();
	serviceThread_ = std::thread{ boost::bind(&boost::asio::io_service::run, &service_) };
}

void HockeyNet::Network::Send(std::string const& message)
{
	transceiver_->Send(message);
}
