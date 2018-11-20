#include "Client.hpp"

Client::ClientPtr Client::Create(boost::asio::io_service& service)
{
	ClientPtr client{ new Client{ service } };
	return client;
}

Client::Client(boost::asio::io_service& service)
	: transceiver_{ Transceiver::Create(service) }
	, sessionId_{}
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

std::string& Client::GetSessionId()
{
	return sessionId_;
}

std::string Client::GetSessionId() const
{
	return sessionId_;
}

void Client::SetSessionId(std::string sessionId)
{
	sessionId_ = sessionId;
}

TransceiverPtr Client::operator->()
{
	return transceiver_;
}

Client::operator bool()
{
	return static_cast<bool>(transceiver_);
}

boost::asio::ip::tcp::socket& Client::Sock()
{
	return *transceiver_->Sock();
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
