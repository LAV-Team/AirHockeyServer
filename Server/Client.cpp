#include "Client.hpp"

HockeyNet::ClientPtr HockeyNet::Client::Create(boost::asio::io_service& service)
{
	ClientPtr client{ new Client{ service } };
	return client;
}

HockeyNet::Client::Client(boost::asio::io_service& service)
	: transceiver_{ Transceiver::Create(service) }
	, sessionId_{}
	, shortSessionId_{}
{}

void HockeyNet::Client::SetErrorHandler(OnErrorHandler onErrorHandler)
{
	transceiver_->SetErrorHandler(onErrorHandler);
}

void HockeyNet::Client::SetAnswerHandler(OnAnswerHandler onAnswerHandler)
{
	transceiver_->SetAnswerHandler(onAnswerHandler);
}

void HockeyNet::Client::SetCloseHandler(OnCloseHandler onCloseHandler)
{
	transceiver_->SetCloseHandler(onCloseHandler);
}

HockeyNet::TransceiverPtr HockeyNet::Client::GetTransceiver()
{
	return transceiver_;
}

std::string HockeyNet::Client::GetSessionId() const
{
	return sessionId_;
}

std::string HockeyNet::Client::GetShortSessionId() const
{
	return shortSessionId_;
}

HockeyNet::ClientPtr HockeyNet::Client::GetAnotherClient() const
{
	return anotherClient_;
}

void HockeyNet::Client::SetSessionId(std::string sessionId)
{
	sessionId_ = sessionId;
	shortSessionId_ = std::string{ sessionId.begin(), sessionId.begin() + SHORT_SESSION_ID_LENGTH };
}

void HockeyNet::Client::SetAnotherClient(ClientPtr anotherClient)
{
	anotherClient_ = anotherClient;
}

void HockeyNet::Client::ClearSessionId()
{
	sessionId_.clear();
	shortSessionId_.clear();
}

void HockeyNet::Client::ClearAnotherClient()
{
	anotherClient_.reset();
}

boost::asio::ip::tcp::socket& HockeyNet::Client::Sock()
{
	return *(transceiver_->Sock());
}

bool HockeyNet::Client::IsOpen() const
{
	return transceiver_->Sock()->is_open();
}

void HockeyNet::Client::StartReading()
{
	transceiver_->StartReading();
}

void HockeyNet::Client::Send(std::string const& message)
{
	transceiver_->Send(message);
}

void HockeyNet::Client::Close()
{
	transceiver_->Close();
}
