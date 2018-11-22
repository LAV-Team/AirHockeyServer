#include "Client.hpp"

Client::ClientPtr Client::Create(boost::asio::io_service& service)
{
	ClientPtr client{ new Client{ service } };
	return client;
}

Client::Client(boost::asio::io_service& service)
	: transceiver_{ Transceiver::Create(service) }
	, sessionId_{}
	, shortSessionId_{}
{}

void Client::SetErrorHandler(OnErrorHandler onErrorHandler)
{
	transceiver_->SetErrorHandler(onErrorHandler);
}

void Client::SetAnswerHandler(OnAnswerHandler onAnswerHandler)
{
	transceiver_->SetAnswerHandler(onAnswerHandler);
}

void Client::SetCloseHandler(OnCloseHandler onCloseHandler)
{
	transceiver_->SetCloseHandler(onCloseHandler);
}

TransceiverPtr Client::GetTransceiver()
{
	return transceiver_;
}

std::string Client::GetSessionId() const
{
	return sessionId_;
}

std::string Client::GetShortSessionId() const
{
	return shortSessionId_;
}

Client::ClientPtr Client::GetAnotherClient() const
{
	return anotherClient_;
}

void Client::SetSessionId(std::string sessionId)
{
	sessionId_ = sessionId;
	shortSessionId_ = std::string{ sessionId.begin(), sessionId.begin() + SHORT_SESSION_ID_LENGTH };
}

void Client::SetAnotherClient(ClientPtr anotherClient)
{
	anotherClient_ = anotherClient;
}

void Client::ClearSessionId()
{
	sessionId_.clear();
	shortSessionId_.clear();
}

void Client::ClearAnotherClient()
{
	anotherClient_.reset();
}

boost::asio::ip::tcp::socket& Client::Sock()
{
	return *(transceiver_->Sock());
}

bool Client::IsOpen() const
{
	return transceiver_->Sock()->is_open();
}

void Client::StartReading()
{
	transceiver_->StartReading();
}

void Client::Send(std::string const& message)
{
	transceiver_->Send(message);
}

void Client::Close()
{
	transceiver_->Close();
}
